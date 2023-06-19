#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t is_received = 0;

void signalHandler(int sig) {
    if (sig == SIGUSR2) {
        is_received = 1;
    }
}

int main() {
    pid_t receiver_pid;

    signal(SIGUSR2, signalHandler);

    printf("Sender PID: %d\nEnter Receiver PID: ", getpid());
    scanf("%d", &receiver_pid);

    printf("Enter an integer: ");
    int number;
    scanf("%d", &number);

    while (!is_received) {
        usleep(100);
    }

    for (int i = 0; i < 32; ++i) {
        is_received = 0;
        kill(receiver_pid, ((number >> i) & 1) ? SIGUSR2 : SIGUSR1);
        while (!is_received) {
            usleep(100);
        }
    }
    kill(receiver_pid, SIGINT);
}
