#include <data_page.h>
#include <storage.h>
#include <transaction.h>

#define HEADER_OFFSET 0
#define SLOT_ARRAY_OFFSET (sizeof(struct data_page_header))

static 
void page_get_generic(struct data_page *page, SLOTID id, const char *column, void *value);

static void 
page_set_generic(struct data_page *page, SLOTID id, const char *column, void *value, size_t value_length);

static struct column * 
get_info(struct data_page *page, const char* name);

PNUM 
new_data_page(struct data_page *page, struct table_info *table, struct transaction *tsx)
{
    PNUM page_number           = transactional_append(tsx);
    page->header.page_type     = DATA_PAGE_TYPE;
    page->header.slot_count    = HEADER_OFFSET;
    page->header.records_end   = sizeof(struct data_page_header);
    page->header.records_begin = PAGE_SIZE;
    page->current_position     = page->header.records_end;
    page->table                = table;
    page->tsx                  = tsx;
    page->page_number          = page_number;

    transactional_update(tsx, page_number, &page->header, HEADER_OFFSET, sizeof(struct data_page_header));

    return page_number;
}

void 
open_data_page(PNUM page_number, struct data_page *page, struct table_info *table, struct transaction *tsx)
{
    transactional_pin(tsx, page_number);

    page->tsx         = tsx;
    page->table       = table;
    page->page_number = page_number;

    transactional_read(tsx, page_number, &page->header, HEADER_OFFSET, sizeof(struct data_page_header));
}

SLOTID 
data_page_insert(struct data_page *page)
{
    if ((page->header.records_begin - page->header.records_end) < page->table->record_size)
    {
        return INVALID_RECORD_HANDLE;
    }

    uint16_t record_start = page->header.records_begin;
    page->header.records_end += sizeof(uint16_t);
    SLOTID id = page->header.slot_count;
    transactional_update(
        page->tsx, page->page_number, 
        &record_start, 
        SLOT_ARRAY_OFFSET + (page->header.slot_count * sizeof(uint16_t)), 
        sizeof(uint16_t));
    
    page->header.slot_count++;
    page->current_position = page->header.records_begin;
    page->header.records_begin -= page->table->record_size;
    transactional_update(page->tsx, page->page_number, &page->header, HEADER_OFFSET, sizeof(struct data_page_header));

    return id;
}

void 
open_page_iterator(struct data_page *page, struct data_page_iterator *iterator)
{
    iterator->page = page;
    iterator->current_slot = 0;
}

SLOTID 
next_page_record(struct data_page_iterator *iterator)
{
    SLOTID id = iterator->current_slot;

    if (iterator->current_slot == iterator->page->header.slot_count)
    {
        return INVALID_RECORD_HANDLE;
    }

    transactional_read(
        iterator->page->tsx, 
        iterator->page->page_number, 
        &iterator->current_record, 
        SLOT_ARRAY_OFFSET + (sizeof(uint16_t) * iterator->current_slot), 
        sizeof(uint16_t));

    iterator->current_slot++;

    return id;
}

void 
page_get_int(struct data_page *page, SLOTID id, const char *column, int32_t *value)
{
    page_get_generic(page, id, column, value);
}

void 
page_get_char(struct data_page *page, SLOTID id, const char *column, char *value)
{
    page_get_generic(page, id, column, value);
}

void 
page_set_int(struct data_page *page, SLOTID id, const char *column, int value)
{
    page_set_generic(page, id, column, &value, sizeof(int));
}

void 
page_set_char(struct data_page *page, SLOTID id, const char *column, char *value)
{
    page_set_generic(page, id, column, value, strlen(value));
}


static 
void page_get_generic(struct data_page *page, SLOTID id, const char *column, void *value)
{
    struct column *cinfo = get_info(page, column);

    if (column == NULL)
    {
        return;
    }

    RECORD_HANDLE handle;

    transactional_read(
        page->tsx, 
        page->page_number, 
        &handle, 
        SLOT_ARRAY_OFFSET + (sizeof(uint16_t) * id), 
        sizeof(uint16_t));

    transactional_read(
        page->tsx, 
        page->page_number, 
        value, 
        handle - cinfo->offset - cinfo->size, 
        cinfo->size);
}

static void 
page_set_generic(struct data_page *page, SLOTID id, const char *column, void *value, size_t value_length)
{
    struct column *cinfo = get_info(page, column);

    if (cinfo == NULL)
    {
        return;
    }

    size_t write_length = cinfo->size > value_length ? value_length : cinfo->size;

    RECORD_HANDLE handle;

    transactional_read(
        page->tsx, 
        page->page_number, 
        &handle, 
        SLOT_ARRAY_OFFSET + (sizeof(uint16_t) * id), 
        sizeof(uint16_t));

    transactional_update(
        page->tsx, 
        page->page_number, 
        value, 
        handle - cinfo->size - cinfo->offset, 
        write_length);
}

static struct column * 
get_info(struct data_page *page, const char* name)
{
    for (int i = 0; i < page->table->column_count; i++)
    {
        if (strncmp(name, page->table->columns[i].name, MAX_COLUMN_NAME) == 0)
        {
            return &page->table->columns[i];
        }
    }

    return NULL;
}
