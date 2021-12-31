#define MAX_PS_PROC 64
struct psinfo {
  int active[MAX_PS_PROC];            // active/inactive process
  int pid[MAX_PS_PROC];               // the pid of each active process
  int states[MAX_PS_PROC];            // the state of each active process
  int num_used_pages[MAX_PS_PROC];    // the number of used bytes per process
  char name[MAX_PS_PROC][16];         // the name of each process if any
};
