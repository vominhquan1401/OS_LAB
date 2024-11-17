#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>

#define FILEPATH "example"
#define SIZE 4096

int main() {
    int fd = open(FILEPATH, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    char *map = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }

    close(fd);

    printf("Reader: Read message from shared memory: %s\n", map);

    if (munmap(map, SIZE) == -1) {
        perror("Error unmapping memory");
        return 1;
    }

    return 0;
}
