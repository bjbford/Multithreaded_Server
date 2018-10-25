#include "process.h"

/**
 * 
 */
void process_balance_check(Request *request, int num_accounts, FILE *fp) {
    int acct_id = request->args[0];
    // Check that account ID is in defined range
    if(acct_id > 0 && acct_id <= num_accounts) {
        // fetch account balance
        // pthread_mutex_lock();
        int balance = read_account(acct_id);
        // pthread_mutex_unlock();
        // print balance to output file
        // write_balance_check(fp, request->id, balance, request->time);
    }
    else {
        printf("%d is an invalid account.\n", acct_id);
    }
}

/**
 * 
 */
void process_transaction(Request *request, FILE *fp) {

    return;
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