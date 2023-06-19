#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd;
    char *ch;

    // Shared memory initialization
    if ((fd = shm_open("shared", O_RDWR | O_CREAT, 0777)) == -1) {
        perror("Can't create shared memory\n");
        return 0;
    }

    if (ftruncate(fd, 2) == -1) {
        perror("Can't set the size\n");
        return 0;
    }

    ch = mmap(NULL, 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ch == MAP_FAILED) {
        perror("Can't mmap objects\n");
        return 0;
    }

    // Seed random number generator
    srand(time(NULL));

    // ch[1] is used as a shutdown flag. It is set to 1 to indicate that the client should shutdown
    ch[1] = 0;

    // Generate random numbers
    for (int i = 0; i < 5; ++i) {
        ch[0] = rand() % 10;
        sleep(2);

        // Check for shutdown flag
        if(ch[1] == 1) {
            break;
        }
    }

    // Set shutdown flag before exiting
    ch[1] = 1;
}
