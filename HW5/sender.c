#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

volatile bool rec_received = false;
volatile int rec_pid = 0;

void handle_reciever_signal(int sig){
    rec_received = true;
}

void handle_unexpected_exit(int sig){
    kill(rec_pid, SIGINT);
    exit(0);
}

int main(){
    pid_t id = getpid();
    printf("Hey, I'm sender with pid:%d\n", id);
    printf("Please enter pid of the sender:");
    int input;
    scanf("%d", &input);
    rec_pid = input;
    
    signal(SIGUSR1, handle_reciever_signal);
    signal(SIGINT, handle_unexpected_exit);
    
    int number;
    printf("Enter an integer to send:");
    scanf("%d", &number);
    
    for (int i = 31; i >= 0; i--) {
        rec_received = false;
        int bit = (number >> i) & 1;
        if (bit) {
            kill(rec_pid, SIGUSR2);
        }
        else {
            kill(rec_pid, SIGUSR1);
        }
        while (!rec_received) {}
    }

    // say to reciever that chat is ended
    kill(rec_pid, SIGINT);
    return 0;
}
