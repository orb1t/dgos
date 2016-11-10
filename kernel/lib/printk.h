
#include <stdarg.h>
#include "types.h"

void vpanic(char const *format, va_list ap);
void panic(char const *format, ...);

int vsnprintf(char *buf, size_t limit, char const *format, va_list ap);
void snprintf(char *buf, size_t limit, char const *format, ...);

void printk(char const *format, ...);
void vprintk(char const *format, va_list ap);

int cprintf(char const *format, ...);
int vcprintf(char const *format, va_list ap);
