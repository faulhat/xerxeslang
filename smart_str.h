#ifndef SMART_STR_H
#define SMART_STR_H

#include <stdint.h>

typedef struct {
    uint32_t length;
    uint32_t capacity;
    char *contents;
} smart_str_t;

smart_str_t *emptyStr(void);

smart_str_t *initStr(const char *);

void str_putc(smart_str_t *, char);

void str_puts(smart_str_t *, const char *);

char *getContentsCpy(const smart_str_t *);

#endif