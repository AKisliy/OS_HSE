// code for 10 points
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

const int size = 128;

struct msg_buffer {
    long msg_type;
    char msg_text[size * 4 + 1];
};

int main(int argc, char** argv){
    printf("Inside");
    fflush(stdout);
    if(argc < 3){
        printf("Not enough arguments to complete task!");
        exit(-1);
    }

    // files' paths for reading and writing
    char* input = argv[1];
    char* output = argv[2];

    key_t key;
    int msg_id;
    struct msg_buffer message;

    key = ftok("msg_queue_key", 60);
    
    msg_id = msgget(key, 0666 | IPC_CREAT);
    
    int fdInput, fdOutput;
    ssize_t read_bytes;

    if((fdInput = open(input, O_RDONLY)) < 0){
      printf("Can\'t open file\n");
      exit(-1);
    }
    if((fdOutput = open(output, O_WRONLY)) < 0){
        printf("Can't open file for writing!\n");
        exit(-1);
    }
    
    do {
      message.msg_type = 1;
      read_bytes = read(fdInput, message.msg_text, size);
      if(read_bytes == -1) {
        printf("Can\'t write this file\n");
        exit(-1);
      }
        
      if ((msgsnd(msg_id, &message, read_bytes, 0)) == -1) {
         perror("Failed to send message");
         exit(-1);
      }
        ssize_t bytes_recieved = msgrcv(msg_id, &message, size * 4 ,2, 0);
      if (bytes_recieved < 0){
          perror("Failed to receive message");
          exit(-1);
      }
      message.msg_text[bytes_recieved] = '\0';
      size_t written = write(fdOutput, message.msg_text, bytes_recieved);
      if(written != bytes_recieved){
          printf("Can\'t write all string to FIFO\n");
          exit(-1);
      }
      // write(1, buffer, read_bytes);
    } while(read_bytes == size);

    if(close(fdInput) < 0) {
      printf("Can\'t close file\n");
      exit(-1);
    }
    if(close(fdOutput) < 0){
        printf("Can\'t close file for writing");
        exit(-1);
    }
    msgctl(msg_id, IPC_RMID, NULL);
}
