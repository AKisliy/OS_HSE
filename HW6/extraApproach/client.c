// write a random number between 0 and 999 to the shm every 1 second
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#include "message.h"

int main(int argc, char* argv[]){
  int shmid;
  message_t *msg_p;
  int num;
  int iterations = 10; // by default client will make 10 iterations
  if(argc > 1){
      int input = atoi(argv[1]);
      if(input > 0){
          iterations = input;
      }
  }

  srand(time(NULL));
  shmid = shmget(SHM_ID, sizeof (message_t), PERMS | IPC_CREAT);
  printf("shm_id = %d\n", shmid);
  if(shmid < 0){
    printf("shmget()");
    exit(-1);
  }

  if ((msg_p = (message_t *) shmat (shmid, 0, 0)) == NULL) {
    printf("client: shared memory attach error");
    exit(-1);
  }

  while(iterations){
    num = random() % 1000;
    msg_p->type = MSG_TYPE_INT;
    msg_p->val = num;
    // wait until message is proccessed
    while(msg_p->type != MSG_TYPE_EMPTY){}
    --iterations;
  }

  msg_p->type = MSG_TYPE_FINISH; // sign that client's sent all numbers
  shmdt (msg_p); // detach segment
  return 0;
}
