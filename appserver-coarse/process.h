#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#include "Bank.h"
#include "file_output.h"

// Request Node for linked list to hold all relevant information
typedef struct Request {
    int id;
    struct timeval time;
    struct Request *next;
    char *type;
    int num_args;
    int *args;
} Request;

// Linked list data structure to hold received request until processing
typedef struct RequestBuffer {
    pthread_mutex_t lock;
    struct Request *head;
    struct Request *tail;
} RequestBuffer;

// Struct to pass arguments needed for thread routine
typedef struct ThreadArgs {
    RequestBuffer *req_buf;
    pthread_mutex_t accounts_lock;
    int num_accounts;
    char *output_file;
} ThreadArgs;

/**
 * Worker threads processing/fulfilling requests from the request buffer.
 */
void *process(void *thread_args);

/**
 * Process a balance check request, get time at end of request, and print to output file.
 */
void process_balance_check(Request *request, pthread_mutex_t accounts_lock, int num_accounts, char *output_file);

/**
 * Process a transaction request, get time at end of request, and print to output file.
 */
void process_transaction(Request *request, pthread_mutex_t accounts_lock, int num_accounts, char *output_file);

/**
 * Get a request from head of request buffer and remove it for processing.
 * Return: request
 */
Request *get_request(RequestBuffer *request_buf);

/**
 * Add a received request to the tail of the request buffer.
 */
void add_request(RequestBuffer *request_buf, Request *request);

/**
 * Create and initialize a request from the user.
 */
Request* initialize_request(int id, char **args, int num_args);

/**
 * Create and initialize an empty request buffer.
 */
RequestBuffer *create_request_buffer();

/**
 * Create and allocate memory for the structure which hold arguments to be passed to the thread start routine.
 */
ThreadArgs *create_thread_args_struct(RequestBuffer *req_buf, pthread_mutex_t accounts_lock, int num_accounts, char *output_file);