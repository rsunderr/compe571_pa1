#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

/*
N = {100000000, 1000000000, 10000000000}
NUM_THREADS = {2, 4, 8}
NUM_TASKS = {2, 4, 8}
*/

#define LEN(array) (sizeof(array) / sizeof((array)[0])) // get size of array
#define CASE "multithreading"

// Log outputs to terminal and add to log file
void log_printf(FILE *log, const char *format, ...) {
    va_list args;

    // Log to terminal
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    // Log to file
    va_start(args, format);
    vfprintf(log, format, args);
    va_end(args);

    fflush(log);
}

// Calculate the sum of variables from 0 up to N (not inclusive)
double WORKLOAD(long start, long N) {
    double sum = 0.0; // using doubles bc long long too small
    for (long i = start; i < N; i++) {
        sum += i;
    }
    return sum;
}

// Create child to call workload
void *WorkloadThread(void *argument) {
    long *args = argument;
    double *sum = malloc(sizeof(double)); // need to allocate space for child sums

    *sum = WORKLOAD(args[0], args[1]); // make workload call
    return sum;
}

int main(void) {
    // Variables
    long N_values[3] = {100000000, 1000000000, 10000000000};
    int NUM_THREADS[3] = {2, 4, 8};
    int max_threads = NUM_THREADS[LEN(NUM_THREADS)-1];
    double res = 0.0;
    long th_idx = 0;
    long first_th_idx = 0;
    void *result;
    int ret;
    int factor;
    double duration = 0.0;
    struct timespec start, end;

    // Make big array of threads based on sizes of related arrays
    pthread_t hThreads[LEN(N_values)*LEN(NUM_THREADS)*max_threads];
    long hThread_args[8][2];

    // Create log file
    FILE *log = fopen(CASE ".log", "w");
    if (log == NULL) {
        perror("fopen");
        return 1;
    }

    log_printf(log, "STARTING %s\n", CASE);

    // Main loop
    for (int j = 0; j < LEN(N_values); j++) {
        for (int k = 0; k < LEN(NUM_THREADS); k++) {
            // Call fxn
            clock_gettime(CLOCK_MONOTONIC, &start); // get start time
            // Setup
            res = 0.0;
            first_th_idx = th_idx;
            // Divvy N into equal slices and create threads
            factor = N_values[j] / NUM_THREADS[k];
            for (int l = 0; l < NUM_THREADS[k]; l++) {
                hThread_args[l][0] = l * factor; // begin
                hThread_args[l][1] = (l+1) * factor; // end

                // Create/join pthread child and add
                ret = pthread_create(&hThreads[th_idx], NULL, WorkloadThread, hThread_args[l]);
                if (ret != 0) { log_printf(log, "Thread creation failed\n"); }

                /*
                log_printf(log, "factor = %d\tth_idx = %d\tl = %d\tbegin = %d\tend=%d\n", factor, th_idx, l, 
                    hThread_args[l][0], hThread_args[l][1]);
                */
                
                th_idx++; // increment thread index at end
            }
            // Join threads, add results
            for (int l = 0; l < NUM_THREADS[k]; l++) {
                pthread_join(hThreads[first_th_idx + l], &result);
                // Add to sum and clear result pointer
                res += *(double *) result;
                free(result);
            }
            clock_gettime(CLOCK_MONOTONIC, &end); // get end time

            // Calculate time passed
            duration = (end.tv_sec - start.tv_sec) * 1e9;
            duration = (duration + (end.tv_nsec - start.tv_nsec)) * 1e-9;

            // Log results
            log_printf(log, "CASE: %s\tN = %ld\tNUM_THREADS = %d\tRES = %.0f\tTIME = %lf\n", CASE, 
                N_values[j], NUM_THREADS[k], res, duration);
        }
    }

    log_printf(log, "FINISHED %s\n", CASE);
    fclose(log);
    return 0;
}