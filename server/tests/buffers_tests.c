#include <buffers.h>
#include <vector.h>

#include <stdio.h>
#include <string.h>

int main(void)
{
    int32_t i = 123456;
    uint8_t b[10];
    memset(b, 0, 10);
    write_uint32(i, b, 2);

    return 0;
}