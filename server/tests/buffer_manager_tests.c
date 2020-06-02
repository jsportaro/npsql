#include <buffers.h>
#include <buffer_manager.h>
#include <file.h>
#include <data_file.h>
#include <log_file.h>

#include <assert.h>
#include <stdbool.h>

void should_find_previously_pinned_page()
{
    const char *data_path = "should_find_previously_pinned_page.dat";
    const char *log_path = "should_find_previously_pinned_page.log";
    struct data_file data_file;
    struct log_file log_file;
    struct buffer_manager buffer_manager;
    char *name = "Heather Portaro";
    struct buffer *new_buffer = NULL;
    struct buffer *test_buffer = NULL;
    file_delete(data_path);
    file_delete(log_path);
    open_data_file(&data_file, data_path);
    open_log_file(&log_file, log_path);
    buffer_manager_initialize(&buffer_manager, &data_file, &log_file);

    //  Create new buffer and set some data
    new_buffer = pin_new(&buffer_manager);
    write_buffer(new_buffer, name, 0, strlen(name), 1, 1);
    flush_buffers(&buffer_manager, 1);
    
    close_log_file(&log_file);
    close_data_file(&data_file);
    struct buffer_manager test_buffer_manager;
    open_data_file(&data_file, data_path);
    open_log_file(&log_file, log_path);
    buffer_manager_initialize(&test_buffer_manager, &data_file, &log_file);
    
    //  Now, get that buffer back and test to make sure things 
    //  were written correclty
    test_buffer = pin(&buffer_manager, 0);
    int cmp_result = strncmp(name, (char *)test_buffer->page, strlen(name));

    assert(cmp_result == 0);
}

void should_get_n_th_pinned_page()
{
    const char *data_path = "should_get_n_th_pinned_page.dat";
    const char *log_path = "should_get_n_th_pinned_page.log";
    struct data_file data_file;
    struct log_file log_file;
    struct buffer_manager buffer_manager;

    file_delete(data_path);
    file_delete(log_path);
    open_data_file(&data_file, data_path);
    open_log_file(&log_file, log_path);

    buffer_manager_initialize(&buffer_manager, &data_file, &log_file);

    char *name[] = { "Heather Portaro", "Joseph Portaro", "Jack Portaro", "Michael Portaro", "Jill Portaro" };

    for (int i = 0 ; i < 5; i++)
    {
        struct buffer *new_buffer = pin_new(&buffer_manager);
        write_buffer(new_buffer, name[i], 0,  strlen(name[i]), 1, 1);
    }

    flush_buffers(&buffer_manager, 1);

    struct buffer *pinned_buffer = pin(&buffer_manager, 2);

    int cmp_result = strncmp(name[2], (char *)pinned_buffer->page, strlen(name[2]));

    assert(cmp_result == 0);

    close_log_file(&log_file);
    close_data_file(&data_file);
}

void number_available_should_update()
{
    const char *data_path = "number_available_should_update.dat";
    const char *log_path = "number_available_should_update.log";
    struct data_file data_file;
    struct log_file log_file;
    struct buffer_manager buffer_manager;

    file_delete(data_path);
    file_delete(log_path);
    open_data_file(&data_file, data_path);
    open_log_file(&log_file, log_path);

    buffer_manager_initialize(&buffer_manager, &data_file, &log_file);

    char *name[] = { "Heather Portaro", "Joseph Portaro", "Jack Portaro", "Michael Portaro", "Jill Portaro" };
    struct buffer *buffers[5];
    for (int i = 0 ; i < 5; i++)
    {
        struct buffer *new_buffer = pin_new(&buffer_manager);
        write_buffer(new_buffer, name[i], 0, strlen(name[i]), 1, 1);
        buffers[i] = new_buffer;
        assert(buffer_manager.available_buffers == BUFFER_POOL_SIZE - (i + 1));
    }

    unpin(&buffer_manager, buffers[2]);
    assert(buffer_manager.available_buffers == BUFFER_POOL_SIZE - 4);
    unpin(&buffer_manager, buffers[4]);
    assert(buffer_manager.available_buffers == BUFFER_POOL_SIZE - 3);

    flush_buffers(&buffer_manager, 1);
}

void number_pins_should_update()
{
    const char *data_path = "number_pins_should_update.dat";
    const char *log_path = "number_pins_should_update.log";
    struct data_file data_file;
    struct log_file log_file;
    struct buffer_manager buffer_manager;

    file_delete(data_path);
    file_delete(log_path);
    open_data_file(&data_file, data_path);
    open_log_file(&log_file, log_path);

    buffer_manager_initialize(&buffer_manager, &data_file, &log_file);

    struct buffer *new_buffer = pin_new(&buffer_manager);
    struct buffer *again = pin(&buffer_manager, new_buffer->page_number);

    assert(new_buffer == again);
    assert(new_buffer->page == again->page);
    
    assert(again->pins == 2);
    unpin(&buffer_manager, again);
    assert(again->pins == 1);
}

void* transaction_thread(void* args)
{
	struct buffer_manager* buffer_manager = (struct buffer_manager*)args;

	struct buffer *b = pin_new(buffer_manager);

	unpin(buffer_manager, b);

    return NULL;
}

void buffer_manager_stress_test()
{
	const char* data_path = "buffer_manager_stress_test.dat";
	const char* log_path = "buffer_manager_stress_test.log";
	struct data_file data_file;
	struct log_file log_file;
	struct buffer_manager buffer_manager;

	file_delete(data_path);
	file_delete(log_path);
	open_data_file(&data_file, data_path);
	open_log_file(&log_file, log_path);

	buffer_manager_initialize(&buffer_manager, &data_file, &log_file);

	gpsql_thread threads[100];

	printf("Create threads\n");

	for (int i = 0; i < 100; i++)
	{
		threads[i] = create_thread(&transaction_thread, &buffer_manager);
	}

	printf("Clean up!\n");

	for (int i = 0; i < 100; i++)
	{
		join_thread(threads[i], 1000);
		release_thread(threads[i]);
	}

	printf("Stress test!\n");
}

int main(void)
{
    should_find_previously_pinned_page();
    should_get_n_th_pinned_page();
    number_available_should_update();
    number_pins_should_update();
	buffer_manager_stress_test();
}