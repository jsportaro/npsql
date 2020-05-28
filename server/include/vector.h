#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <assert.h> 
#include <stddef.h>
#include <stdlib.h>

// Taken from https://github.com/eteran/c-vector
// I just updated the formatting

#define vector_type(type) type *
     
#define vector_set_capacity(vec, size)                                         \
    do {                                                                       \
        if (vec) {                                                             \
            ((size_t *)(vec))[-1] = (size);                                    \
        }                                                                      \
    } while (0)     
     
#define vector_set_size(vec, size)                                             \
    do {                                                                       \
        if (vec) {                                                             \
            ((size_t *)(vec))[-2] = (size);                                    \
        }                                                                      \
    } while (0)     

#define vector_increase_size(vec, size)                                        \
    do {                                                                       \
        if (vec) {                                                             \
            size_t __csize = ((size_t *)(vec))[-2];                            \
            ((size_t *)(vec))[-2] = __csize + (size);                          \
        }                                                                      \
    } while (0)     
     
#define vector_capacity(vec)                                                   \
    ((vec) ? ((size_t *)(vec))[-1] : (size_t)0)     
     
#define vector_size(vec)                                                       \
    ((vec) ? ((size_t *)(vec))[-2] : (size_t)0)     
     
#define vector_empty(vec)                                                      \
	(vector_size(vec) == 0)     
    
#define vector_begin(vec)                                                      \
	(vec)

#define vector_end(vec)                                                        \
	((vec) ? &((vec)[vector_size(vec)]) : NULL)

#define vector_grow(vec, count)                                                \
    do {                                                                       \
        const size_t __new_size =                                              \
            (count) * sizeof(*(vec)) + (sizeof(size_t) * 2);                   \
        if (!(vec)) {                                                          \
            size_t *__new_vector = malloc(__new_size);                         \
            assert(__new_vector);                                              \
            (vec) = (void *)(&__new_vector[2]);                                \
            vector_set_capacity((vec), (count));                               \
            vector_set_size((vec), (0));                                       \
        } else {                                                               \
            size_t *__true_beginning = &((size_t *)(vec))[-2];                 \
            size_t *__new_vector = realloc(__true_beginning, __new_size);      \
            assert(__new_vector);                                              \
            (vec) = (void *)(&__new_vector[2]);                                \
            vector_set_capacity((vec), (count));                               \
        }                                                                      \
    } while (0)     
     
#define vector_free(vec)                                                       \
    do {                                                                       \
        if (vec) {                                                             \
            size_t *true_beginning = &((size_t *)(vec))[-2];                   \
            free(true_beginning);                                              \
        }                                                                      \
    } while (0)     
     
#define vector_push(vec, value)                                                \
	do {                                                                       \
		size_t __capacity = vector_capacity(vec);                              \
		if (__capacity <= vector_size(vec)) {                                  \
			vector_grow((vec), !__capacity ? __capacity + 1 : __capacity * 2); \
		}                                                                      \
		vec[vector_size(vec)] = (value);                                       \
		vector_set_size((vec), vector_size(vec) + 1);                          \
	} while (0)



#endif
