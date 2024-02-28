// code for 8 points
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

const int size = 5000;

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
    printf("wtf is happening\n");
    if(argc < 3){
        printf("Not enough arguments to complete task!");
        exit(-1);
    }

    // files' paths for reading and writing
    char* input = argv[1];
    char* output = argv[2];

    char buffer[size];

    char reader2Handler[] = "p1.fifo";
    char handler2Reader[] = "p2.fifo";
    int fd;

    mknod(reader2Handler, S_IFIFO | 0666, 0);
    mknod(handler2Reader, S_IFIFO | 0666, 0);
    
    ssize_t readFromFile = readInformation(input, buffer, size);
    if((fd = open(reader2Handler, O_WRONLY)) < 0){
      printf("Can\'t open FIFO for writting\n");
      exit(-1);
   }

    size_t written = write(fd, buffer, readFromFile);
    if(written != readFromFile){
        printf("Can\'t write all string to FIFO\n");
        exit(-1);
    }
    if(close(fd) < 0) {
        printf("reader: Can\'t close FIFO\n");
        exit(-1);
    }
    if((fd = open(handler2Reader, O_RDWR)) < 0){
        printf("Can\'t open FIFO for reading\n");
        exit(-1);
    }
    char infoFromHandler[size * 4];
    ssize_t readFromHandler = read(fd, infoFromHandler, size * 4);
    if(readFromHandler < 0){
        printf("Can\'t read string from FIFO\n");
        exit(-1);
    }

    ssize_t writeToFile = writeInformation(output, infoFromHandler, readFromHandler);
    if(writeToFile < readFromHandler){
        printf("Can\'t write everything to file");
        exit(-1);
    }
    if(close(fd) < 0){
        printf("Can\'t close FIFO");
        exit(-1);
    }
}
