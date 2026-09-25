// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct {
  struct spinlock lock;  
  uint8  count[(PHYSTOP)/PGSIZE];
} ref_count;

void
kinit()
{
  initlock(&ref_count.lock, "ref_count");
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void inc_ref(uint64 pa){ // increasing reference
  if(pa < PGROUNDUP((uint64)end) || pa >= PHYSTOP) return;
  acquire(&ref_count.lock);
  (ref_count.count)[(pa)/PGSIZE]++;
  release(&ref_count.lock);
}
void set_ref(uint64 pa){ // initalising from kalloc
  if(pa < PGROUNDUP((uint64)end) || pa >= PHYSTOP) return;
  acquire(&ref_count.lock);
  (ref_count.count)[(pa)/PGSIZE] = 1;
  release(&ref_count.lock);
}
int dec_ref(uint64 pa){ // decreasing reference
  if(pa < PGROUNDUP((uint64)end) || pa >= PHYSTOP) return -1;
  uint64 pa_ref;
  acquire(&ref_count.lock);
  (ref_count.count)[(pa)/PGSIZE]--;
  pa_ref = (ref_count.count)[(pa)/PGSIZE];
  release(&ref_count.lock);
  if(pa_ref == 0) return 1;
  return 0;
}
void clear_ref(uint64 pa){ // reseting to zero -freerange
  if(pa < PGROUNDUP((uint64)end) || pa >= PHYSTOP) return;
  acquire(&ref_count.lock);
  (ref_count.count)[(pa)/PGSIZE] = 0;    
  release(&ref_count.lock);
}
int get_ref(uint64 pa){
  if(pa < PGROUNDUP((uint64)end) || pa >= PHYSTOP) return -1;
  int pa_ref;
  acquire(&ref_count.lock);
  pa_ref = (ref_count.count)[(pa)/PGSIZE];
  release(&ref_count.lock);
  return pa_ref;
}


void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    clear_ref((uint64)p);
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r; 

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");    
 
  if (get_ref((uint64)pa) > 0) {
    if (dec_ref((uint64)pa) == 0) {
      return; 
    }
  }

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  
  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
   
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  
  
  if(r){
   uint64 pa = (uint64)r;
   set_ref(pa);
  }
 
  return (void*)r;
}
