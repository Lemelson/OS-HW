#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256
#define SEM_WRITER "/sem_writer"
#define SEM_READER "/sem_reader"

int main(int argc, char* argv[]) {
    sem_t *sem_writer, *sem_reader;
    FILE *buffer;
    int write_pos = 0;

    if (argc != 2) {
        perror("You should enter some text");
        exit(1);
    }

    if ((sem_writer = sem_open(SEM_WRITER, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("Couldn't open semaphore writer");
        exit(1);
    }

    if ((sem_reader = sem_open(SEM_READER, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("Couldn't open semaphore reader");
        exit(1);
    }

    if ((buffer = fopen("buffer.txt", "w")) < 0) {
        perror("Can't open file for writing");
        exit(1);
    }
    fclose(buffer);

    size_t size = strlen(argv[1]);
    size_t i = 0;

    while (i < size) {
        sem_wait(sem_writer);

        if ((buffer = fopen("buffer.txt", "r+")) < 0) {
            perror("Can't open file for writing");
            exit(1);
        }

        fseek(buffer, write_pos, SEEK_SET);

        char chunk[6] = {0};
        strncpy(chunk, &argv[1][i], 5);
        chunk[5] = '\0';

        fprintf(buffer, "%s", chunk);
        printf("Writer wrote: %s\n", chunk);
        fflush(buffer);

        write_pos = ftell(buffer);
        fclose(buffer);

        i += 5;
        sem_post(sem_reader);
    }

    sem_wait(sem_writer);
    if ((buffer = fopen("buffer.txt", "a")) < 0) {
        perror("Can't open file for writing");
        exit(1);
    }
    fprintf(buffer, "exit\n");
    fclose(buffer);
    sem_post(sem_reader);

    sem_wait(sem_writer);
    sem_close(sem_writer);
    sem_close(sem_reader);

    printf("\nAll operations done\n");

    return 0;
}
