#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>


#define FILE1 "/home/quan/code_lab2/homework/movie-100k-split/movie-100k_1.txt"

#define FILE2 "/home/quan/code_lab2/homework/movie-100k-split/movie-100k_2.txt"

#define SHM_NAME "/shm_avg_ratings"

#define SHM_SIZE sizeof(float) * 2


// calculate
float calculate(const char *filename) {

    FILE *file = fopen(filename, "r");

    if (!file) {

        perror("ERROR WHEN OPENNING FILE");

        exit(1);

    }



    int userID, movieID, rating;

    long timestamp;

    int count = 0;

    float total = 0.0;



    while (fscanf(file, "%d %d %d %ld", &userID, &movieID, &rating, &timestamp) == 4) {

        total += rating;

        count++;

    }



    fclose(file);

    return (total / count);

}



int main() {

    int shm_fd;
    float *shm_ptr;


    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

    if (shm_fd == -1) {

        perror("ERROR");

        exit(1);

    }


    ftruncate(shm_fd, SHM_SIZE);

    shm_ptr = (float *)mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shm_ptr == MAP_FAILED) {

        perror("ERROR");

        exit(1);

    }



    pid_t pid1 = fork();

    if (pid1 == 0) {

        shm_ptr[0] = calculate(FILE1);

        exit(0);

    }



    pid_t pid2 = fork();

    if (pid2 == 0) {

        shm_ptr[1] = calculate(FILE2);

        exit(0);

    }


    waitpid(pid1, NULL, 0);

    waitpid(pid2, NULL, 0);


    printf("average file 1: %.2f\n", shm_ptr[0]);

    printf("average file 2: %.2f\n", shm_ptr[1]);

    munmap(shm_ptr, SHM_SIZE);

    shm_unlink(SHM_NAME);



    return 0;

}
