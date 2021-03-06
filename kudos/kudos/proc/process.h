/// Kernel support for userland processes.

#ifndef KUDOS_PROC_PROCESS_H
#define KUDOS_PROC_PROCESS_H

#include "lib/types.h"
#include "vm/memory.h"

#define PROCESS_PTABLE_FULL  (-1)
#define PROCESS_ILLEGAL_JOIN (-2)

#define PROCESS_MAX_FILELENGTH (256)
#define PROCESS_MAX_PROCESSES  (128)
#define PROCESS_MAX_FILES      (10)

typedef int pid_t;

typedef enum {
  PROCESS_SLEEPING,
  PROCESS_READY,
  PROCESS_WORKING,
  PROCESS_DONE
} process_state_t;

typedef struct {
  pid_t pid;
  char path[256];
  process_state_t state;
  int retval;
} pcb_t;

/// Initialize process table.
/// Should be called during boot.
void process_init();

int process_read(int filehandle, void *buffer, int length);
int process_write(int filehandle, const void *buffer, int length);

/// Load and run the executable as a new process in a new thread.
/// Arguments: Path to the executable and arguments.
/// Flags are documented below.
/// Returns the process ID of the new process.
pid_t process_spawn(char const* executable, int flags);

void process_exit(int retval);

int process_join(pid_t pid);
#endif // KUDOS_PROC_PROCESS_H
