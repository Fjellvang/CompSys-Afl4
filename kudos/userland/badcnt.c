#include "lib.h"
#define tostart "[disk]threadbad"

volatile long cnt = 0;


int main(){
  int niters = 20000;
  
  pid_t pid1 = syscall_spawn(tostart, 0);
  pid_t pid2 = syscall_spawn(tostart, 0);
  syscall_join(pid1);
  syscall_join(pid2);

  if(cnt > 2*niters) printf("boom");
  else printf("hohohohoho");

  return 0;
}


void increment(){
  cnt++;

}

