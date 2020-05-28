#include <buffer.h>

static int byte_order() {
        short int word = 0x0001;
        char *b = (char *)&word;
        return (b[0] ? LITTLE_ENDIAN : BIG_ENDIAN);
}

void htop32(const int32_t i32, vector_type(uint8_t*) dest)
{
    uint8_t *p = (uint8_t *)&i32;
    vector_type(uint8_t) d = *dest;

    if (byte_order() == LITTLE_ENDIAN)
    {
        vector_push(d, p[0]);
        vector_push(d, p[1]);
        vector_push(d, p[2]);
        vector_push(d, p[3]);
    } 
    else
    {
        vector_push(d, p[4]);
        vector_push(d, p[3]);
        vector_push(d, p[2]);
        vector_push(d, p[0]);
    }

    *dest = d;
}
