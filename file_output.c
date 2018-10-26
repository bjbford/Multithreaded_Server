#include "file_output.h"

/**
 * Write the balance check request output to specfied output file.
 */
void write_balance_check(char *output_file, int request_id, int balance, struct timeval r_time, struct timeval f_time) {
    FILE *fp = fopen(output_file, "a");
    flockfile(fp);
    fprintf(fp, "%d BAL %d TIME %ld.%06d %ld.%06d\n", request_id, balance, r_time.tv_sec, r_time.tv_usec, f_time.tv_sec, f_time.tv_usec);
    funlockfile(fp);
    fclose(fp);
}

/**
 * Write transaction request result output to specfied output file.
 */
void write_transaction(char *output_file, int request_id, int acct_id, bool isf, struct timeval r_time, struct timeval f_time) {
    FILE *fp = fopen(output_file, "a");
    flockfile(fp);
    flockfile(fp);
    if(isf) {
        fprintf(fp, "%d ISF %d TIME %ld.%06d %ld.%06d\n", request_id, acct_id, r_time.tv_sec, r_time.tv_usec, f_time.tv_sec, f_time.tv_usec);
    } else {
        fprintf(fp, "%d OK TIME %ld.%06d %ld.%06d\n", request_id, r_time.tv_sec, r_time.tv_usec, f_time.tv_sec, f_time.tv_usec);
    }
    funlockfile(fp);
    fclose(fp);
}