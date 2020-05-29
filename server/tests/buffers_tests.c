#include <buffers.h>
#include <vector.h>

int main(void)
{
    int32_t i = 123456;
    vbuffer b = NULL;

    write_uint32(i, &b);

    return 0;
}