#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "smart_str.h"

const int INIT_CAPACITY = 4;
const double CAP_INC_FACTOR = 1.5;

smart_str_t *emptyStr()
{
    smart_str_t *str_ptr = (smart_str_t *) malloc(sizeof(smart_str_t));
    str_ptr->length = 0;
    str_ptr->capacity = INIT_CAPACITY;
    str_ptr->contents = (char *) malloc((INIT_CAPACITY + 1) * sizeof(char));
    str_ptr->contents[INIT_CAPACITY] = '\0';

    return str_ptr;
}

smart_str_t *initStr(const char *contents)
{
    smart_str_t *str_ptr = (smart_str_t *) malloc(sizeof(smart_str_t));
    str_ptr->length = strlen(contents);
    str_ptr->capacity = (int) ceil(pow(CAP_INC_FACTOR, log((double) strlen(contents) / log(CAP_INC_FACTOR))));
    str_ptr->contents = (char *) malloc((str_ptr->capacity + 1) * sizeof(char));
    strcpy(str_ptr->contents, contents);

    return str_ptr;
}

void inc_capacity(smart_str_t *buf)
{
    buf->capacity += buf->capacity >> 1; // Equivalent to floor(capacity * 1.5)
    char *tmp = buf->contents;
    buf->contents = (char *) malloc((buf->capacity + 1) * sizeof(char));
    strcpy(buf->contents, tmp);
    free(tmp);
}

// Append a char to the contents buffer
void str_putc(smart_str_t *dst, char c)
{
    // Expand buffer when necessary
    if (dst->length + 1 > dst->capacity) {
        inc_capacity(dst);
    }

    // Insert the new char
    dst->length++;
    dst->contents[dst->length] = '\0';
    dst->contents[dst->length - 1] = c;
}

// Append a full string to the contents buffer
void str_puts(smart_str_t *dst, const char *str)
{
    while (dst->length + strlen(str) > dst->capacity) {
        inc_capacity(dst);
    }

    strcpy(dst->contents + strlen(dst->contents), str);
}

char *getContentsCpy(const smart_str_t *src)
{
    char *out = (char *) malloc((src->length + 1) * sizeof(char));
    strcpy(out, src->contents);
    return out;
}
