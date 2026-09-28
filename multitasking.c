#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
N = {100000000, 1000000000, 10000000000}
NUM_THREADS = {2, 4, 8}
NUM_TASKS = {2, 4, 8}
*/

#define LEN(array) (sizeof(array) / sizeof((array)[0])) // get size of array
#define CASE "multitasking"

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
    long N_values[3] = {100000000L, 1000000000L, 10000000000L};
    int NUM_TASKS[3] = {2, 4, 8};
    double task_res = 0.0;
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

        int num_tasks = NUM_TASKS[j];
        int p[2];
        if (pipe(p) == -1) {
            perror("pipe");
            fclose(log);
            return 1;
        }

        for (int i = 0; i < num_tasks; i++) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                fclose(log);
                return 1;
            }
            if (pid == 0) {
                long start_boundary = (N_values[j] * i) / num_tasks;
                long end_boundary = (N_values[j] * (i + 1)) / num_tasks;
                double child_res = WORKLOAD(start_boundary, end_boundary);
                close(p[0]);
                (void)write(p[1], &child_res, sizeof(child_res));
                close(p[1]);
                _exit(0);
            }
        }

        close(p[1]);
        res = 0.0;
        while (read(p[0], &task_res, sizeof(task_res)) == sizeof(task_res)) {
            res += task_res;
        }
        close(p[0]);
        while (wait(NULL) > 0) {
            /* Reap all child processes. */
        }

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
