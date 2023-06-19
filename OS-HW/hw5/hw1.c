#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SIZE 80

void open_fifo_write(char* name, int* fd) {
    *fd = open(name, O_WRONLY);
    if (*fd < 0) {
        printf("Error opening FIFO\n");
        _exit(1);
    }
}

void open_fifo_read(char* name, int* fd) {
    *fd = open(name, O_RDONLY);
    if (*fd < 0) {
        printf("Error opening FIFO\n");
        _exit(1);
    }
}

int main(int argc, char* argv[]) {
    int fd;
    char* name;

    if (argc != 2) {
        printf("You should input name of the buffer file via command line arguments");
        return 1;
    }

    name = argv[1];
    mkfifo(name, 0666);

    char arr1[SIZE], arr2[SIZE];

    while (1) {
        open_fifo_write(name, &fd);
        printf("Input message: ");
        fgets(arr2, SIZE, stdin);
        write(fd, arr2, strlen(arr2) + 1);
        close(fd);

        open_fifo_read(name, &fd);
        read(fd, arr1, SIZE);
        printf("Received from hw2: %s\n", arr1);
        close(fd);
    }
    return 0;
}
