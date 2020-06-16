#include <buffers.h>
#include <types.h>
#include <value.h>

#include <stdlib.h>

void reset(struct value *v)
{
    switch (v->type)
    {
    case TYPE_INT:
        v->as.number = 0;
        break;
    case TYPE_CHAR:
        v->as.string = NULL;
        break;
    default:
        break;
    }

    free(v->base);
    v->type = TYPE_UNKNOWN;
}

void deserialize(struct value *dest, enum npsql_type type, uint8_t *src, uint32_t size)
{
    reset(dest);
    dest->size = size;
    dest->type = type;
    dest->base = src;
    switch (dest->type)
    {
        case TYPE_INT:
            dest->as.number = read_uint32(src, 0);
            break;
        case TYPE_CHAR:
            dest->as.string = (char *)src;
            break;
        case TYPE_UNKNOWN:
            break;
    }
}
