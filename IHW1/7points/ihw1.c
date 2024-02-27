// code for 7 points
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
        printf("Not enough arguments to complete task!\n");
        printf("Input should look like: ./a.out {path to input file} {path to output file}\n");
        exit(-1);
    }
    
    // files' paths for reading and writing
    char* input = argv[1];
    char* output = argv[2];

    char readerToHandler[] = "a1.fifo";
    char handlerToReader[] = "a2.fifo";
    // make nodes without checking of existence 
    mknod(readerToHandler, S_IFIFO | 0666, 0);
    mknod(handlerToReader, S_IFIFO | 0666, 0);

    char buffer[size];
    
    int pid = fork();
    if(pid < 0){
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (pid > 0){ // let's assume that parent will read information from file and then write it
            ssize_t readFromFile = readInformation(input, buffer, size);
            int fd1, fd2;

            if((fd1 = open(readerToHandler, O_WRONLY)) < 0){
                printf("Can\'t open FIFO for writing\n");
                exit(-1);
            }

            size_t written = write(fd1, buffer, readFromFile);
            if(written != readFromFile){
                printf("Can\'t write all string to FIFO\n");
                exit(-1);
            }

            if(close(fd1) < 0) {
                printf("reader: Can\'t close writing side of FIFO\n");
                exit(-1);
            }

            if((fd2 = open(handlerToReader, O_RDONLY)) < 0){
                printf("Can\'t open FIFO for reading\n");
                exit(-1);
            }

            char infoFromHandler[size * 4];
            ssize_t readFromHandler = read(fd2, infoFromHandler, size * 4);
            if(readFromHandler < 0){
                printf("Can\'t read string from FIFO\n");
                exit(-1);
            }

            ssize_t writeToFile = writeInformation(output, infoFromHandler, readFromHandler);
            if(writeToFile < readFromHandler){
                printf("Can\'t write everything to file");
                exit(-1);
            }

            if(close(fd2) < 0){
                printf("Can\'t close FIFO");
                exit(-1);
            }
    } else { // child will be the handler
            int fd1, fd2;
            char infoFromFifo[size];

            if((fd1 = open(readerToHandler, O_RDONLY)) < 0){
                printf("Can\'t open FIFO for reading\n");
                exit(-1);
            }

            ssize_t readFromFifo = read(fd1,infoFromFifo, size);
            
            if(readFromFifo < 0){
                printf("Can\'t read string from FIFO\n");
                exit(-1);
            }

            if(close(fd1) < 0) {
                printf("handler: Can\'t close reading side of FIFO\n");
                exit(-1);
            }

            char romanNotation[size * 4];
            ssize_t romanSize = convertToRoman(infoFromFifo, romanNotation, readFromFifo);
            // now let's write this info to different pipe
            if((fd2 = open(handlerToReader, O_WRONLY)) < 0){
                printf("Can\'t open FIFO for writing\n");
                exit(-1);
            }

            size_t written = write(fd2, romanNotation, romanSize);
            if(written != romanSize){
                printf("Can\'t write all string to FIFO\n");
                exit(-1);
            }

            if(close(fd2) < 0) {
                printf("handler: Can\'t close writing side of FIFO\n");
                exit(-1);
            }
        }
}
