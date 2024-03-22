#include<stdio.h>
#include<unistd.h>
#include<sys/shm.h>
#include<stdlib.h>

int main() {
  int shm_id;
  int *share;

  shm_id = shmget (0x2FF, getpagesize(), 0666 | IPC_CREAT);
  if(shm_id == -1){
    printf("shmget()");
    exit(-1);
  }

  share = (int *)shmat(shm_id, 0, 0);
  if(share == NULL){
    printf("shmat()");
    exit(-1);
  }
  // read while don't recieve stop sign - (-1)
  while(1){
    sleep(1);
    if((*share) == -1)
        break;
    printf("Recieved number: %d\n", *share);
  }
  shmdt(share); // detach segment
  // delete segment
  if (shmctl (shm_id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
    printf("server: shared memory remove error");
    exit(-1);
  }
  return 0;
}
