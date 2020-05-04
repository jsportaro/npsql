#include <common.h>
#include <stdio.h>

int byte_order() {
        short int word = 0x0001;
        char *b = (char *)&word;
        return (b[0] ? LITTLE_ENDIAN : BIG_ENDIAN);
}

void uuid_string(uint8_t *uuid, char *string)
{
    sprintf(string, 
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
        uuid[3], uuid[2], uuid[1], uuid[0], uuid[5], uuid[4], uuid[7], uuid[6],
        uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
}

void htops(const int16_t value, uint8_t *buffer)
{
    uint8_t *p = (uint8_t *)&value;

    if (byte_order() == LITTLE_ENDIAN)
    {
        buffer[0] = p[0];
        buffer[1] = p[1];
    } 
    else
    {
        buffer[0] = p[1];
        buffer[1] = p[0];
    }
}

int16_t ptohs(uint8_t *buffer)
{
    int16_t s = 0;
    uint8_t *p = (uint8_t *)&s;

    if (byte_order() == LITTLE_ENDIAN) 
    {
        p[0] = buffer[0];
        p[1] = buffer[1];
    } else {
        p[0] = buffer[1];
        p[1] = buffer[0];
    }

    return s;
}