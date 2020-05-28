#ifndef __COMMON_H__
#define __COMMON_H__

#include <defaults.h>

#include <stdint.h>
#include <stdlib.h>

#define UNUSED(expr) do { (void)(expr); } while (0)


#define TYPE_INT_SIZE 4


struct byte_buffer
{
    uint8_t *bytes;
    size_t length;
};


void uuid_string(uint8_t *uuid, char *string);
void htops(const int16_t value, uint8_t *buffer);
void htopi(const int32_t value, uint8_t *buffer);
int16_t ptohs(uint8_t *buffer);

#endif
