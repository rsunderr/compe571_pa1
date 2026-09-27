#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdarg.h>

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

    double sum = WORKLOAD(args[0], args[1]);
    pthread_exit(0);
}

int main(void) {
    // Variables
    long N_values[3] = {32, 64, 80};
    int NUM_THREADS[3] = {2, 4, 8};
    double res = 0.0;
    long args[2] = {0, 0};
    int idx = 0;
    int ret;
    int factor;
    double duration = 0.0;
    struct timespec start, end;

    // Make big array of threads based on sizes of related arrays
    pthread_t hThreads[LEN(N_values)*LEN(NUM_THREADS)*NUM_THREADS[LEN(NUM_THREADS)-1]];

    // Create log file
    FILE *log = fopen(CASE ".log", "w");
    if (log == NULL) {
        perror("fopen");
        return 1;
    }

    log_printf(log, "STARTING %s\n", CASE);

    // pthread_t hThread;
    // args[0] = 0;
    // args[1] = 100;
    // ret = pthread_create(hThread, NULL, WorkloadThread, args);
    // if(ret < 0) {
    //     printf("Thread Creation Failed\n");
    //     return 1;
    // }
    // pthread_join(hThread, NULL);
    // printf("Parent is continuing....\n");

    // Main loop
    for (int j = 0; j < LEN(N_values); j++) {
        for (int k = 0; k < LEN(NUM_THREADS); k++) {
            // Call fxn
            clock_gettime(CLOCK_MONOTONIC, &start); // get start time
            // Divvy N into equal slices and create threads
            factor = N_values[j] / NUM_THREADS[k];
            for (int l = 1; l <= NUM_THREADS[k]; l++) {
                args[0] = (l - 1) * factor; // begin
                args[1] = l * factor - 1; // end
                //ret = pthread_create(hThreads[idx], NULL, WorkloadThread, args);
                // pthread_join(hThreads[idx], NULL);
                log_printf(log, "factor = %d\tidx = %d\tl = %d\tbegin = %d\tend=%d\n", factor, idx, l, args[0], args[1]);
                idx++;
            }
            clock_gettime(CLOCK_MONOTONIC, &end); // get end time

            // Calculate time passed
            duration = (end.tv_sec - start.tv_sec) * 1e9;
            duration = (duration + (end.tv_nsec - start.tv_nsec)) * 1e-9;

            // Log results
            log_printf(log, "CASE: %s\tN = %ld\tNUM_THREADS = %d\tTIME = %lf\n", CASE, 
                N_values[j], NUM_THREADS[k], duration);
        }
    }

    log_printf(log, "FINISHED %s\n", CASE);
    fclose(log);
    return 0;
}