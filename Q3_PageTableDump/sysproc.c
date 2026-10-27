#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

extern pte_t* walkpgdir(pde_t*, const void*, int);
int sys_va2pa(void) {
  int va;
  if(argint(0, &va) < 0) return -1;
  
  pte_t *pte = walkpgdir(myproc()->pgdir, (void*)va, 0);
  
  if(!pte || !(*pte & 0x001) || !(*pte & 0x004)) // Not present or not user-accessible
    return -1;
    
  int pdx = (va >> 22) & 0x3FF;
  int ptx = (va >> 12) & 0x3FF;
  int offset = va & 0xFFF;
  
  int p = (*pte & 0x001) ? 1 : 0;
  int w = (*pte & 0x002) ? 1 : 0;
  int u = (*pte & 0x004) ? 1 : 0;
  
  cprintf("va2pa: PDX=%d, PTX=%d, OFFSET=%d, FLAGS: P=%d W=%d U=%d\n", pdx, ptx, offset, p, w, u);
  
  return (*pte & ~0xFFF) | offset;
}

int sys_getmeminfo(void) {
  struct meminfo *mi;
  if(argptr(0, (void*)&mi, sizeof(*mi)) < 0) return -1;
  
  mi->total_pages = get_total_pages();
  mi->free_pages = get_free_pages();
  mi->user_pages = myproc()->sz / 4096;
  
  int pt_pages = 1; // 1 for the directory itself
  uint *pgdir = (uint*)myproc()->pgdir;
  for(int i = 0; i < 1024; i++) {
    if(pgdir[i] & 1) pt_pages++; // Count valid PDEs (PTE_P)
  }
  mi->pt_pages = pt_pages;
  
  return 0;
}

int sys_vmprint(void) {
  vmprint(myproc()->pgdir);
  return 0;
}
