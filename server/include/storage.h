#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <vector.h>

#include <stdint.h>

#define PAGE_SIZE       0x200
#define MAX_OBJECT_NAME 15
#define MAX_TABLE_NAME  MAX_OBJECT_NAME
#define MAX_COLUMN_NAME 25
#define MAX_COLUMNS     16 
#define HEADER_OFFSET   0

#define INVALID_PNUM UINT64_MAX
#define INVALID_LSN UINT64_MAX
#define INVALID_TSX UINT32_MAX
#define INVALID_RECORD_HANDLE UINT16_MAX

typedef uint64_t PNUM;
typedef uint64_t LSN;
typedef uint32_t TSXID;
typedef uint16_t RECORD_HANDLE;

#define DATA_PAGE_TYPE 0x0FF0
#define AM_PAGE_TYPE    0x1FF1

enum column_type
{
    TYPE_INT,
    TYPE_CHAR
};

struct column
{
    enum column_type type;
    uint16_t size;
    char *name;
    uint16_t offset;
};

struct table_info
{
    char *table_name;
    uint16_t record_size;

    struct column columns[MAX_COLUMNS];
    uint16_t column_count;
};

// these might belong in the syscat 
void init_table_info(struct table_info *table);
void add_int(struct table_info *table, const char *name, const uint16_t name_length);
void add_char(struct table_info *table, const char *name, const uint16_t name_length, uint16_t size);
void free_table_info(struct table_info *table);

#endif
