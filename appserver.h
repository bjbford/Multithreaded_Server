#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Bank.h"

/** 
 * Process a user's request.
 * Prints warning to user if not valid, exits program if is "END",
 * otherwise executes the given request.
 * Return:
 *      -1, if end command
 *       0, if unknown
 *       otherwise, request ID
 */
int process_request(char **args, int num_args);