#ifndef __VALUE_H__
#define __VALUE_H__

#include <types.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct value
{
    enum npsql_type type;
    size_t size;
    uint8_t *base;
    union {
        int number;
        char *string;
        bool boolean;
    } as;
};

void reset(struct value *v);
void deserialize(struct value *dest, enum npsql_type type, uint8_t *src, uint32_t size);

#endif
