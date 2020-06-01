#include <buffer_manager.h>
#include <data_file.h>
#include <log_file.h>
#include <transaction.h>

void 
initialize_transaction_context(
    struct transaction_manager *transaction_manager, 
    const char *data_file_path, 
    const char *log_file_path)
{
    data_file_open(&transaction_manager->data_file, data_file_path);
    open_log_file(&transaction_manager->log_file, log_file_path);
    buffer_manager_initialize(&transaction_manager->buffer_manager, &transaction_manager->data_file, &transaction_manager->log_file);


    transaction_manager->count_lock = create_mutex();
}

struct transaction*  
begin_transaction(struct transaction_context *context);
{
    struct transaction *tsx = malloc(sizeof(struct transaction));

    assert(tsx != null);

    
}
