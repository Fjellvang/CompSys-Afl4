#define MAX_NAME_LEN (30)
#define MAX_SEMPAHORES (128)

#include "kernel/spinlock.h"
#include "kernel/semaphore.h"

typedef struct{
  int value;
  spinlock_t slock;
  //redundant???
  int creator;
  //
  const char name[MAX_NAME_LEN]; 
  semaphore_t* sem;

} usr_sem_t;

//return handle identified by name.
// if arg is zero or positivie make fresh sem with Value
// unless semaphore of that name already exists - then return NULL
// If Value is negative. return sema of given NAME
// null if name doesnt exists
usr_sem_t* usr_sem_open(const char* name, int value);

// close sem unless its is blocked by a process.
// return 0 on sucess negative if error
int usr_sem_close(usr_sem_t* sem);

// Procure. Execute P operation on sem. return 0 if nothng went wrong negative on error
int usr_sem_p(usr_sem_t* sem);


//vacate. Execute V operation on.
int usr_sem_v(usr_sem_t* sem);

void usr_sem_init();
int usr_sem_exists(const char* name);
