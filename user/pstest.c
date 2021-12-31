#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/riscv.h"
#include "kernel/ps.h"
#include "user/user.h"

static const char *stnames[] = {
  "UNUSED",
  "USED",
  "SLEEPING",
  "RUNNABLE",
  "RUNNING",
  "ZOMBIE",
  "UNKNOWN",
};

static inline void
callpinfo(struct psinfo *psinfo)
{
  if(pinfo(psinfo) < 0) {
    fprintf(2, "callpinfo: pinfo failed...\n");
    exit(1);
  }
}

static int
findpid(struct psinfo *psinfo, int pid)
{
  int i;

  for(i = 0; i < MAX_PS_PROC; i++) {
    if(psinfo->active[i] && psinfo->pid[i] == pid)
      return i;
  }

  fprintf(2, "Process with pid = %d not found...\n", pid);
  exit(1);
}

void
testnames(const char *name)
{
  int i;
  struct psinfo psinfo;

  callpinfo(&psinfo);
  i = findpid(&psinfo, 1);
  if(strcmp(psinfo.name[i], "init")) {
    fprintf(2, "Process with pid = 1 is not named 'init'");
    exit(1);
  }

  i = findpid(&psinfo, 2);
  if(strcmp(psinfo.name[i], "sh")) {
    fprintf(2, "Process with pid = 2 is not named 'sh'");
    exit(1);
  }

  i = findpid(&psinfo, getpid());
  if(strcmp(psinfo.name[i], name)) {
    fprintf(2, "Process with pid = %s is not named '%s'", getpid(), name);
    exit(1);
  }
}

void
teststates(void)
{
  int i, state;
  struct psinfo psinfo;

  callpinfo(&psinfo);

  /* check pid = 1 */
  i = findpid(&psinfo, 1);
  state = psinfo.states[i];
  if(strcmp(stnames[state], "SLEEPING")) {
    fprintf(2, "Process with pid = 1 is not sleeping\n");
    exit(1);
  }

  /* check pid = 2 */
  i = findpid(&psinfo, 2);
  state = psinfo.states[i];
  if(strcmp(stnames[state], "SLEEPING")) {
    fprintf(2, "Process with pid = 2 is not sleeping\n");
    exit(1);
  }

  /* check pid = getpid */
  i = findpid(&psinfo, getpid());
  state = psinfo.states[i];
  if(strcmp(stnames[state], "RUNNING")) {
    fprintf(2, "Process with pid = %d is not running\n", getpid());
    exit(1);
  }
}

void
testcount(void)
{
  int i;
  int count = 0;
  const int numforks = 10;
  struct psinfo psinfo;

  for(i = 0; i < numforks; i++) {
    if(fork() == 0) {
      sleep(5);
      exit(0);
    }
  }

  callpinfo(&psinfo);
  for(i = 0; i < MAX_PS_PROC; i++) {
    if(psinfo.active[i])
      count++;
  }

  for(i = 0; i < numforks; i++) {
    wait(0);
  }

  if(count != 13) {
    fprintf(2, "testcount FAILED: expected (13), actual (%d)\n", count);
    exit(1);
  }
}

void
testmem(void)
{
  const int num_pages = 10;
  uint64 sz = (uint64)sbrk(0);
  int i, j, npages, n = sz;
  struct psinfo psinfo;

  callpinfo(&psinfo);
  i = findpid(&psinfo, getpid());
  npages = psinfo.num_used_pages[i];

  for(j = 0; j < num_pages; j++) {
    if((uint64)sbrk(PGSIZE) == 0xffffffffffffffff){
      fprintf(2, "pstest: sbrk failed\n");
      exit(1);
    }
    n += PGSIZE;
  }

  callpinfo(&psinfo);
  i = findpid(&psinfo, getpid());
  if(psinfo.num_used_pages[i] - npages != 10) {
    fprintf(2, "FAILED: after alloc, expected %d pages, got %d pages\n",
            npages + 10, psinfo.num_used_pages[i]);
    exit(1);
  }

  sbrk(-(n - sz));
  callpinfo(&psinfo);
  i = findpid(&psinfo, getpid());
  if(psinfo.num_used_pages[i] != npages) {
    fprintf(2, "FAILED: after free, expected %d pages, got %d pages\n",
            npages, psinfo.num_used_pages[i]);
    exit(1);
  }
}

void
testexec(void)
{
  int pid;
  char *nargv[MAXARG];

  pid = fork();
  if(pid == 0) {
    nargv[0] = "ps";
    nargv[1] = 0;
    exec(nargv[0], nargv);
    exit(0);
  } else {
    wait(0);
  }
}

int
main(int argc, char *argv[])
{
  printf("%s: starting...\n", argv[0]);
  printf("%s: testname(): ", argv[0]);
  testnames(argv[0]);
  printf(" OK.\n");
  printf("%s: teststates(): ", argv[0]);
  teststates();
  printf("OK.\n");
  printf("%s: testcount(): ", argv[0]);
  testcount();
  printf("OK.\n");
  printf("%s: testmem(): ", argv[0]);
  testmem();
  printf("OK.\n");
  printf("%s: testexec(): \n", argv[0]);
  testexec();

  printf("%s: OK...\n", argv[0]);

  exit(0);
}
