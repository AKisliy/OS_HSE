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
    char msg_text[size * 4];
};

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
    key_t key;
    int msg_id;
    struct msg_buffer messageRec;
    struct msg_buffer msgSend;

    key = ftok("msg_queue_key", 60);
    
    msg_id = msgget(key, 0666 | IPC_CREAT);
    
    ssize_t read_bytes;
    while((read_bytes = msgrcv(msg_id, &messageRec, size, 1, 0)) > 0){
        messageRec.msg_text[read_bytes] = '\0';
        size_t roman = convertToRoman(messageRec.msg_text, msgSend.msg_text, read_bytes);
        msgSend.msg_type = 2;
        if((msgsnd(msg_id, &msgSend, roman ,0)) < 0){
            perror("Failed to send message");
            exit(-1);
        }
        printf("Message sent %s", msgSend.msg_text);
    }
}
