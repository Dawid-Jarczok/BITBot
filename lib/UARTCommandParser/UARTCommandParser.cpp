#include "UARTCommandParser.h"

UARTCommandParser::UARTCommandParser(Stream& stream) : _stream(&stream) {
    memset(_buf, 0, sizeof(_buf));
}

void UARTCommandParser::begin() {

}

// bool UARTCommandParser::on(const char *cmdName, std::function<void(double)> cb) {
//     if (!cmdName || !cb) return false;
//     _doubleHandlers.push_back({String(cmdName), cb});
//     return true;
// }

bool UARTCommandParser::on(const char *cmdName, std::function<void(int32_t)> cb) {
    if (!cmdName || !cb) return false;
    IntHandler h;
    strncpy(h.name, cmdName, UART_CMD_MAX_LEN);
    h.name[UART_CMD_MAX_LEN] = '\0';
    h.cb = cb;
    h.len = strlen(h.name);
    _intHandlers.push_back(h);
    return true;
}

void UARTCommandParser::parseData(const char *data, size_t len) {
    uint32_t startTimestamp = micros();
    _processLine(data, len);
    _lastCmdReadTime = micros() - startTimestamp;
}

void UARTCommandParser::iterate() {
    uint32_t startTimestamp;
    while (_stream && _stream->available()) {
        int c = _stream->read();
        if (c < 0) break;

        // ignore carriage return
        if (c == '\r') continue;

        // normal character: add to buffer
        if (c != '\n') {
            if (_pos < UART_CMD_MAX_LEN) {
                _buf[_pos] = (char)c;
                _pos++;
            } else {
                _buf[_pos] = '\0';
                if (_defaultHandler) _defaultHandler(_errBufferOverflow, _buf);
                _pos = 0;
                memset(_buf, 0, sizeof(_buf));
            }
        } else {
            // end of line: process the command
            if (_pos <= UART_CMD_MAX_LEN) {
                _buf[_pos] = '\0';
                startTimestamp = micros();
                _processLine(_buf, _pos);
                _lastCmdReadTime = micros() - startTimestamp;
            }
            // reset buffer
            _pos = 0;
            memset(_buf, 0, sizeof(_buf));
        }
    }
}

void UARTCommandParser::_trim_span(const char *start, size_t len, const char **out_start, size_t *out_len) {
    const char *s = start;
    const char *e = start + len;
    while (s < e && isspace((unsigned char)*s)) s++;
    while (e > s && isspace((unsigned char)*(e - 1))) e--;
    *out_start = s;
    *out_len = (size_t)(e - s);
}

void UARTCommandParser::_processLine(const char *line, size_t len) {
    if (!line || len == 0) return;

    char *eq = (char *)memchr(line, '=', len);
    if (!eq) {
      if (_defaultHandler) _defaultHandler(_errNoSeparator, line);
      return;
    }

    const char *raw_name = line;
    size_t raw_name_len = (size_t)(eq - line);
    const char *name_trim;
    size_t name_len;
    _trim_span(raw_name, raw_name_len, &name_trim, &name_len);

    char *raw_payload = eq + 1;
    size_t raw_payload_len = len - (size_t)(raw_payload - line);
    const char *payload_trim;
    size_t payload_len;
    _trim_span(raw_payload, raw_payload_len, &payload_trim, &payload_len);

    if (payload_len == 0) {
      if (_defaultHandler) _defaultHandler(name_len ? name_trim : "", "");
      return;
    }

    // Musimy skopiowac payload do tymczasowego bufora z null-terminatorem, poniewaz
    // strtol oczekuje null-terminated string.
    char pbuf[UART_CMD_MAX_LEN + 1];
    size_t copy_len = payload_len;
    if (copy_len > UART_CMD_MAX_LEN) copy_len = UART_CMD_MAX_LEN;
    memcpy(pbuf, payload_trim, copy_len);
    pbuf[copy_len] = '\0';

    char *endptr = nullptr;
    int32_t val = strtol(pbuf, &endptr, 10);
    if (endptr == pbuf || *endptr != '\0') {
      if (_defaultHandler) {
        // błąd parsowania
        pbuf[copy_len] = '\0';
        _defaultHandler(name_len ? name_trim : "", pbuf);
      }
      return;
    }

    // Znajdz handler
    for (auto &h : _intHandlers) {
        if (h.len != name_len) continue;
        if (strncmp(h.name, name_trim, name_len) != 0) continue;
        h.cb(val);
        return;
    }

    // nie znaleziono handlera
    if (_defaultHandler) {
      pbuf[copy_len] = '\0';
      _defaultHandler(name_len ? name_trim : "", pbuf);
    }
}