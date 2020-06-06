#ifndef __CREATE_TABLE_H__
#define __CREATE_TABLE_H__

#include <sql.h>
#include <syscat.h>
#include <transaction.h>

#include <stdbool.h>

bool execute_create_table(struct transaction *tsx, struct syscat *cat, struct create_table *ct);

#endif
