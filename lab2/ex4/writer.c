#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>

#define FILEPATH "example"
#define SIZE 4096

int main() {
    int fd = open(FILEPATH, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    if (ftruncate(fd, SIZE) == -1) {
        perror("Error resizing file");
        close(fd);
        return 1;
    }

    char *map = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }

    close(fd);

    printf("Writer: Enter a message: ");
    fgets(map, SIZE, stdin);
    printf("Message written to shared memory: %s\n", map);

    sleep(30);

    if (munmap(map, SIZE) == -1) {
        perror("Error unmapping memory");
        return 1;
    }

    return 0;
}
