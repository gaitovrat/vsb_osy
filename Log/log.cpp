#include "log.h"

#include <stdio.h>

void log(LogLevel level, const char *message)
{
    const char *format[] = {
        "ERROR: %s\n",
        "INFO: %s\n"
    };

    switch (level)
    {
        case error:
            fprintf(stderr, format[level], message);
            break;
        case info:
            fprintf(stdout, format[level], message);
            break;
    }
}