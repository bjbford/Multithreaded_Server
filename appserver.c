#include "process.h"

/**
 * Main program.
 */
int main(int argc, char **argv) {
    // check for proper number of args
    if(argc != 4) {
        printf("You entered the wrong number of arguments!\n");
        return 0;
    }
    if(!initialize_accounts(atoi(argv[2]))) {
        printf("Failed to initialize %s bank accounts\n", argv[2]);
    }
    int request_id = 0;
    // retrieve server parameters
    int num_workers = atoi(argv[1]);
    int num_accounts = atoi(argv[2]);
    // prepare output file
    char *filename = argv[3];
    FILE *output_file;
    output_file = fopen(filename, "w");

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
        int request_result = process_request(user_args, num_args, request_id, num_accounts, output_file);
        // If true, processing request
        if(request_result == 1) {
            request_id += 1;
            printf("< ID %d\n", request_id);
        }
        // If returns -1, exit program
        else if (request_result == -1){
            printf("Exiting 'appserver' program...\n");
            break;
        }
        else {
            printf("Please enter a valid request.\n");
        }
    }
    // close output file
    fclose(output_file);
    return 0;
}