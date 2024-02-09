// code for mark 8
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

// we assume that files contain up to 10000 symbols
const int MAX_SIZE = 10000;

int main(int argc, char** argv){
    if(argc < 3){
        printf("Not enough arguments to complete task!");
        exit(-1);
    }
    char buffer[MAX_SIZE];
    int fdRead, fdWrite;
    
    char* input = argv[1];
    char* output = argv[2];
    
    if((fdRead = open(input, O_RDONLY | O_CREAT, 0777)) < 0){
        printf("Can't open file for reading!\n");
        exit(-1);
    }
    if((fdWrite = open(output, O_WRONLY | O_CREAT, 0777)) < 0){
        printf("Can't open file for writing!\n");
        exit(-1);
    }
    // reading from input file
    ssize_t read_bytes = read(fdRead, buffer, MAX_SIZE - 1);
    if(read_bytes == -1){
        printf("Error occured while reading!\n");
        exit(-1);
    }
    buffer[read_bytes] = '\0';
    
    // writing to output file
    ssize_t written = write(fdWrite, buffer, read_bytes);
    if(written != read_bytes){
        printf("Can't write all string!\n");
        exit(-1);
    }
    
    //closing files
    if(close(fdRead) < 0 || close(fdWrite) < 0){
        printf("Error occured while closing!\n");
        exit(-1);
    }
    printf("Text was succsfully moved!\n");
    return 0;
}
