#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_clone(void)
{
  uint64 stack_top;
  uint64 start;
  argaddr(0, &start);
  argaddr(1, &stack_top);
  return clone((void (*)())start, (void*)stack_top);
}


uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  // Restore the user trapframe 
  memmove(p->trapframe, p->a_trapframe, PGSIZE);
  kfree(p->a_trapframe);
  p->a_trapframe = 0;
  p->alarm_available = 1;
  return 0;
}

uint64
sys_sigalarm(void)
{
  int n;
  struct proc * p = myproc();
  uint64 handler_addr;
  void (*handler)();

  argint(0, &n);
  argaddr(1, &handler_addr);

  handler = (void (*)())handler_addr;

  p->alarm_on = 1;
  p->alarm_interval = n;
  p->alarm_handler = handler;

  return 0;
}

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
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

  argint(0, &pid);
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
