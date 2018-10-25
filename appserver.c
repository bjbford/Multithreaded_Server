#include "process.h"

/**
 * Main program.
 */
int main(int argc, char **argv) {
    // check for proper number of args
    if(argc != 4) {
        printf("You entered the wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }
    if(!initialize_accounts(atoi(argv[2]))) {
        printf("Failed to initialize %s bank accounts\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    int request_id = 0;
    // retrieve server parameters
    int num_workers = atoi(argv[1]);
    int num_accounts = atoi(argv[2]);
    // prepare output file
    char *filename = argv[3];
    FILE *output_file;
    output_file = fopen(filename, "w");

    // Initialize request buffer
    RequestBuffer *request_buff = create_request_buffer();
    // Initialize all account locks (mutexes)
    pthread_mutex_t account_locks[num_accounts];
    int lock_iter;
    for(lock_iter=0;lock_iter<num_accounts;lock_iter++) {
        pthread_mutex_init(&(account_locks[lock_iter]), NULL);
    }

    // Initialize user-defined amount of worker threads
    int i;
    for(i=0;i<num_workers;i++) {
        // pthread_create();
    }

    // Infinite loop until user requests to END (Requirement 2.3)
    while(1) {
        char *input = NULL;
        size_t size = 0;
        ssize_t chars;
        int num_args = 0;
        int max_args = 21;
        char *user_args[max_args];
        // Print user prompt, denoting input
        printf("> ");
        // read from user command line
        if((chars = getline(&input, &size, stdin)) == -1) {
            perror("Could not read from user command line");
            exit(EXIT_FAILURE);
        }
        // Check if the user just hit enter (Reprompt edge case)
        if(strcmp(input, "\n") == 0 && strlen(input) == 1) {
            continue;
        }
        // Change '\n' at end of input to null terminator
        input[strlen(input) - 1] = '\0';
        // Get first argument delimited by space
        char *argument = strtok(input, " ");	
        // Create array of user arguments from input string delimited by space
        while(argument != NULL) {
            user_args[num_args++] = argument;
            argument = strtok(NULL, " ");
        }
        // Catch max arg warning and notify user. (10 transaction limit)
        if(num_args > max_args) {
            printf("Warning: You reached the maximum allowable number of transactions: 10\n");
        }
        
        // Create a new request
        request_id++;
        Request *new_req = initialize_request(request_id, user_args, num_args);
        // Debugging to file
        // fprintf(output_file, "RequestID: %d\nNum_args:%d\nType: %s\n", new_req->id, new_req->num_args, new_req->type);
        // int check;
        // for(check=0;check<new_req->num_args;check++) {
        //     fprintf(output_file, "new_req->arg[%d]: %d\n", check, new_req->args[check]);
        // }
        // fprintf(output_file, "-------------------\n");

        // Determine the user's request:
        // Check for valid balance check
        if((strcmp(user_args[0], "CHECK") == 0) && (num_args == 2)) {
            printf("< ID %d\n", request_id);
        }
        // Check for valid transaction 
        else if((strcmp(user_args[0], "TRANS") == 0) && (num_args >= 3) && (num_args%2 != 0)) {
            printf("< ID %d\n", request_id);
        }
        // Check for end
        else if(strcmp(user_args[0], "END") == 0) {
            printf("< Exiting 'appserver' program...\n");
            break;
        }
        // Invalid request
        else {
            printf("< Please enter a valid request.\n");
            // Decrement ID
            request_id--;
            // Throw away request
            free(new_req->args);
            free(new_req);
        }

        // Add request to request buffer, lock mutex first
        pthread_mutex_lock(&(request_buff->lock));
        add_request(request_buff, new_req);
        pthread_mutex_unlock(&(request_buff->lock));
    }

    // Wait for threads to complete
    for(i=0;i<num_workers;i++) {
        //pthread_join();
    }

    // close output file
    fclose(output_file);
    free(request_buff);
    exit(EXIT_SUCCESS);
    return 0;
}