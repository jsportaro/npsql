#include <buffer_manager.h>

int leak_check(void)
{
    struct buffer_manager bf;

    buffer_manager_initialize(&bf);

    free_buffer_manager(&bf);

    return 1;
}

int main(void)
{
    leak_check();

    return 1;
}