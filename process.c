#include "process.h"

/** 
 * Process a user's request.
 * Prints warning to user if not valid, exits program if is "END",
 * otherwise executes the given request.
 * Return:
 *      -1, if end command
 *       0, if unknown
 *       otherwise, 1
 */
int process_request(char **args, int num_args, int request_id, int num_accounts, FILE *fp) {
    //Add request to command_buffer

    // Check for balance check
    if(strcmp(args[0], "CHECK") == 0) {
        process_balance_check(args, num_args, num_accounts, fp);
    }
    // Check for transaction 
    else if(strcmp(args[0], "TRANS") == 0) {
        process_transaction(args, num_args);
    }
    // Check for end
    else if(strcmp(args[0], "END") == 0) {

        return -1;
    }
    else {
        return 0;
    }
    return 1;
}

/**
 * 
 */
void process_balance_check(char **args, int num_args, int request_id, int num_accounts, FILE *fp) {
    int acct_id = args[1];
    // Check that account ID is in defined range
    if(acct_id > 0 && acct_id <= num_accounts) {
        // fetch account balance
        // pthread_mutex_lock();
        int balance = read_account(acct_id);
        // pthread_mutex_unlock();
        // print balance to output file
        write_balance_check(fp, request_id, balance);
    }
    else {
        printf("%d is an invalid account.\n", acct_id);
    }
}

/**
 * 
 */
void process_transaction(char **args, int num_args) {

    return;
}

/**
 * Get a request from head of request buffer and remove it for processing.
 * Return: request
 */
Request *get_request(Request_buffer *request_buf) {
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
void add_request(Request_buffer *request_buf, Request *request) {
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
 */
Request* initialize_request(int id, char **args, int num_args) {
    // Create and allocate memory for request
    Request *req = (Request *) malloc(sizeof(Request));
    // Allocate memory for arg int vector
    req->args = (int *) malloc(sizeof(int) * num_args);
    // Deep-copy arguments vector
    int i;
    for(i=0;i<num_args;i++) {
        // convert string args to signed integers
        req->args[i] = atoi(args[i]);
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
Request_buffer *create_request_buffer() {
    // Create and allocate memory for request buffer
    Request_buffer *req_buf = (Request_buffer *) malloc(sizeof(Request_buffer));
    // Set-up variables
    pthread_mutex_init(&(req_buf->lock), NULL);
    req_buf->head = req_buf->tail = NULL;
    return req_buf;
}