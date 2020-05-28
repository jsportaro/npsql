#ifndef __SCANS_H__
#define __SCANS_H__

#include <sql.h>
#include <vector.h>

#include <stdbool.h>

enum scan_type
{
    SCAN_PROJECT
};

struct scan_field
{
    bool error;
    enum expr_type type;
    union {
        int number;
        char *s;
    } value;
};

struct scan
{
    enum scan_type type;
    bool (*next)(struct scan *scan);
    vector_type(struct scan_field) scan_fields;
    struct scan *scan;
    bool has_rows;
};

struct scan_project
{
    enum scan_type type;
    bool (*next)(struct scan *scan);
    vector_type(struct scan_field) scan_fields;
    struct scan *scan;
    bool has_rows;

    vector_type(struct expr *) expr_list;
    bool has_next;
    vector_type(uint8_t) row_data;
};

struct scan * create_scan_project(vector_type(struct expr *) expr_list, struct scan *scan);
vector_type(struct column *) get_scan_columns(struct scan *scan);

#endif
