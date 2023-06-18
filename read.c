#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

struct shmem {
  int num;
  int endflag;
};
struct shmem *share;
int shm_id;

void safe_exit() {
  printf("Safely exiting\n");
  if (share->endflag) {
    shmdt((char *) share);
    shmctl(shm_id, IPC_RMID, NULL);
    printf("Shared memory removed\n");
  } else {
    share->endflag = 1;
    shmdt((char *) share);
  }
  exit(0);
}

int main() {
  signal(SIGINT, safe_exit);
  shm_id = shmget(0x2FF, sizeof(struct shmem), 0666 | IPC_CREAT);
  if (shm_id == -1) {
    perror("shmget()");
    exit(1);
  }

  share = (struct shmem *) shmat(shm_id, 0, 0);
  if (share == NULL) {
    perror("shmat()");
    exit(2);
  }

  while (1) {
    sleep(1);
    if (share->endflag) {
      safe_exit();
    }
    printf("%d\n", share->num);
  }

  return 0;
}
