#ifndef __DATA_PAGE_H__
#define __DATA_PAGE_H__

#include <storage.h>

typedef uint16_t SLOTID;

struct record_id
{
    PNUM pid;
    SLOTID slot;
};

struct data_page_header
{
    uint16_t page_type;
    uint16_t records_begin;
    uint16_t records_end;
    uint16_t slot_count;
};

struct data_page
{
    struct data_page_header header;
    struct table_info *table;
    struct transaction* tsx;
    uint16_t current_position;

    PNUM page_number;
};

struct data_page_iterator
{
    struct data_page *page;
    uint16_t current_slot;
    uint16_t current_record;
    uint16_t total_slots;
};

PNUM new_data_page(struct data_page *page, struct table_info *table, struct transaction *tsx);
void open_data_page(PNUM page_number, struct data_page *page, struct table_info *table, struct transaction *tsx);
SLOTID data_page_insert(struct data_page *page);

void open_page_iterator(struct data_page *page, struct data_page_iterator *iterator);
SLOTID next_page_record(struct data_page_iterator *iterator);

void page_get_int(struct data_page *page, SLOTID handle, const char *column, int32_t *value);
void page_get_char(struct data_page *page, SLOTID handle, const char *column, char *value);
void page_set_int(struct data_page *page, SLOTID handle, const char *column, int value);
void page_set_char(struct data_page *page, SLOTID handle, const char *column, char *value);


#endif
