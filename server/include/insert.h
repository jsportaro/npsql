#ifndef __INSERT_H__
#define __INSERT_H__

#include <syscat.h>
#include <sql.h>
#include <transaction.h>

#include <stdbool.h>

bool execute_insert_into(struct transaction *tsx, struct syscat *cat, struct insert *i);

#endif