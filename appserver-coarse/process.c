#include "process.h"

/**
 * Worker threads processing/fulfilling requests from the request buffer.
 */
void * process(void *thread_args) {
    // Get arguments back out of thread args structure
    ThreadArgs *t_args = thread_args;
    RequestBuffer *req_buf = t_args->req_buf;
    pthread_mutex_t accounts_lock = t_args->accounts_lock;
    int num_accounts = t_args->num_accounts;
    char *output_file = t_args->output_file;

    // Infinite loop to process requests from buffer
    while(1) {
        // Check for empty buffer to avoid seg faulting
        if(req_buf->head == NULL) {
            usleep(10000);
            continue;
        }
        // Lock request buffer
        pthread_mutex_lock(&(req_buf->lock));
        // Get request from buffer
        Request *req = get_request(req_buf);
        // Unlock request buffer
        pthread_mutex_unlock(&(req_buf->lock));

        // Determine type of request, then process
        if(strcmp(req->type, "CHECK") == 0) {
            process_balance_check(req, accounts_lock, num_accounts, output_file);
        }
        else if(strcmp(req->type, "TRANS") == 0) {
            process_transaction(req, accounts_lock, num_accounts, output_file);
        }
        else if(strcmp(req->type, "END") == 0) {
            // Add request to buffer, so that all threads know to end
            pthread_mutex_lock(&(req_buf->lock));
            add_request(req_buf, req);
            pthread_mutex_unlock(&(req_buf->lock));
            // kill thread
            return NULL;
        }
        // Deallocate request and variables from heap
        free(req->args);
        free(req);
    }
    return NULL;
}

/**
 * Process a balance check request, get time at end of request, and print to output file.
 */
void process_balance_check(Request *request, pthread_mutex_t accounts_lock, int num_accounts, char *output_file) {
    // Initialize variable used to store request finish time
    struct timeval finish_time;
    int acct_id = request->args[0];
    // lock all accounts (coarse-grained locking)
    pthread_mutex_lock(&accounts_lock);
    // fetch account balance
    int balance = read_account(acct_id);
    // unlock all accounts (coarse-grained locking)
    pthread_mutex_unlock(&accounts_lock);
    // Request finished processing, so set finish time
    gettimeofday(&finish_time, NULL);
    // print balance to output file
    write_balance_check(output_file, request->id, balance, request->time, finish_time);
}

/**
 * Process a transaction request, get time at end of request, and print to output file.
 */
void process_transaction(Request *request, pthread_mutex_t accounts_lock, int num_accounts, char *output_file) {
    // Initialize variable used to store request finish time
    struct timeval finish_time;
    bool isf = false;
    int acct_id = 0;

    // Yes I know this is integer division, however we ensured this
    // was an positive even integer before adding to request buffer.
    int trans_num_accts = request->num_args/2;
    int trans_accts[trans_num_accts];
    int trans_amounts[trans_num_accts];

    // iterate over arguments: seperate accts and amounts
    int i;
    int acct_iter = 0;
    int amount_iter = 0;
    for(i=0;i<request->num_args;i++) {
        // even args = account ids
        if(i%2 == 0){
            trans_accts[acct_iter++] = request->args[i];
        }
        else {
            // odd args = amounts
            trans_amounts[amount_iter++] = request->args[i];
        }   
    }

    // Lock all acounts (coarse-grained locking)
    pthread_mutex_lock(&accounts_lock);

    // First check for insufficient funds, this isn't the most efficient approach,
    // however it preserves the account balances, in a ISF case.
    for(i=0;i<trans_num_accts;i++) {
        if((read_account(trans_accts[i]) + trans_amounts[i]) < 0) {
            isf = true;
            // set acct_id for account with ISF
            acct_id = trans_accts[i];
            break;
        }
    }

    // No ISF's, so perform actual transactions now
    if(!isf) {
        for(i=0;i<trans_num_accts;i++) {
            int new_bal = read_account(trans_accts[i]) + trans_amounts[i];
            write_account(trans_accts[i], new_bal);
        }
    }

    // Unlock all accounts (coarse-grained locking)
    pthread_mutex_unlock(&accounts_lock);

    // Request finished processing, so set finish time
    gettimeofday(&finish_time, NULL);
    // print balance to output file
    write_transaction(output_file, request->id, acct_id, isf, request->time, finish_time);
}

/**
 * Get a request from head of request buffer and remove it for processing.
 * Return: request
 */
Request *get_request(RequestBuffer *request_buf) {
    // Empty list, return NULL
    if (request_buf->head == NULL) {
        return NULL; 
    }
    // Get request from head of request buffer
    Request *request = request_buf->head; 
    // Remove from buffer
    request_buf->head = request_buf->head->next; 
    // If list becomes empty, update tail
    if (request_buf->head == NULL) {
       request_buf->tail = NULL; 
    }
    return request;
}

/**
 * Add a received request to the tail of the request buffer.
 */
void add_request(RequestBuffer *request_buf, Request *request) {
    // Empty list (request will be head and tail)
    if(request_buf->tail == NULL) {
        request_buf->head = request_buf->tail = request;
        return;
    }
    // Add request to end of request_buffer
    request_buf->tail->next = request; 
    request_buf->tail = request; 
}

/**
 * Create and initialize a request from the user.
 * Will be freed in thread upon execution.
 */
Request* initialize_request(int id, char **args, int num_args) {
    // decrement num_args since we don't want the first argument
    num_args -= 1;
    // Create and allocate memory for request
    Request *req = (Request *) malloc(sizeof(Request));
    // Type of request
    req->type = args[0];
    // Allocate memory for arg int vector
    req->args = (int *) malloc(sizeof(int) * num_args);
    // Deep-copy arguments vector (ignore first, that is our request type)
    int i;
    for(i=0;i<num_args;i++) {
        // convert string args to signed integers
        req->args[i] = atoi(args[i+1]);
    }
    // Setup variables
    req->next = NULL;
    req->id = id;
    req->num_args = num_args;
    gettimeofday(&(req->time), NULL);
    return req;
}

/**
 * Create and initialize an empty request buffer.
 */
RequestBuffer *create_request_buffer() {
    // Create and allocate memory for request buffer
    RequestBuffer *req_buf = (RequestBuffer *) malloc(sizeof(RequestBuffer));
    // Set-up variables
    pthread_mutex_init(&(req_buf->lock), NULL);
    req_buf->head = req_buf->tail = NULL;
    return req_buf;
}

/**
 * Create and allocate memory for the structure which hold arguments to be passed to the thread start routine.
 */
ThreadArgs *create_thread_args_struct(RequestBuffer *req_buf, pthread_mutex_t accounts_lock, int num_accounts, char *output_file) {
    ThreadArgs *t_args = (ThreadArgs *) malloc(sizeof(ThreadArgs));
    // Set-up variables
    t_args->req_buf = req_buf;
    t_args->accounts_lock = accounts_lock;
    t_args->num_accounts = num_accounts;
    t_args->output_file = output_file;
    return t_args;
}