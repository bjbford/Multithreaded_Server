#include "file_output.h"

/**
 * Write the balance check request output to specfied output file.
 */
void write_balance_check(FILE *fp, int request_id, int balance, struct timeval r_time, struct timeval f_time) {
    flockfile(fp);
    fprintf(fp, "%d BAL %d TIME %ld.%06d %ld.%06d\n", request_id, balance, r_time.tv_sec, r_time.tv_usec, f_time.tv_sec, f_time.tv_usec);
    funlockfile(fp);
}

/**
 * Write transaction request result output to specfied output file.
 */
void write_transaction(FILE *fp, int request_id, int acct_id, bool isf, struct timeval r_time, struct timeval f_time) {
    flockfile(fp);
    if(isf) {
        fprintf(fp, "%d ISF %d TIME %ld.%06d %ld.%06d\n", request_id, acct_id, r_time.tv_sec, r_time.tv_usec, f_time.tv_sec, f_time.tv_usec);
    } else {
        fprintf(fp, "%d OK TIME %ld.%06d %ld.%06d\n", request_id, r_time.tv_sec, r_time.tv_usec, f_time.tv_sec, f_time.tv_usec);
    }
    funlockfile(fp);
}