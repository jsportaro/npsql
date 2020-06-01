#ifndef __PLANS_H__
#define __PLANS_H__

#include <scans.h>
#include <sql.h>
#include <vector.h>

// Maybe the following two decs 
// go into storage.h?
enum column_type
{
    TYPE_INT,
    TYPE_CHAR
};

struct column
{
    vector_type(char) name;
    enum column_type type;
    uint16_t size;
};

enum plan_type
{
    PLAN_PROJECT
};

struct plan
{
    enum plan_type type;
    struct scan *(*open)(struct plan *plan);
    vector_type(struct column) column_list;
};

struct plan_project
{
    enum plan_type type;
    struct scan * (*open)(struct plan *project);
    vector_type(struct column) column_list;
    vector_type(struct expr *) expr_list;
};

struct plan * create_no_data_select_plan(struct select *select);
void free_plan(struct plan *plan);

#endif