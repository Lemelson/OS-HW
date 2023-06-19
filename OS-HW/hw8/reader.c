#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256
#define SEM_WRITER "/sem_writer"
#define SEM_READER "/sem_reader"

int main() {
    sem_t *sem_writer, *sem_reader;
    FILE *buffer;
    char ring_buffer[BUFFER_SIZE];
    int read_pos = 0;

    if ((sem_writer = sem_open(SEM_WRITER, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("Couldn't open semaphore writer");
        exit(1);
    }

    if ((sem_reader = sem_open(SEM_READER, O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("Couldn't open semaphore reader");
        exit(1);
    }

    sem_post(sem_writer);

    while (1) {
        sem_wait(sem_reader);

        if ((buffer = fopen("buffer.txt", "r")) < 0) {
            perror("Can't open file for reading");
            exit(1);
        }

        fseek(buffer, read_pos, SEEK_SET);
        fgets(ring_buffer, 6, buffer);

        if (strcmp(ring_buffer, "exit\n") == 0) {
            fclose(buffer);
            sem_post(sem_writer);
            sem_close(sem_writer);
            sem_close(sem_reader);
            sem_unlink(SEM_WRITER);
            sem_unlink(SEM_READER);
            break;
        }

        printf("Reader read: %s\n", ring_buffer);
        memset(ring_buffer, 0, sizeof(ring_buffer));

        read_pos = ftell(buffer);
        fclose(buffer);

        sem_post(sem_writer);
    }

    return 0;
}
