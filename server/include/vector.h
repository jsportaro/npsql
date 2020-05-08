#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <common.h>

#include <assert.h> 
#include <stddef.h>
#include <stdlib.h>

// Taken from https://github.com/eteran/c-vector
// I just updated the formatting

#define VECTOR_TYPE(type) type *
     
#define VECTOR_SET_CAPACITY(vec, size)                                         \
    do {                                                                       \
        if (vec) {                                                             \
            ((size_t *)(vec))[-1] = (size);                                    \
        }                                                                      \
    } while (0)     
     
#define VECTOR_SET_SIZE(vec, size)                                             \
    do {                                                                       \
        if (vec) {                                                             \
            ((size_t *)(vec))[-2] = (size);                                    \
        }                                                                      \
    } while (0)     

#define VECTOR_INCREASE_SIZE(vec, size)                                        \
    do {                                                                       \
        if (vec) {                                                             \
            size_t __csize = ((size_t *)(vec))[-2];                            \
            ((size_t *)(vec))[-2] = __csize + (size);                          \
        }                                                                      \
    } while (0)     
     
#define VECTOR_CAPACITY(vec)                                                   \
    ((vec) ? ((size_t *)(vec))[-1] : (size_t)0)     
     
#define VECTOR_SIZE(vec)                                                       \
    ((vec) ? ((size_t *)(vec))[-2] : (size_t)0)     
     
#define VECTOR_EMPTY(vec)                                                      \
	(VECTOR_SIZE(vec) == 0)     
    
#define VECTOR_BEGIN(vec)                                                      \
	(vec)

#define VECTOR_END(vec)                                                        \
	((vec) ? &((vec)[VECTOR_SIZE(vec)]) : NULL)

#define VECTOR_GROW(vec, count)                                                \
    do {                                                                       \
        const size_t __new_size =                                              \
            (count) * sizeof(*(vec)) + (sizeof(size_t) * 2);                   \
        if (!(vec)) {                                                          \
            size_t *__new_vector = malloc(__new_size);                         \
            assert(__new_vector);                                              \
            (vec) = (void *)(&__new_vector[2]);                                \
            VECTOR_SET_CAPACITY((vec), (count));                               \
            VECTOR_SET_SIZE((vec), (0));                                       \
        } else {                                                               \
            size_t *__true_beginning = &((size_t *)(vec))[-2];                 \
            size_t *__new_vector = realloc(__true_beginning, __new_size);      \
            assert(__new_vector);                                              \
            (vec) = (void *)(&__new_vector[2]);                                \
            VECTOR_SET_CAPACITY((vec), (count));                               \
        }                                                                      \
    } while (0)     
     
#define VECTOR_FREE(vec)                                                       \
    do {                                                                       \
        if (vec) {                                                             \
            size_t *true_beginning = &((size_t *)(vec))[-2];                   \
            free(true_beginning);                                              \
        }                                                                      \
    } while (0)     
     
#define VECTOR_PUSH(vec, value)                                                \
	do {                                                                       \
		size_t __capacity = VECTOR_CAPACITY(vec);                              \
		if (__capacity <= VECTOR_SIZE(vec)) {                                  \
			VECTOR_GROW((vec), !__capacity ? __capacity + 1 : __capacity * 2); \
		}                                                                      \
		vec[VECTOR_SIZE(vec)] = (value);                                       \
		VECTOR_SET_SIZE((vec), VECTOR_SIZE(vec) + 1);                          \
	} while (0)

#endif