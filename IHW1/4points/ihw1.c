#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

const int size = 5000;

struct romandata_t { unsigned int value; char const* numeral; };
const struct romandata_t romandata[] =
{
    {9, "IX"}, { 8, "VIII"},
    { 7, "VII"}, { 6, "VI"},
    { 5, "V"}, { 4, "IV"},
    { 3, "III"}, { 2, "II"},
    { 1, "I"},
};

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

ssize_t convertToRoman(char* src, char* dest, size_t srcSize){
    ssize_t index = 0;
    for(size_t i = 0; i < srcSize; ++i){
        if(isdigit(src[i]) && src[i] > '0'){
            dest[index++]='<';
            int num = src[i] - '0';
            const struct romandata_t* r = romandata;
            const char* romanNumber;
            while(r->value != num){
                ++r;
            }
            romanNumber = r->numeral;
            for(int i = 0; i < strlen(romanNumber); ++i)
                dest[index++] = romanNumber[i];
            dest[index++] = '>';
        }else{
            dest[index++] = src[i];
        }
    }
    dest[index] = '\0';
    return index;
}

int main(int argc, char** argv){
    if(argc < 3){
        printf("Not enough arguments to complete task!");
        exit(-1);
    }
    
    // files' paths for reading and writing
    char* input = argv[1];
    char* output = argv[2];
    char buffer[size];
    
    int fdHandlerWriter[2]; // pipe's descriptors for handler -> writer
    if(pipe(fdHandlerWriter) < 0){
        printf("Can\'t open pipe\n");
        exit(-1);
    }
    
    int fdReaderHandler[2]; // handler - thread which is responsible for
    if(pipe(fdReaderHandler) < 0){
        printf("Can\'t open pipe\n");
        exit(-1);
    }
    
    int pid = fork();
    if(pid < 0){
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (pid > 0){ // let's assume that parent will read information from file and then pass it
        int readerHandler = fork();
        if(readerHandler < 0){
            printf("Can\'t fork child\n");
            exit(-1);
        } else if(readerHandler > 0){ // we are parent now, let's read info and write to pipe
            ssize_t readFromFile = readInformation(input, buffer, size);
            
            if(close(fdReaderHandler[0]) < 0){
                printf("parent: Can\'t close reading side of pipe\n");
                exit(-1);
            }
            size_t written = write(fdReaderHandler[1], buffer, readFromFile);
            if(written != readFromFile){
                printf("Can\'t write all string to pipe\n");
                exit(-1);
            }
            if(close(fdReaderHandler[1]) < 0) {
                printf("parent: Can\'t close writing side of pipe\n");
                exit(-1);
            }
        } else{
            char infoFromPipe[size];
            if(close(fdReaderHandler[1]) < 0){
                printf("handler: Can\'t close writing side of pipe\n");
                exit(-1);
            }
            ssize_t readFromPipe = read(fdReaderHandler[0],infoFromPipe, size);
            //printf("read from pipe: %s", infoFromPipe);
            if(readFromPipe < 0){
                printf("Can\'t read string from pipe\n");
                exit(-1);
            }
            char romanNotation[size * 4];
            close(fdReaderHandler[0]);
            ssize_t romanSize = convertToRoman(infoFromPipe, romanNotation, readFromPipe);
            // now let's write this info to different pipe
            if(close(fdHandlerWriter[0]) < 0){
                printf("handler: Can\'t close reading side of pipe\n");
                exit(-1);
            }
            size_t written = write(fdHandlerWriter[1], romanNotation, romanSize);
            if(written != romanSize){
                printf("Can\'t write all string to pipe\n");
                exit(-1);
            }
            if(close(fdHandlerWriter[1]) < 0) {
                printf("handler: Can\'t close writing side of pipe\n");
                exit(-1);
            }
        }
    } else {
        char infoFromPipe[size];
        if(close(fdHandlerWriter[1]) < 0){
            printf("Writer to file: Can\'t close writing side of pipe\n");
            exit(-1);
        }
        ssize_t readFromPipe = read(fdHandlerWriter[0],infoFromPipe, size);
        //printf("write to file read from pipe: %s", infoFromPipe);
        if(readFromPipe < 0){
            printf("Can\'t read string from pipe\n");
            exit(-1);
        }
        size_t writtenToFile = writeInformation(output, infoFromPipe ,readFromPipe);
        if(writtenToFile != readFromPipe){
            printf("Can't write all string to file!\n");
            exit(-1);
        }
        if(close(fdHandlerWriter[0]) < 0){
            printf("Writer to file: Can\'t close reading side of pipe\n");
            exit(-1);
        }
    }
}
