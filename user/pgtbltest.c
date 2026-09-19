#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"      // Defines PGSIZE

char *testname = "pgaccess_test_char_buf";

void pgaccess_test_char();

int
main(int argc, char *argv[])
{
  pgaccess_test_char();
  printf("pgtbltest: char buffer pgaccess test succeeded\n");
  exit(0);
}

void
pgaccess_test_char()
{
  char *mem_buf;
  char abits[8]; // 8 bytes = 64 bits to track up to 64 pages
  int i;

  printf("pgaccess_test_char starting\n");
  
  // 1. Allocate 64 pages of memory
  mem_buf = sbrk(64 * PGSIZE);
  if (mem_buf == (char *)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  // Clear out the tracking buffer
  for (i = 0; i < 8; i++) {
    abits[i] = 0;
  }

  // 2. Access specific pages across the 64-page range
  // - Page 1  -> Bit 1 of byte 0 (abits[0])
  // - Page 2  -> Bit 2 of byte 0 (abits[0])
  // - Page 30 -> Bit 6 of byte 3 (abits[3])  (30 / 8 = 3 remainder 6)
  // - Page 62 -> Bit 6 of byte 7 (abits[7])  (62 / 8 = 7 remainder 6)
  mem_buf[1 * PGSIZE] = 'A';
  mem_buf[2 * PGSIZE] = 'B';
  mem_buf[30 * PGSIZE] = 'C';
  mem_buf[62 * PGSIZE] = 'D';

  // 3. Invoke the system call scanning 64 pages into our char buffer
  if (pgaccess((uint64)mem_buf, 64, abits) < 0) {
    printf("pgaccess failed\n");
    exit(1);
  }

  // 4. Verify the correctness of the byte array mask
  // Expected values:
  // abits[0] = (1 << 1) | (1 << 2) = 2 | 4 = 6 (0x06)
  // abits[3] = (1 << 6) = 64 (0x40)
  // abits[7] = (1 << 6) = 64 (0x40)
  // All other bytes should be 0
  if (abits[0] != 0x06 || abits[3] != 0x40 || abits[7] != 0x40) {
    printf("pgaccess_test failed: incorrect bits in tracking array\n");
    printf("Got: abits[0]=0x%x, abits[3]=0x%x, abits[7]=0x%x\n", abits[0], abits[3], abits[7]);
    exit(1);
  }

  for (i = 0; i < 8; i++) {
    if (i != 0 && i != 3 && i != 7 && abits[i] != 0) {
      printf("pgaccess_test failed: unexpected active bits in index %d\n", i);
      exit(1);
    }
  }

  // 5. Verify that the kernel successfully cleared the access bits after reading them
  for (i = 0; i < 8; i++) abits[i] = 0;

  if (pgaccess((uint64)mem_buf, 64, abits) < 0) {
    printf("pgaccess failed on consecutive check\n");
    exit(1);
  }

  // The second check must return 0 everywhere because no pages were touched
  for (i = 0; i < 8; i++) {
    if (abits[i] != 0) {
      printf("pgaccess_test failed: bits were not cleared on read at index %d\n", i);
      exit(1);
    }
  }


  printf("pgaccess_test_char: OK\n");
}
