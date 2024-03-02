// code for 8 points
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

const int size = 128;

ssize_t readInformation(char* path, char* dest, int size){
    int fd;
    if((fd = open(path, O_RDONLY)) < 0){
      printf("Can\'t open file\n");
      exit(-1);
    }
    ssize_t read_bytes = read(fd, dest, size);
    if(read_bytes == -1){
        printf("Can\'t read from the file\n");
        exit(-1);
    }
    dest[read_bytes] = '\0';
    if(close(fd) < 0){
        printf("Can\'t close file\n");
        exit(-1);
    }
    return read_bytes;
}

ssize_t writeInformation(char* path,char* buffer, size_t size){
    int fdWrite;
    if((fdWrite = open(path, O_WRONLY | O_CREAT, 0666)) < 0){
        printf("Can't open file for writing!\n");
        exit(-1);
    }
    
    ssize_t written = write(fdWrite, buffer, size);
    if(written != size){
        printf("Can't write all string!\n");
        exit(-1);
    }
    
    if(close(fdWrite) < 0){
        printf("Can\'t close file\n");
        exit(-1);
    }
    return written;
}

int main(int argc, char** argv){
    if(argc < 3){
        printf("Not enough arguments to complete task!");
        exit(-1);
    }

    // files' paths for reading and writing
    char* input = argv[1];
    char* output = argv[2];

    char buffer[size + 1];

    char reader2Handler[] = "p1.fifo";
    char handler2Reader[] = "p2.fifo";
    int fd, fdInput, fdOutput;
    ssize_t read_bytes;

    mknod(reader2Handler, S_IFIFO | 0666, 0);
    mknod(handler2Reader, S_IFIFO | 0666, 0);
    // descriptors for file and FIFO
    if((fd = open(reader2Handler, O_WRONLY)) < 0){
      printf("Can\'t open FIFO for writting\n");
      exit(-1);
   }
    if((fdInput = open(input, O_RDONLY)) < 0){
      printf("Can\'t open file\n");
      exit(-1);
    }
    if((fdOutput = open(output, O_WRONLY | O_CREAT, 0666)) < 0){
        printf("Can't open file for writing!\n");
        exit(-1);
    }

    do {
      read_bytes = read(fdInput, buffer, size);
      if(read_bytes == -1) {
        printf("Can\'t write this file\n");
        exit(-1);
      }
      buffer[read_bytes] = '\0';
      size_t written = write(fd, buffer, read_bytes);
      if(written != read_bytes){
          printf("Can\'t write all string to FIFO\n");
          exit(-1);
      }
      // write(1, buffer, read_bytes);
    } while(read_bytes == size);
    
    if(close(fd) < 0) {
        printf("reader: Can\'t close FIFO\n");
        exit(-1);
    }

    if(close(fdInput) < 0) {
      printf("Can\'t close file\n");
    }
    
    if((fd = open(handler2Reader, O_RDWR)) < 0){
        printf("Can\'t open FIFO for reading\n");
        exit(-1);
    }
    
    char infoFromHandler[size + 1];
    do {
        read_bytes = read(fd, infoFromHandler, size);
        if(read_bytes <= 0){
            printf("Can\'t read string from FIFO\n");
            exit(-1);
        }
        infoFromHandler[read_bytes] = '\0';
        ssize_t written = write(fdOutput, infoFromHandler, read_bytes);
        if(written != read_bytes){
            printf("Can't write all string!\n");
            exit(-1);
        }
        printf("%s\n", infoFromHandler);
    } while (read_bytes == size);
    
    if(close(fd) < 0){
        printf("Can\'t close FIFO");
        exit(-1);
    }
    if(close(fdOutput) < 0){
        printf("Can\'t close file for writing");
        exit(-1);
    }
}
