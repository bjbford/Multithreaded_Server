#include "process.h"

/**
 * Check user command for a valid transaction, under the following conditions (Requirement 2.2):
 *      - Number of arguments are odd
 *      - Greater than or equal to 3 arguments total
 *      - All account ids are > 0 and <= num_accounts
 */
bool valid_transaction_check(char **user_args, int num_args, int num_accounts);

/**
 * Check user command for a valid balance check, under the following conditions (Requirement 2.1):
 *      - Exactly two arguments
 *      - Account id is > 0 and <= num_accounts
 */
bool valid_balance_check(char **user_args, int num_args, int num_accounts);

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
    char *output_file = argv[3];
    // open file to either clear or create it, then close
    FILE *fp = fopen(output_file, "w");
    fclose(fp);
    // Variable to check for end of program
    bool end_program = false;

    // Initialize request buffer
    RequestBuffer *request_buff = create_request_buffer();
    // Initialize all accounts with one mutex (coarse-grained locking)
    pthread_mutex_t accounts_lock;
    pthread_mutex_init(&accounts_lock, NULL);
    
    // Initialize thread routine argument struct
    ThreadArgs *thread_args = create_thread_args_struct(request_buff, accounts_lock, num_accounts, output_file);
    // Initialize user-defined amount of worker threads
    pthread_t worker_threads[num_workers];
    int i;
    for(i=0;i<num_workers;i++) {
        pthread_create(&(worker_threads[i]), NULL, process, (void*)thread_args);
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

        // Determine the user's request:
        // Check for valid balance check
        if((strcmp(user_args[0], "CHECK") == 0) && valid_balance_check(user_args, num_args, num_accounts)) {
            printf("< ID %d\n", request_id);
        }
        // Check for valid transaction 
        else if((strcmp(user_args[0], "TRANS") == 0) && valid_transaction_check(user_args, num_args, num_accounts)) {
            printf("< ID %d\n", request_id);
        }
        // Check for end
        else if((strcmp(user_args[0], "END") == 0) && (num_args == 1)) {
            end_program = true;
            printf("< Exiting 'appserver' program...\n");
        }
        // Invalid request
        else {
            printf("< Please enter a valid request.\n");
            // Decrement ID
            request_id--;
            // Throw away request
            free(new_req->args);
            free(new_req);
            // Reprompt the user
            continue;
        }

        // Add request to request buffer, lock mutex first
        pthread_mutex_lock(&(request_buff->lock));
        add_request(request_buff, new_req);
        pthread_mutex_unlock(&(request_buff->lock));

        // Break loop when end_program request is seen
        if(end_program) {
            break;
        }
    }

    // Wait for threads to complete
    for(i=0;i<num_workers;i++) {
        pthread_join(worker_threads[i], NULL);
    }

    // Free allocated memory for request buffer and thread argument structure.
    free(request_buff);
    free(thread_args);
    exit(EXIT_SUCCESS);
    return 0;
}


/**
 * Check user command for a valid transaction, under the following conditions (Requirement 2.2):
 *      - Number of arguments are odd
 *      - Greater than or equal to 3 arguments total
 *      - All account ids are > 0 and <= num_accounts
 */
bool valid_transaction_check(char **user_args, int num_args, int num_accounts) {
    // First check for odd number of args
    if (num_args % 2 == 0) {
        return false;
    }
    // Check for min number of arguments
    if (num_args < 3) {
        return false;
    }
    // Finally check bounds of all account numbers
    int i;
    int acct_id;
    for(i=1;i<num_args;i+=2) {
        acct_id = atoi(user_args[i]);
        if((acct_id <= 0) || (acct_id > num_accounts)){
            return false;
        }
    }
    // Made it here, so valid transaction
    return true;
}

/**
 * Check user command for a valid balance check, under the following conditions (Requirement 2.1):
 *      - Exactly two arguments
 *      - Account id is > 0 and <= num_accounts
 */
bool valid_balance_check(char **user_args, int num_args, int num_accounts) {
    if((num_args != 2)) {
        return false;
    }
    // perform checks
    bool acct_id_positive = (atoi(user_args[1]) > 0);
    bool acct_in_bound = (atoi(user_args[1]) <= num_accounts);
    // All checks must evaluate be true to be valid
    if(acct_id_positive && acct_in_bound) {
        return true;
    }
    else {
        return false;
    }
}