#include <plans.h>

#include <common.h>

struct scan * open_no_data_select_scan(struct plan *plan)
{
    struct project *p = (struct project *)plan;
    
    return create_scan_project(p->expr_list, NULL);
}

struct plan * create_no_data_select_plan(struct select *select)
{
    struct project *p = malloc(sizeof(struct project));

    p->type = PLAN_PROJECT;
    p->expr_list = select->expr_list;
    p->open = &open_no_data_select_scan;

    return (struct plan *)p;
}
