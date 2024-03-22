// write a random number between 0 and 999 to the shm every 1 second
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]){
  int shm_id;
  int *share;
  int num;
  int iterations = 10; // by default client will make 10 iterations
  if(argc > 1){
      int input = atoi(argv[1]);
      if(input > 0){
          iterations = input;
      }
  }

  srand(time(NULL));
  shm_id = shmget(0x2FF, getpagesize(), 0666 | IPC_CREAT);
  printf("shm_id = %d\n", shm_id);
  if(shm_id < 0){
    printf("shmget()");
    exit(-1);
  }

  share = (int *)shmat(shm_id, 0, 0);
  if(share == NULL){
    printf("shmat()");
    exit(-1);
  }

  while(iterations){
    num = random() % 1000;
    *share = num;
    sleep(1);
    --iterations;
  }

  *share = -1; // sign that client's sent all numbers
  shmdt (share); // detach segment
  return 0;
}
