#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"      // Defines PGSIZE and MAXVA
#include "kernel/memlayout.h"  // Defines USYSCALL

// Helper macro to read the shared layout page safely
#define USYSCALL_PID ((struct usyscall *)USYSCALL)->pid

// Declare standard test tracking variable used by xv6 grading scripts
char *testname = "ugetpid_test";

void ugetpid_test();

int
main(int argc, char *argv[])
{
  ugetpid_test();
  printf("pgtbltest: ugetpid test succeeded\n");
  exit(0);
}

void
ugetpid_test()
{
  int i;

  printf("ugetpid_test starting\n");

  for (i = 0; i < 64; i++) {
    int ret = fork();
    if (ret < 0) {
      printf("fork failed\n");
      exit(1);
    }
    if (ret == 0) {
      // Look up our PID through the optimized userspace data window
      int u_pid = USYSCALL_PID;
      // Look up our PID through the standard heavy system call crossing
      int k_pid = getpid();
      
      if (u_pid != k_pid) {
        printf("ugetpid_test failed: userspace pid %d != kernel pid %d\n", u_pid, k_pid);
        exit(1);
      }
      exit(0);
    }
    wait(0);
  }

  printf("ugetpid_test: OK\n");
}
