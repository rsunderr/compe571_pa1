#include <stdio.h>
#include <time.h>
#include <stdarg.h>

/*
N = {100000000, 1000000000, 10000000000}
NUM_THREADS = {2, 4, 8}
NUM_TASKS = {2, 4, 8}
*/

#define LEN(array) (sizeof(array) / sizeof((array)[0])) // get size of array
#define CASE "baseline"

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

int main(void) {
    // Variables
    long N_values[3] = {100000000, 1000000000, 10000000000};
    double res = 0.0;
    double duration = 0.0;
    struct timespec start, end;

    // Create log file
    FILE *log = fopen(CASE ".log", "w");
    if (log == NULL) {
        perror("fopen");
        return 1;
    }
    
    // Begin main loop
    log_printf(log, "STARTING %s\n", CASE);
    for (int j = 0; j < LEN(N_values); j++) {
        // Call fxn
        clock_gettime(CLOCK_MONOTONIC, &start); // get start time
        res = WORKLOAD(0, N_values[j]);
        clock_gettime(CLOCK_MONOTONIC, &end); // get end time

        // Calculate time passed
        duration = (end.tv_sec - start.tv_sec) * 1e9;
        duration = (duration + (end.tv_nsec - start.tv_nsec)) * 1e-9;

        // Log results
        log_printf(log, "CASE: %s\tN = %ld\tRES = %.0f\tTIME = %lf\n", CASE, N_values[j], res, duration);
    }

    log_printf(log, "FINISHED %s\n", CASE);
    fclose(log);
    return 0;
}