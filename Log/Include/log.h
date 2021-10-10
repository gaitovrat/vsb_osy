#ifndef LOG_H
#define LOG_H
enum LogLevel
{
    error = 0,
    info
};

void log(LogLevel level, const char *message);
#endif