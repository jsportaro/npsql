#include <buffers.h>
#include <common.h>
#include <endian.h>

#include <string.h>

union i16_u
{
    uint8_t  c[2];
    uint16_t i;
};

union i32_u
{
    uint8_t c[4];
    uint32_t i;
};

union i64_u
{
    uint8_t  c[8];
    uint64_t i;
};

void push_uint16(const uint16_t number, vector_type(uint8_t *) dest)
{
    union i16_u n;
    vector_type(uint8_t) d = *dest;

    n.i = htole16(number);

    vector_push(d, n.c[0]);
    vector_push(d, n.c[1]);

    *dest = d;
}

void push_uint32(const uint32_t number, vector_type(uint8_t *) dest)
{
    union i32_u n;
    vector_type(uint8_t) d = *dest;

    n.i = htole32(number);

    vector_push(d, n.c[0]);
    vector_push(d, n.c[1]);
    vector_push(d, n.c[2]);
    vector_push(d, n.c[3]);

    *dest = d;
}

void push_uint64(const uint64_t number, vector_type(uint8_t *) dest)
{
    union i64_u n;
    vector_type(uint8_t) d = *dest;

    n.i = htole64(number);

    vector_push(d, n.c[0]);
    vector_push(d, n.c[1]);
    vector_push(d, n.c[2]);
    vector_push(d, n.c[3]);
    vector_push(d, n.c[4]);
    vector_push(d, n.c[5]);
    vector_push(d, n.c[6]);
    vector_push(d, n.c[7]);    

    *dest = d;
}

void write_uint16(const uint16_t number, uint8_t *dest, uint32_t start)
{
    union i16_u n;
    n.i = htole16(number);

    memcpy(dest + start, n.c, sizeof(uint16_t));
}

void write_uint32(const uint32_t number, uint8_t *dest, uint32_t start)
{
    union i32_u n;
    n.i = htole32(number);

    memcpy(dest + start, n.c, sizeof(uint32_t));
}

void write_uint64(const uint64_t number, uint8_t *dest, uint32_t start)
{
    union i64_u n;
    n.i = htole64(number);

    memcpy(dest + start, n.c, sizeof(uint64_t));
}

uint16_t read_uint16(const uint8_t *src, uint32_t start)
{
    union i16_u n;// = {{ *n.c = src[start]}};

    memcpy(n.c, &src[start], sizeof(uint16_t));

    return htole16(n.i);
}

uint32_t read_uint32(const uint8_t *src, uint32_t start)
{
    union i32_u n;// = {{ *n.c = src[start]}};

    memcpy(n.c, &src[start], sizeof(uint32_t));

    return htole32(n.i);
}

uint64_t read_uint64(const uint8_t *src, uint32_t start)
{
    union i64_u n;// = {{ *n.c = src[start]}};

    memcpy(n.c, &src[start], sizeof(uint64_t));

    return htole64(n.i);
}
