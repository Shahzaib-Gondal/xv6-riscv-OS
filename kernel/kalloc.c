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
void S_freerange(void *pa_start, void *pa_end);
void s_free(void *pa);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;
//supermem struct
struct {
  struct spinlock lock;
  struct run *freelist;
} smem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&smem.lock, "supermemory");

  uint64 memstart = SUPERPGROUNDUP((uint64)end);
  uint64 memend = PHYSTOP;

  // Divide memory roughly in half
  uint64 s_start = memstart;
  uint64 s_end = memstart + (memend - memstart)/2;
  uint64 k_start = PGROUNDUP(s_end);
  uint64 k_end   = memend;

  S_freerange((void*)s_start, (void*)s_end);
  freerange((void*)k_start, (void*)k_end);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}
void
S_freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)SUPERPGROUNDUP((uint64)pa_start);
  for(; p + S_PGSIZE <= (char*)pa_end; p += S_PGSIZE)
    s_free(p);
}


// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

void
s_free(void *pa)
{
  struct run *r;

  if(((uint64)pa % S_PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("S_free");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&smem.lock);
  r->next = smem.freelist;
  smem.freelist = r;
  release(&smem.lock);
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
  return (void*)r;
}
void *
salloc(void)
{
  struct run *r;
  printf("Super Page Has Been Allocated \n");


  acquire(&smem.lock);
  r = smem.freelist;
  if(r)
    smem.freelist = r->next;
  release(&smem.lock);

  if(r)
    memset((char*)r, 5, S_PGSIZE); // fill with junk
  return (void*)r;
}

