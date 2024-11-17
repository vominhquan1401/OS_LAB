#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int start;
    int end;
    unsigned long long partial_sum;
} ThreadData;

void *calculate_partial_sum(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->partial_sum = 0;
    for (int i = data->start; i <= data->end; i++) {
        data->partial_sum += i;
    }
    return NULL;
}

unsigned long long sum_multi_thread(int n, int num_threads) {
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    int range = n / num_threads;
    unsigned long long total_sum = 0;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].start = i * range + 1;
        thread_data[i].end = (i == num_threads - 1) ? n : (i + 1) * range;
        pthread_create(&threads[i], NULL, calculate_partial_sum, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].partial_sum;
    }

    return total_sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_threads> <n>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int n = atoi(argv[2]);

    printf("Multi-threaded sum from 1 to %d with %d threads is: %llu\n", n, num_threads, sum_multi_thread(n, num_threads));

    return 0;
}
