#include <stdio.h>
#include <stdlib.h>
unsigned long long sum_serial(int n) {
    unsigned long long sum = 0;
    for (int i = 1; i <= n; i++) {
        sum += i;
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    printf("Sum from 1 to %d is: %llu\n", n, sum_serial(n));

    return 0;
}
