#include "proc/usr_sem.h"
#include "kernel/interrupt.h"
#include "kernel/sleepq.h"
#include "lib/libc.h"

static usr_sem_t user_sem_list[MAX_SEMPAHORES];
static spinlock_t semaphore_table_slock;

void usr_sem_init(){
 for(int i = 0; i < MAX_SEMPAHORES; i++){
   user_sem_list[i].creator= -1;
 }

}

usr_sem_t* usr_sem_open(const char* name, int value){
  // CHECK STRING LENGTH???
  int i;

  interrupt_status_t intr_status;
  // check om navnet existerer
  int sem_exists = usr_sem_exists(name);

  if(strlen(name) >= MAX_SEMPAHORES){
    //navn for langt. fejl
    kprintf("SEMAPHORE NAME TOO LONG");
    return NULL;
  }

  if(value >= 0){
    // value er pos eller 0, så lav ny sem med navn.
    // hvis navn findes, returner den med navn
    // lås før vi laver ny semaphore
    int index;
    if((index = sem_exists) != -1){
      return &user_sem_list[index];
    }else{
    intr_status = _interrupt_disable();
      spinlock_acquire(&semaphore_table_slock);
      for(i = 0; i < MAX_SEMPAHORES; i++){
        if(user_sem_list[i].creator == -1){
          user_sem_list[i].creator = i;
          stringcopy(&user_sem_list[i].name, name, strlen(name)+1);
          break;
        }
      } 
      spinlock_release(&semaphore_table_slock);
      _interrupt_set_state(intr_status);
    }
  }
  else{
    // val er negativ så return semaphore med navn
    for(i = 0; i < MAX_SEMPAHORES; i++){
      if(stringcmp(name, user_sem_list[i].name)){
        return &user_sem_list[i]; 
      }
    }
  }
  if(i == MAX_SEMPAHORES) return NULL;
}

int usr_sem_exists(const char* name){
    for(int i = 0; i < MAX_SEMPAHORES; i++){
      if(stringcmp(name, user_sem_list[i].name)){
        //name does exists
        return i;
      }
    }
    //name does not exists
    return -1;
}



int usr_sem_close(usr_sem_t* sem){
  // not assigned
  if(sem->creator == -1 || sem->value < 0) return -1;
 
  return 0;
}


int usr_sem_p(usr_sem_t* sem){
  //semaphore not assigned
  if(sem->creator == -1) return -1;

  interrupt_status_t intr_status;

  intr_status = _interrupt_disable();
  spinlock_acquire(&sem->slock);

  sem->value--;
  if (sem->value < 0) {
    sleepq_add(sem);
    spinlock_release(&sem->slock);
    thread_switch();
  } else {
    spinlock_release(&sem->slock);
  }
  _interrupt_set_state(intr_status);

  return 0;
}


int usr_sem_v(usr_sem_t* sem){
  // if not assigned.
  if(sem->creator == -1) return -1;

  interrupt_status_t intr_status;
    
  intr_status = _interrupt_disable();
  spinlock_acquire(&sem->slock);

  sem->value++;
  if (sem->value <= 0) {
    sleepq_wake(sem);
  }

  spinlock_release(&sem->slock);
  _interrupt_set_state(intr_status);

  return 0;
}
