#include "lib.h"
#define tostart "[disk]1"

int main(void){
  sem_t* sem1 = syscall_sem_open("mutex", 0);
  sem_t* sem2 = syscall_sem_open("mutex2",0);
  syscall_spawn(tostart, 0);
  
  printf("Hej program2");

  syscall_sem_close(sem1);
  syscall_sem_close(sem2);

  return 0;
}
