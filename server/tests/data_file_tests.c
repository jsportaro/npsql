#include <common.h>
#include <data_file.h>
#include <file.h>

#include <assert.h>
#include <stdlib.h>


uint8_t * 
allocate_page(void)
{
    uint8_t *page = calloc(PAGE_SIZE, sizeof(uint8_t));

    page[0] = 'h';
    page[1] = 'e';
    page[2] = 'l';
    page[3] = 'l';
    page[4] = 'o';
    
    return page;
}

void 
create_data_file(struct data_file *data_file, const char *path, const uint64_t pages)
{
    uint8_t *page = NULL;
    PNUM page_number = 0;
    
    file_delete(path);
    open_data_file(data_file, path);

    for (uint64_t i = 0; i < pages; i++)
    {
         uint8_t *page = allocate_page();

         page[5] = (char)i;
         append_page(data_file, &page_number, page);
         free(page);
    }

    close_data_file(data_file);
}

void
append_page_on_new_file()
{
    struct data_file test_data_file;
    const char *path = "append_page_on_new_file.dat";
    off_t actual_size;

    create_data_file(&test_data_file, path, 1);
    open_data_file(&test_data_file, path);
    actual_size = file_size(test_data_file.file);

    assert(actual_size == PAGE_SIZE);

    close_data_file(&test_data_file);
}

void
append_page_on_existing_file()
{
    struct data_file test_data_file;
    const char *path = "append_page_on_existing_file.dat";
    uint64_t actual_size;
    uint8_t *page = NULL;
    PNUM page_number = 0;

    file_delete(path);
    
    // Append page and close
    page = allocate_page();
    page_number = 0;

    open_data_file(&test_data_file, path);
    append_page(&test_data_file, &page_number, page);
    actual_size = file_size(test_data_file.file);
    close_data_file(&test_data_file);
    free(page);

    // Re-open file and append again
    page = allocate_page();
    page_number = 0;
    open_data_file(&test_data_file, path);
    append_page(&test_data_file, &page_number, page);
    actual_size = file_size(test_data_file.file);
    close_data_file(&test_data_file);
    free(page); 
    
    assert(page_number == 1);
    assert(actual_size == (PAGE_SIZE * 2));
}

void
read_page_to_buffer()
{
    const char *path = "read_page_to_buffer.dat";
    struct data_file test_data_file;
    uint8_t *page;

    create_data_file(&test_data_file, path, 4);
    
    open_data_file(&test_data_file, path);
    page = allocate_page();
    read_page(&test_data_file, 2, page);

    assert(page[0] == 'h');

    free(page);
    close_data_file(&test_data_file);
}

void
write_page_to_file()
{
    const char *path = "write_page_to_file.dat";
    struct data_file test_data_file;
    struct data_file actual_file;
    uint8_t *page;

    //  Create file, read page, write page, close.
    create_data_file(&test_data_file, path, 4);
    
    open_data_file(&test_data_file, path);
    page = allocate_page();
    read_page(&test_data_file, 2, page);
    page[0] = 'j';
    write_page(&test_data_file, 2, page);
    free(page);
    
    close_data_file(&test_data_file);

    //  Now, open file back up to see if changes persisted
    open_data_file(&actual_file, path);
    page = allocate_page();
    read_page(&actual_file, 2, page);

    assert(page[0] == 'j');  // Do we have jello?

    free(page);
    close_data_file(&actual_file);
}

void
should_return_next_page_number()
{
    struct data_file test_data_file;
    const char *path = "should_return_next_page_number.dat";
    uint64_t expected_page_number = 6;

    create_data_file(&test_data_file, path, expected_page_number);

    open_data_file(&test_data_file, path);
    uint64_t actual_page_number = page_count(&test_data_file);
    
    assert(expected_page_number == actual_page_number);

    close_data_file(&test_data_file);
}

int main(void)
{
    append_page_on_new_file();
    append_page_on_existing_file();
    read_page_to_buffer();
    write_page_to_file();
    should_return_next_page_number();
}
