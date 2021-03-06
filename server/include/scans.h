#ifndef __SCANS_H__
#define __SCANS_H__

#include <heap_table.h>
#include <query_context.h>
#include <sql.h>
#include <vector.h>

#include <stdbool.h>

enum scan_type
{
    PROJECT_SCAN,
    SELECT_SCAN,
    PRODUCT_SCAN,
    TABLE_SCAN
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
    void (*get_value)(struct scan *scan, struct identifier *column_name, struct value *v);
    void (*reset)(struct scan *scan);
};


struct project_scan
{
    enum scan_type type;
    bool (*next)(struct scan *scan);
    void (*get_value)(struct scan *scan, struct identifier *column_name, struct value *value);
    void (*reset)(struct scan *scan);
    
    struct scan *scan;
    bool has_next;
};

struct select_scan
{
    enum scan_type type;
    bool (*next)(struct scan *scan);
    void (*get_value)(struct scan *scan, struct identifier *column_name, struct value *value);
    void (*reset)(struct scan *scan);

    struct scan *scan;
    struct expr *where_clause;
};

struct product_scan
{
    enum scan_type type;
    bool (*next)(struct scan *scan);
    void (*get_value)(struct scan *scan, struct identifier *column_name, struct value *value);
    void (*reset)(struct scan *scan);

    struct scan *l;
    struct scan *r;
    bool primed;
};

struct table_scan
{
    enum scan_type type;
    bool (*next)(struct scan *scan);
    void (*get_value)(struct scan *scan, struct identifier *column_name, struct value *value);
    void (*reset)(struct scan *scan);

    struct table_info *ti;
    struct heap_table ht;
    struct heap_iterator i;
};

struct scan * new_scan_project(vector_type(struct expr_ctx *) expr_list, struct scan *scan);

struct scan * new_project_scan(struct scan *inner);
struct scan * new_select_scan(struct scan *inner, struct expr *where_clause);
struct scan * new_product_scan(struct scan *l, struct scan *r);
struct scan * new_table_scan(struct table_info *ti, PNUM first_am, struct query_ctx *ctx);

void free_scan(struct scan *scan);

#endif
