#include "appserver.h"

/**
 * Main program.
 */
int main(int argc, char **argv) {
    // check for proper number of args
    if(argc != 4) {
        printf("You entered the wrong number of arguments!\n");
        return 0;
    }
    initialize_accounts(atoi(argv[2]));

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
        // Check if the user just hit enter (Reprompt)
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
        int request_id = process_request(user_args, num_args);
        // If true, processing request
        if(request_id > 0) {
            printf("< ID %d\n", request_id);
        }
        // If returns -1, exit program
        else if (request_id == -1){
            printf("Exiting 'appserver' program...\n");
            break;
        }
        else {
            printf("Please enter a valid request.\n");
        }
    }
    return 0;
}

/** 
 * Process a user's request.
 * Prints warning to user if not valid, exits program if is "END",
 * otherwise executes the given request.
 * Return:
 *      -1, if end command
 *       0, if unknown
 *       otherwise, request ID
 */
int process_request(char **args, int num_args) {
    int request_id = 0;
    // Check for balance check
    if(strcmp(args[0], "CHECK") == 0) {
        //id = do_balance_check(args, num_args);
        request_id = 1;
    }
    // Check for transaction 
    else if(strcmp(args[0], "TRANS") == 0) {
        //id = do_transaction(args, num_args);
        request_id = 1;
    }
    // Check for end
    else if(strcmp(args[0], "END") == 0) {
        request_id = -1;
    }
    else {
        request_id = 0;
    }
    return request_id;
}