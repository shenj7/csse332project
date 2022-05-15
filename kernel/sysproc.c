#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "ps.h"

extern struct proc proc[NPROC];

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  struct proc* currproc = myproc();
  int arg;
  if(argint(0, &arg) < 0)
    return -1;
  currproc->trace_mask = arg;
  return 0;
}

uint64
sys_pinfo(void)
{
  uint64 addr;
  if(argaddr(0, &addr) < 0)
    return -1;
  struct proc *kernelproc;
  struct proc *userproc = myproc();
  struct psinfo *processinfo = kalloc();
  int ps_proc_num = 0;
  for(kernelproc = proc; proc < &proc[NPROC]; kernelproc++) {
    if(ps_proc_num > MAX_PS_PROC)
      exit(1);
    acquire(&kernelproc->lock);
    if(kernelproc->state != UNUSED){
      processinfo->active[ps_proc_num] = 1;
      copyout(userproc->pagetable, addr + ps_proc_num*4, (char *)processinfo+ps_proc_num*4, 4);
      processinfo->pid[ps_proc_num] = kernelproc->pid;
      copyout(userproc->pagetable, addr + 256 + ps_proc_num*4, (char *)processinfo+ 256 + ps_proc_num*4, 4);
      processinfo->states[ps_proc_num] = kernelproc->state;
      copyout(userproc->pagetable, addr + 256*2 +  ps_proc_num*4, (char *)processinfo+ 256*2 +  ps_proc_num*4, 4);
      processinfo->num_used_pages[ps_proc_num] = 0;//countmapped(kernelproc->pagetable);
      copyout(userproc->pagetable, addr + 256*3 + ps_proc_num*4, (char *) processinfo+256*3 + ps_proc_num*4, 4);
      strncpy(processinfo->name[ps_proc_num], kernelproc->name, 16);
      copyout(userproc->pagetable, addr+256*4+ps_proc_num*16, (char *)processinfo+256*4+ps_proc_num*16, 16);
    }
    release(&proc->lock);
    ps_proc_num++;
  }
  return -1;
}
