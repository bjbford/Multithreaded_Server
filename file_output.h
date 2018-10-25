#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

/**
 * Write the balance check request output to specfied output file.
 */
void write_balance_check(FILE *fp, int request_id, int balance, struct timeval r_time, struct timeval f_time);

/**
 * Write transaction request result output to specfied output file.
 */
void write_transaction(FILE *fp, int request_id, int acct_id, bool isf, struct timeval r_time, struct timeval f_time);