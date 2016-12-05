#include "lib.h"

#define tostart "[disk]thread"
#define HOWMANY 2

volatile int count = 0;
int niters = 20000;

int main(void) {

  pid_t pid1 = syscall_spawn(tostart, 0);
  pid_t pid2 = syscall_spawn(tostart, 0);
  syscall_join(pid1);
  syscall_join(pid2);

  if(count != 2*niters){
    printf("BOOM");
  } else
    printf("no boom");
  return 0;
}


void increment(){
    count++;
}

