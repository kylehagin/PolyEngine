#pragma once
#include <cstdio>
#include <cstdarg>

inline void Log(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}
