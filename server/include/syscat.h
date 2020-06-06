#ifndef __SYSCAT_H__
#define __SYSCAT_H__

#include <storage.h>
#include <transaction.h>

struct syscat 
{
    struct table_info object_catalog_schema;
    struct table_info table_catalog_schema;
    struct table_info column_catalog_schema;

    PNUM object_catalog_pid;
    PNUM table_catalog_pid;
    PNUM column_catalog_pid;
};

void create_system_catalogs(
    struct syscat *syscat, 
    struct transaction *tsx);

void intialize_system_catalogs(struct syscat *md_mgr);

void create_table(
    struct syscat *syscat,
    struct transaction *tsx, 
    struct table_info *schema);



#endif
