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

struct romandata_t { unsigned int value; char const* numeral; };
const struct romandata_t romandata[] =
{
    {9, "IX"}, { 8, "VIII"},
    { 7, "VII"}, { 6, "VI"},
    { 5, "V"}, { 4, "IV"},
    { 3, "III"}, { 2, "II"},
    { 1, "I"},
};

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
    char reader2Handler[] = "p1.fifo";
    char handler2Reader[] = "p2.fifo";

    int fd1, fd2;
    
    char infoFromFifo[size + 1];
    char romanNotation[size * 4 + 1];

    if((fd1 = open(reader2Handler, O_RDONLY)) < 0 | (fd2 = open(handler2Reader, O_WRONLY)) < 0){
      printf("Can\'t open FIFO\n");
      exit(-1);
    }
    
    ssize_t readFromFifo;
    do {
        readFromFifo = read(fd1,infoFromFifo, size);
        if(readFromFifo < 0){
            printf("Can\'t read string from FIFO\n");
            exit(-1);
        }
        infoFromFifo[readFromFifo] = '\0';
        ssize_t romanSize = convertToRoman(infoFromFifo, romanNotation, readFromFifo);
        size_t written = write(fd2, romanNotation, romanSize);
        if(written != romanSize){
            printf("Can\'t write all string to FIFO\n");
            exit(-1);
        }
    } while (readFromFifo == size);
    
    if((close(fd1)) < 0) {
        printf("handler: Can\'t close FIFO\n");
        exit(-1);
    }

    if(close(fd2) < 0) {
        printf("handler: Can\'t close FIFO\n");
        exit(-1);
    }
}
