#pragma once

#include <Arduino.h>
#include <functional>
#include <vector>

#define UART_CMD_MAX_LEN 32
#define UART_CMD_PARSER_TASK_STACK_SIZE 8192

class UARTCommandParser {
public:
    UARTCommandParser(Stream& stream);

    void begin();
    // bool on(const char *cmdName, std::function<void(double)> cb);
    bool on(const char *cmdName, std::function<void(int32_t)> cb);

    void parseData(const char *data, size_t len);

    void setDefaultHandler(std::function<void(const char *, const char *)> cb) {_defaultHandler = cb;}

    uint32_t getLastCmdReadTime() const { return _lastCmdReadTime; }
    void iterate();
private:
    
    static void _trim_span(const char *start, size_t len, const char **out_start, size_t *out_len);
    void _processLine(const char *line, size_t len);

    // struct DoubleHandler { String name; std::function<void(double)> cb; };
    struct IntHandler { char name[UART_CMD_MAX_LEN + 1]; size_t len; std::function<void(int32_t)> cb; };

    Stream *_stream;
    char _buf[UART_CMD_MAX_LEN + 1];
    uint8_t _pos = 0;
    // std::vector<DoubleHandler> _doubleHandlers;
    std::vector<IntHandler> _intHandlers;
    std::function<void(const char *, const char *)> _defaultHandler;

    static constexpr const char* _errBufferOverflow = "<BUFFER_OVERFLOW>";
    static constexpr const char* _errNoSeparator = "<NO_SEP>";

    uint32_t _lastCmdReadTime = 0;
};