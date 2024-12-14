#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <sys/wait.h>
/* Struct for storing configuration arguments */
struct _appconf {
    int arrsz;
    int tnum;
    int seednum;
};

/* Struct for storing range information */
struct _range {
    int start;
    int end;
};

/* Global variables */
long sumbuf = 0;
int* shrdarrbuf;
pthread_mutex_t mtx;

/** process command line argument.
 *  values are made available through the 'conf' struct.
 *  using the parsed conf to get arguments: the arrsz, tnum, and seednum
 */
extern int processopts (int argc, char **argv, struct _appconf *conf); 

/** process string to number.
 *  string is stored in 'nptr' char array.
 *  'num' is returned the valid integer number.
 *  return 0 valid number stored in num
 *        -1 invalid and num is useless value.
 */
extern int tonum (const char * nptr, int * num);

/** validate the array size argument.
 *  the size must be splitable "num_thread".
 */
extern int validate_and_split_argarray (int arraysize, int num_thread, struct _range* thread_idx_range);

/** generate "arraysize" data for the array "buf"
 *  validate the array size argument.
 *  the size must be splitable "num_thread".
 */
extern int generate_array_data (int* buf, int arraysize, int seednum);

/** display help */
extern void help (int xcode);
void* sum_worker(void *arg);
long validate_sum(int arraysize);

/* Implementation of the sum worker function */
void* sum_worker(void *arg) {
    struct _range *idx_range = (struct _range *)arg;
    int i;
    long local_sum = 0;

    printf("In worker from %d to %d\n", idx_range->start, idx_range->end);

    /* Calculate local sum */
    for (i = idx_range->start; i <= idx_range->end; i++) {
        local_sum += shrdarrbuf[i];
    }

    /* Update global sum buffer with mutex protection */
    pthread_mutex_lock(&mtx);
    sumbuf += local_sum;
    pthread_mutex_unlock(&mtx);

    return NULL;
}

/* Main program */
int main(int argc, char *argv[]) {
    struct _appconf appconf;
    struct _range *thread_idx_range;
    pthread_t *tid;
    int pid;

    if (argc < 3 || argc > 4) /* only accept 2 or 3 arguments */
        help(EXIT_SUCCESS);

    processopts(argc, argv, &appconf); /* Process all options and arguments */

    printf("Program runs with arrsz=%d, tnum=%d, seednum=%d\n",
           appconf.arrsz, appconf.tnum, appconf.seednum);

    /* Allocate memory for thread ranges */
    thread_idx_range = malloc(appconf.tnum * sizeof(struct _range));
    if (thread_idx_range == NULL) {
        fprintf(stderr, "Error! Memory for index storage not allocated.\n");
        exit(EXIT_FAILURE);
    }

    /* Validate and split array index ranges */
    if (validate_and_split_argarray(appconf.arrsz, appconf.tnum, thread_idx_range) < 0) {
        fprintf(stderr, "Error! Array index not splitable.\n");
        exit(EXIT_FAILURE);
    }

    /* Allocate shared array buffer */
    shrdarrbuf = malloc(appconf.arrsz * sizeof(int));
    if (shrdarrbuf == NULL) {
        fprintf(stderr, "Error! Memory for array buffer not allocated.\n");
        exit(EXIT_FAILURE);
    }

    /* Generate array data */
    if (generate_array_data(shrdarrbuf, appconf.arrsz, appconf.seednum) < 0) {
        fprintf(stderr, "Error! Failed to generate array data.\n");
        exit(EXIT_FAILURE);
    }

    /* Create a child process to validate sum */
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error! Fork failed.\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { /* Child process */
        printf("Sequence sum results: %ld\n", validate_sum(appconf.arrsz));
        exit(EXIT_SUCCESS);
    }

    /* Parent process */
    tid = malloc(appconf.tnum * sizeof(pthread_t));
    if (tid == NULL) {
        fprintf(stderr, "Error! Memory for thread IDs not allocated.\n");
        exit(EXIT_FAILURE);
    }

    /* Initialize mutex */
    pthread_mutex_init(&mtx, NULL);

    /* Create threads for parallel sum computation */
    for (int i = 0; i < appconf.tnum; i++) {
        pthread_create(&tid[i], NULL, sum_worker, &thread_idx_range[i]);
    }

    /* Wait for all threads to finish */
    for (int i = 0; i < appconf.tnum; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("Parallel sum result: %ld\n", sumbuf);

    /* Cleanup */
    free(thread_idx_range);
    free(shrdarrbuf);
    free(tid);
    pthread_mutex_destroy(&mtx);

    waitpid(pid,NULL,0);

    return 0;
}

/* Convert string to number */
int tonum(const char *nptr, int *num) {
    char *endptr;
    long val = strtol(nptr, &endptr, 10);

    if (*endptr != '\0' || endptr == nptr || val > INT_MAX || val < INT_MIN) {
        return -1; /* Invalid number */
    }

    *num = (int)val;
    return 0;
}

/* Process command line arguments */
int processopts(int argc, char **argv, struct _appconf *conf) {
    if (tonum(argv[1], &conf->arrsz) < 0 || conf->arrsz <= 0) {
        fprintf(stderr, "Invalid array size: %s\n", argv[1]);
        return -1;
    }

    if (tonum(argv[2], &conf->tnum) < 0 || conf->tnum <= 0) {
        fprintf(stderr, "Invalid thread count: %s\n", argv[2]);
        return -1;
    }

    if (argc == 4) {
        if (tonum(argv[3], &conf->seednum) < 0) {
            fprintf(stderr, "Invalid seed value: %s\n", argv[3]);
            return -1;
        }
    } else {
        conf->seednum = time(NULL); /* Default seed is current time */
    }

    return 0;
}

/* Validate and split array size */
int validate_and_split_argarray(int arraysize, int num_thread, struct _range *thread_idx_range) {
    if (arraysize % num_thread != 0) {
        return -1; /* Not divisible */
    }

    int chunk_size = arraysize / num_thread;
    for (int i = 0; i < num_thread; i++) {
        thread_idx_range[i].start = i * chunk_size;
        thread_idx_range[i].end = (i + 1) * chunk_size - 1;
    }

    return 0;
}

/* Generate array data */
int generate_array_data(int *buf, int arraysize, int seednum) {
    if (buf == NULL || arraysize <= 0) {
        return -1; /* Invalid input */
    }

    srand(seednum);
    for (int i = 0; i < arraysize; i++) {
        buf[i] = rand() % 100; /* Random value in range [0, 99] */
    }

    return 0;
}

/* Display help message */
void help(int xcode) {
    printf("Usage: aggsum <array_size> <num_threads> [seed]\n");
    printf("\nArguments:\n");
    printf("  <array_size>    Size of the array to generate and process.\n");
    printf("  <num_threads>   Number of threads to use for parallel computation.\n");
    printf("  [seed]          Optional seed for random number generation.\n");
    printf("\nExample:\n");
    printf("  ./aggsum 1000 4 42\n");
    printf("  This will generate an array of size 1000, use 4 threads, and seed the random number generator with 42.\n");
    exit(xcode);
}

/* Validate sum in sequence */
long validate_sum(int arraysize) {
    long validsum = 0;
    for (int i = 0; i < arraysize; i++) {
        validsum += shrdarrbuf[i];
    }
    return validsum;
}
