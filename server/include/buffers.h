#ifndef __BUFFERS_H__
#define __BUFFERS_H__

#include <common.h>
#include <vector.h>

#include <endian.h>
#include <stdint.h>

typedef vector_type(uint8_t) vbuffer;

//  These functions are for building byte buffers as you go along.
//  They'll add/resize memory as needed. 
void push_uint16(vector_type(uint8_t) dest, const uint16_t number);
void push_uint32(vector_type(uint8_t) dest, const uint32_t number);
void push_uint64(vector_type(uint8_t) dest, const uint64_t number);
void push_cpy(vector_type(uint8_t) dest, void *src, size_t size);

//  These are for when the byte buffers are already sized. 
//  start should never go past the length of the buffer
void write_uint16(uint8_t *dest, uint32_t start, const uint16_t number);
void write_uint32(uint8_t *dest, uint32_t start, const uint32_t number);
void write_uint64(uint8_t *dest, uint32_t start, const uint64_t number);
uint16_t read_uint16(const uint8_t *src, uint32_t start);
uint32_t read_uint32(const uint8_t *src, uint32_t start);
uint64_t read_uint64(const uint8_t *src, uint32_t start);

#endif
