#ifndef __EXPR_EVAL_H__
#define __EXPR_EVAL_H__

#include <scans.h>
#include <sql.h>
#include <value.h>

#include <stdbool.h>

struct value eval(struct expr *expr, struct scan *sscan);

#endif
