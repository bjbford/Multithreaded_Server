#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include "Bank.h"

// Request Node for linked list to hold all relevant information
typedef struct Request {
    int id;
    struct timeval time;
    struct Request *next;
    int num_args;
    char **args;
} Request;

// Linked list data structure to hold received request until processing
typedef struct Request_buffer {
    pthread_mutex_t lock;
    struct Request *head;
    struct Request *tail;
} Request_buffer;

/** 
 * Process a user's request.
 * Prints warning to user if not valid, exits program if is "END",
 * otherwise executes the given request.
 * Return:
 *      -1, if end command
 *       0, if unknown
 *       otherwise, 1
 */
int process_request(char **args, int num_args, int request_id, int num_accounts, FILE *fp);

/**
 * 
 */
void process_balance_check(char **args, int num_args, int request_id, int num_accounts, FILE *fp);

/**
 * 
 */
void process_transaction(char **args, int num_args);

/**
 * Get a request from head of request buffer and remove it for processing.
 * Return: request
 */
Request *get_request(Request_buffer *request_buf);

/**
 * Add a received request to the tail of the request buffer.
 */
void add_request(Request_buffer *request_buf, Request *request);

/**
 * Create and initialize a request from the user.
 */
Request* initialize_request(int id, char **args, int num_args);

/**
 * Create and initialize an empty request buffer.
 */
Request_buffer *create_request_buffer();