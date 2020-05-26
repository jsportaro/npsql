#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

enum scan_type
{
    SCAN_PROJECT
};

struct scan
{
    enum scan_type type;

    void (*next)(struct scan *scan);
};

struct scan_project
{
    enum scan_type type;
};

#endif
