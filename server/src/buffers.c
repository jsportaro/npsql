#include <buffers.h>
#include <common.h>
#include <endian.h>

union i16_u
{
    const uint8_t* c;
    uint32_t i;
};

union i32_u
{
    uint8_t c[4];
    uint32_t i;
};

union i64_u
{
    const uint8_t* c;
    uint32_t i;
};

void write_uint16(const uint16_t number, vector_type(uint8_t *) dest)
{
    union i16_u n;
    vector_type(uint8_t) d = *dest;

    n.i = htole16(number);

    vector_push(d, n.c[0]);
    vector_push(d, n.c[1]);

    *dest = d;
}

void write_uint32(const uint32_t number, vector_type(uint8_t *) dest)
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

void write_uint64(const uint64_t number, vector_type(uint8_t *) dest)
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

uint16_t read_uint16(const vector_type(uint8_t) src, uint32_t start)
{
    union i16_u n = {{ *n.c = src[start]}};

    return htole16(n.i);
}

uint32_t read_uint32(const vector_type(uint8_t) src, uint32_t start)
{
    union i32_u n = {{ *n.c = src[start]}};

    return htole32(n.i);
}

uint64_t read_uint64(const vector_type(uint8_t) src, uint32_t start)
{
    union i64_u n = {{ *n.c = src[start]}};

    return htole64(n.i);
}
