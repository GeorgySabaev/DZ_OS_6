#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
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
    shmdt((char*) share);
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
  int num;

  srand(time(NULL));
  shm_id = shmget(0x2FF, sizeof(struct shmem), 0666 | IPC_CREAT);
  printf("shm_id = %d\n", shm_id);
  if (shm_id < 0) {
    perror("shmget()");
    exit(1);
  }

  /* подключение сегмента к адресному пространству процесса */
  share = (struct shmem *) shmat(shm_id, 0, 0);
  if (share == NULL) {
    perror("shmat()");
    exit(2);
  }
  printf("share = %p\n", share);

  while (1) {
    num = random() % 1000;
    share->num = num;
    printf("write a random number %d\n", num);
    sleep(1);
    if (share->endflag) {
      safe_exit();
    }
  }
  return 0;
}
