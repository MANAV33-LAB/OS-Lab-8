#include "types.h"
#include "stat.h"
#include "user.h"

#define MB (1024 * 1024)
#define PG 4096

int main(int argc, char *argv[])
{
  int before, after_sbrk, after_touch, after_shrink, i, pid, n;
  int fd[2];
  char *base, *buf, *bad;

  printf(1, "=== Q4: Lazy Memory Allocation ===\n");

  before = getmeminfo();
  printf(1, "Free frames before sbrk           : %d\n", before);

  base = sbrk(32 * MB);
  if(base == (char*)-1){
    printf(1, "sbrk failed\n");
    exit();
  }
  after_sbrk = getmeminfo();
  printf(1, "Free frames after sbrk(32MB)      : %d (used %d)\n", after_sbrk, before - after_sbrk);

  for(i = 0; i < 4; i++)
    base[i * 8 * MB + 100] = 'A' + i;
  after_touch = getmeminfo();
  printf(1, "Free frames after touching 4 pages: %d (used %d, includes page-table frames)\n",
         after_touch, after_sbrk - after_touch);
  for(i = 0; i < 4; i++)
    printf(1, "base[%d MB + 100] = %c\n", i * 8, base[i * 8 * MB + 100]);
  printf(1, "Untouched page reads as zero: %d\n", base[5 * MB]);

  printf(1, "\n[Test: fork with lazy pages]\n");
  pid = fork();
  if(pid == 0){
    printf(1, "child: copied value base[100] = %c\n", base[100]);
    base[2 * MB] = 7;
    printf(1, "child: touched a new page, value %d. Exiting.\n", base[2 * MB]);
    exit();
  }
  wait();
  printf(1, "parent: child exited, base[100] still %c\n", base[100]);

  printf(1, "\n[Test: system calls with untouched buffers]\n");
  pipe(fd);
  write(fd[1], "hello", 5);
  buf = base + 30 * MB + 500;
  n = read(fd[0], buf, 5);
  buf[5] = 0;
  printf(1, "read() into untouched buffer returned %d: %s\n", n, buf);
  n = write(fd[1], base + 20 * MB, 3);
  printf(1, "write() from untouched buffer returned %d\n", n);
  close(fd[0]);
  close(fd[1]);

  printf(1, "\n[Test: invalid pointer given to a system call]\n");
  bad = sbrk(0) + 10 * PG;
  n = read(0, bad, 5);
  printf(1, "read() with address beyond sz returned %d (kernel did not crash)\n", n);

  printf(1, "\n[Test: access at address >= sz]\n");
  pid = fork();
  if(pid == 0){
    bad = sbrk(0) + 10 * PG;
    *bad = 1;
    printf(1, "ERROR: child should have been killed\n");
    exit();
  }
  wait();
  printf(1, "parent: child was killed, parent still alive\n");

  printf(1, "\n[Test: access to the stack guard page]\n");
  pid = fork();
  if(pid == 0){
    int x;
    bad = (char*)(((uint)&x & ~(PG - 1)) - PG);
    *bad = 1;
    printf(1, "ERROR: child should have been killed\n");
    exit();
  }
  wait();
  printf(1, "parent: child was killed, parent still alive\n");

  sbrk(-32 * MB);
  after_shrink = getmeminfo();
  printf(1, "\nFree frames after sbrk(-32MB)     : %d (freed %d touched pages)\n",
         after_shrink, after_shrink - after_touch);

  printf(1, "Lazy allocation test complete.\n");
  exit();
}
