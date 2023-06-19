#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>

char buffer[1];

void perform_child_operations(int* pipe_descriptor, int semaphore_id1, int semaphore_id2) {
    struct sembuf semaphore_struct_parent = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
    struct sembuf semaphore_struct_child = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};

    if (semop(semaphore_id2, &semaphore_struct_child, 1) < 0) {
        fprintf(stderr, "Unable to subtract 1 from the reader's semaphore\n");
        exit(0);
    }

    char child_read[1];
    read(pipe_descriptor[0], child_read, 1);
    fprintf(stdout, "Child read: %s\n", child_read);

    if (semop(semaphore_id1, &semaphore_struct_parent, 1) < 0) {
        fprintf(stderr, "Unable to add 1 to the writer's semaphore\n");
        exit(0);
    }
    exit(0);
}


void perform_parent_operations(int* pipe_descriptor, int semaphore_id1, int semaphore_id2) {
    struct sembuf semaphore_struct_parent = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
    struct sembuf semaphore_struct_child = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};

    if (semop(semaphore_id1, &semaphore_struct_parent, 1) < 0) {
        fprintf(stderr, "Unable to subtract 1 from the writer's semaphore\n");
        exit(0);
    }

    char generated = (char)('0' + rand() % 10);
    buffer[0] = generated;
    write(pipe_descriptor[1], &buffer, 1);
    fprintf(stdout, "Parent wrote: %d\n", (int)(generated - '0'));

    if (semop(semaphore_id2, &semaphore_struct_child, 1) < 0) {
        fprintf(stderr, "Unable to add 1 to the reader's semaphore\n");
    }
}

int main() {
    srand(time(NULL));

    int i = 0;
    int pipe_descriptor[2];

    if (pipe(pipe_descriptor) < 0) {
        fprintf(stderr, "Error occurred while creating the pipe\n");
        exit(0);
    }

    char pathname1[] = "writer_semaphore.c";
    char pathname2[] = "reader_semaphore.c";

    key_t key1 = ftok(pathname1, 0);
    key_t key2 = ftok(pathname2, 0);

    int semaphore_id1, semaphore_id2;

    if ((semaphore_id1 = semget(key1, 1, 0666 | IPC_CREAT)) < 0) {
        fprintf(stderr, "Unable to create the semaphore\n");
        exit(0);
    }

    if ((semaphore_id2 = semget(key2, 1, 0666 | IPC_CREAT)) < 0) {
        fprintf(stderr, "Unable to create the semaphore\n");
        exit(0);
    }

    semctl(semaphore_id1, 0, SETVAL, 0);
    semctl(semaphore_id2, 0, SETVAL, 1);

    while (i < 10) {
        ++i;
        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork error\n");
            exit(0);
        } else if (pid == 0) {
            perform_child_operations(pipe_descriptor, semaphore_id1, semaphore_id2);
            if (i == 10) {
                close(pipe_descriptor[0]);
                close(pipe_descriptor[1]);
            }
            exit(0);
        } else {
            perform_parent_operations(pipe_descriptor, semaphore_id1, semaphore_id2);
        }
        wait(0);
    }


    if (close(pipe_descriptor[0]) != 0 || close(pipe_descriptor[1]) != 0) {
        fprintf(stderr, "Error occurred while deleting the pipe\n");
        exit(0);
    }

    semctl(semaphore_id1, 0, IPC_RMID, 0);
    semctl(semaphore_id2, 0, IPC_RMID, 0);

    printf("The end\n");

    return 0;
}
