#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/ps.h"
#include "user/user.h"

static const char *ststr(int state)
{
  switch(state) {
  case 0:
    return "UNUSED";
    break;
  case 1:
    return "USED";
    break;
  case 2:
    return "SLEEPING";
    break;
  case 3:
    return "RUNNABLE";
    break;
  case 4:
    return "RUNNING";
    break;
  case 5:
    return "ZOMBIE";
    break;
  default:
    return "UNKNOWN";
  }
}

int
main(int argc, char *argv[])
{
  struct psinfo psinfo;
  int i;

  if(pinfo(&psinfo) < 0) {
    fprintf(2, "%s: pinfo failed...\n", argv[0]);
    exit(1);
  }

  printf("PID \t STATE \t USED PAGES \t NAME\n");
  for(i = 0; i < MAX_PS_PROC; i++) {
    if(psinfo.active[i]) {
      printf("%d \t %s \t %d \t %s\n",
             psinfo.pid[i],
             ststr(psinfo.states[i]), psinfo.num_used_pages[i],
             psinfo.name[i]);
    }
  }

  exit(0);
}
