#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

/*
N = {100000000, 1000000000, 10000000000}
NUM_THREADS = {2, 4, 8}
NUM_TASKS = {2, 4, 8}
*/

#define CASE "Baseline"

// Calculate the sum of variables from 0 up to N (not inclusive)
int WORKLOAD(long N) {
    int sum = 0;
    for (long i = 0; i < N; i++) {
        sum += i;
    }
    return sum;
}

int main(void) {
    long n_values[3] = {100000000, 1000000000, 10000000000};
    double duration = 0.0;
    struct timespec start, end;
    
    for (int j = 0; j < 3; j++) {
        // Call fxn
        clock_gettime(CLOCK_MONOTONIC, &start); // get start time
        WORKLOAD(n_values[j]);
        clock_gettime(CLOCK_MONOTONIC, &end); // get end time

        // Calculate time passed
        duration = (end.tv_sec - start.tv_sec) * 1e9;
        duration = (duration + (end.tv_nsec - start.tv_nsec)) * 1e-9;

        // Log results
        printf("CASE: %s\tVALUE = %ld\tTIME = %lf\n", CASE, n_values[j], duration);
    }

    return 0;
}