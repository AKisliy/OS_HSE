// code for mark 10
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// we have buffer of limited size
const int BUF_SIZE = 32;

int main(int argc, char** argv){
    if(argc < 3){
        printf("Not enough arguments to complete task!");
        exit(-1);
    }
    char buffer[BUF_SIZE];
    int fdRead, fdWrite;
    ssize_t read_bytes;
    
    char* input = argv[1];
    char* output = argv[2];
    // open input file(if exists) or create new NOT executable
    if((fdRead = open(input, O_RDONLY | O_CREAT, 0666)) < 0){
        printf("Can't open file for reading!\n");
        exit(-1);
    }
    // get permissions of input file
    struct stat statInput;
    if(stat(input, &statInput) < 0)
        exit(-1);
    mode_t bits = statInput.st_mode;

    // open output file or create with the SAME permissions as input
    if((fdWrite = open(output, O_WRONLY | O_CREAT, bits)) < 0){
        printf("Can't open file for writing!\n");
        exit(-1);
    }
    
    // loop to read and write
    do {
        // read "portion" from the input file
        read_bytes = read(fdRead, buffer, BUF_SIZE);
        if(read_bytes == -1){
            printf("Error occured while reading!\n");
            exit(-1);
        }
        
        // writing to the output file
        ssize_t written = write(fdWrite, buffer, read_bytes);
        if(written != read_bytes){
            printf("Can't write all string!\n");
            exit(-1);
        }
    } while (read_bytes == BUF_SIZE);
    
    //closing files
    if(close(fdRead) < 0 || close(fdWrite) < 0){
        printf("Error occured while closing!\n");
        exit(-1);
    }
    printf("Text was succesfully moved!\n");
    return 0;
}
