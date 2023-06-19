#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int data = 0;
int bitPos = 0;
pid_t sender_pid;

void signalHandler(int sig) {
    if (sig == SIGUSR1) {
        data |= (0 << bitPos++);
    } else if (sig == SIGUSR2) {
        data |= (1 << bitPos++);
    } else if (sig == SIGINT) {
        printf("Received number: %d\n", data);
        exit(0);
    }
    kill(sender_pid, SIGUSR2);
}

int main() {
    signal(SIGUSR1, signalHandler);
    signal(SIGUSR2, signalHandler);
    signal(SIGINT, signalHandler);

    printf("Receiver PID: %d\nEnter Sender PID: ", getpid());
    scanf("%d", &sender_pid);
    kill(sender_pid, SIGUSR2);

    while (1) pause();
}
