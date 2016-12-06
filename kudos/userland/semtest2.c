#include "lib.h"


int main(void){
  printf("HEJ PROGRAM1\n");
  sem_t* sem = syscall_sem_open("mutex",1);
  printf("sem lavet\n");
  syscall_sem_p(sem);
  printf("HEJ PROGRAM1\n");
  if(sem){
    printf("YOYOYO");
  } 

  return 0;

}
