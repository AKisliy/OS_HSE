#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

volatile int result = 0;
volatile bool ended = false;
volatile int sender_pid = 0;

void getByteOne(int sig){
    result = (result << 1) | 1;
    kill(sender_pid, SIGUSR1);
}

void getByteZero(int sig){
    result = result << 1;
    kill(sender_pid, SIGUSR1);
}

void handle_ending(int sig){
    ended = true;
}

int main(){
    pid_t id = getpid();
    printf("Hey, I'm receiver with pid:%d\n", id);
    printf("Please enter pid of the sender:");
    int input;
    scanf("%d", &input);
    sender_pid = input;
    printf("Waiting for number...\n");
    
    signal(SIGUSR1, getByteZero);
    signal(SIGUSR2, getByteOne);
    signal(SIGINT, handle_ending);
    // wait for ending of chat

    while(!ended){}
    
    printf("Receiver sent number: %d\n", result);
    return 0;
}
