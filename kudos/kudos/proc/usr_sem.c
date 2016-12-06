#include "proc/usr_sem.h"
#include "kernel/interrupt.h"
#include "kernel/sleepq.h"
#include "lib/libc.h"
//#include "kernel/semaphore.h"

static usr_sem_t user_sem_list[MAX_SEMPAHORES];
static spinlock_t semaphore_table_slock;

void usr_sem_init(){
 for(int i = 0; i < MAX_SEMPAHORES; i++){
   user_sem_list[i].creator= -1;
 }

}
/*
usr_sem_t* usr_sem_open(const char* name, int value){
  int i;

  if(strlen(name) >= MAX_SEMPAHORES){
    //navn for langt. fejl
    kprintf("SEMAPHORE NAME TOO LONG\n");
    return NULL;
  }

  if(value >= 0){
    int index;
    if((index = usr_sem_exists(name)) != -1){
      return &user_sem_list[index]; 
    } 
    else{
      for(i = 0; i < MAX_SEMPAHORES; i++){
        if(user_sem_list[i].creator == -1){
          user_sem_list[i].sem = semaphore_create(value);
          user_sem_list[i].value = value;
          user_sem_list[i].creator = i;
          stringcopy(&user_sem_list[i].name, name, strlen(name)+1);
          return &user_sem_list[i];
        }
      } 
    }
    
  }
  else{
  //negativ val. return navn hvis den findes
    for(i = 0; i < MAX_SEMPAHORES; i++){
      if(stringcmp(name, user_sem_list[i].name)){
        return &user_sem_list[i];
      }
    }
  }
  //ingen er returned så derfor må en fejl være sket
  return NULL;
}*/
usr_sem_t* usr_sem_open(const char* name, int value){
  // CHECK STRING LENGTH???
  int i;

  kprintf("STARTED TO OPEN\n");
  interrupt_status_t intr_status;
  // check om navnet existerer

  if(strlen(name) >= MAX_SEMPAHORES){
    //navn for langt. fejl
    kprintf("SEMAPHORE NAME TOO LONG");
    return NULL;
  }

  if(value >= 0){
    // value er pos eller 0, så lav ny sem med navn.
    // hvis navn findes, returner den med navn
    // lås før vi laver ny semaphore
    kprintf("RAMTE IF\n");

    for(int j = 0; j < MAX_SEMPAHORES; j++){
      if((user_sem_list[j].creator != -1) && (stringcmp(name, user_sem_list[j].name))){
        //name does exists
        kprintf("\nstring compare true ved index %i",j);
        return &user_sem_list[j];
      }
    }
    
    kprintf("EFTER STRINGCOPY");
    //if((index = usr_sem_exists(&name)) != -1){
      //kprintf("IT EXISTS RET %i\n", index);
      //return &user_sem_list[index];
    //}
    intr_status = _interrupt_disable();
    spinlock_acquire(&semaphore_table_slock);
    for(i = 0; i < MAX_SEMPAHORES; i++){
      if(user_sem_list[i].creator == -1){
        kprintf("FØR STRINGCOPY\n");
        user_sem_list[i].creator = i;
        user_sem_list[i].value = value;
        stringcopy(&user_sem_list[i].name, name, strlen(name)+1);
        kprintf("%i, %c", user_sem_list[i].creator, user_sem_list[i].name);
        return &user_sem_list[i];
      }
    } 
    spinlock_release(&semaphore_table_slock);
    _interrupt_set_state(intr_status);
    
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
/*
int usr_sem_exists((const char*) *name){
    for(int i = 0; i < MAX_SEMPAHORES; i++){
      if(stringcmp(*name, user_sem_list[i].name)){
	kprintf("STRINGCOMPARE RET %i\n",i);
        //name does exists
        return i;
      }
    }
    //name does not exists
    kprintf("END OF FORLOOP\n");
    return -1;
}*/



int usr_sem_close(usr_sem_t* sem){
  // not assigned
  if(sem->creator == -1 || sem->value < 0) return -1;
 
  return 0;
}


int usr_sem_p(usr_sem_t* sem){
  //semaphore not assigned
  kprintf("START OF P, val: %i",sem->value);
  if(sem->creator == -1) return -1;
  kprintf("START OF P, val: %i",sem->value);
  interrupt_status_t intr_status;

  intr_status = _interrupt_disable();
  spinlock_acquire(&sem->slock);

  sem->value--;
  if (sem->value < 0) {
    kprintf("FØR SLEEP: i: %i",sem->value);
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
