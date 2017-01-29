#pragma once

#include <stdarg.h>
#include "types.h"

#ifdef __GNUC__
#define ATTRIBUTE_FORMAT(m,n) __attribute__((format(printf, m, n)))
#else
#define ATTRIBUTE_FORMAT(m,n)
#endif

void vpanic(char const *format, va_list ap);

void panic(char const *format, ...)
    ATTRIBUTE_FORMAT(1, 2);

int vsnprintf(char *buf, size_t limit, char const *format, va_list ap);

int snprintf(char *buf, size_t limit, char const *format, ...)
    ATTRIBUTE_FORMAT(3, 4);

void printk(char const *format, ...)
    ATTRIBUTE_FORMAT(1, 2);

void vprintk(char const *format, va_list ap);

void printdbg(char const *format, ...)
    ATTRIBUTE_FORMAT(1, 2);

void printdbg_lock(void);
void printdbg_unlock(void);

void vprintdbg(char const *format, va_list ap);

int cprintf(char const *format, ...)
    ATTRIBUTE_FORMAT(1, 2);

int vcprintf(char const *format, va_list ap);
