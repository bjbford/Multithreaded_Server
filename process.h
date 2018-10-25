#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

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

/**
 * 
 */
void process_balance_check(Request *request, int num_accounts, FILE *fp);

/**
 * 
 */
void process_transaction(Request *request, FILE *fp);

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