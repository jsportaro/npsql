#ifndef __BUFFERS_H__
#define __BUFFERS_H__

#include <common.h>
#include <vector.h>

#include <endian.h>
#include <stdint.h>

typedef vector_type(uint8_t) vbuffer;

void write_uint16(const uint16_t number, vector_type(uint8_t *) dest);
void write_uint32(const uint32_t number, vector_type(uint8_t *) dest);
void write_uint64(const uint64_t number, vector_type(uint8_t *) dest);
uint16_t read_uint16(const vector_type(uint8_t) src, uint32_t start);
uint32_t read_uint32(const vector_type(uint8_t) src, uint32_t start);
uint64_t read_uint64(const vector_type(uint8_t) src, uint32_t start);

#endif
