#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdint.h>

#define PAGE_SIZE 0x200

#define INVALID_PNUM UINT64_MAX
#define INVALID_LSN UINT64_MAX
#define INVALID_TSX UINT32_MAX
#define INVALID_RECORD_HANDLE UINT16_MAX

typedef uint64_t PNUM;
typedef uint64_t LSN;
typedef uint32_t TSXID;
typedef uint16_t RECORD_HANDLE;

#endif
