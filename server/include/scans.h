#ifndef __SCANS_H__
#define __SCANS_H__

#include <sql.h>
#include <vector.h>

#include <stdbool.h>

enum column_types
{
    TYPE_INT,
    TYPE_CHAR
};

struct column
{
    vector_type(uint8_t) name;
    enum column_types type;
    uint16_t size;
};


enum scan_type
{
    SCAN_PROJECT
};

struct scan
{
    enum scan_type type;
    bool (*next)(struct scan *scan);
    struct scan *scan;
    bool has_rows;
};

struct scan_project
{
    enum scan_type type;
    bool (*next)(struct scan *scan);
    struct scan *scan;
    bool has_rows;

    vector_type(struct expr *) expr_list;
    bool hasNext;
};

struct scan * create_scan_project(vector_type(struct expr *) expr_list, struct scan *scan);
vector_type(struct column *) get_scan_columns(struct scan *scan);

#endif
