// User-space thread library
//
// Provide thread_create() as an abstraction over sys_thread_create().
// The caller is responsible for allocating and freeing the thread's stack.

#include <inc/lib.h>

// Size of each thread's stack (in bytes). Can be changed if justified.
#define THREAD_STACK_SIZE PGSIZE

// Create a new thread that runs func(arg).
//
// Returns the thread's envid on success, or < 0 on error.
//
// Hints:
//   1. Allocate the thread's stack with sys_page_alloc, in the current
//      process's address space (envid 0).
//   2. Choose where to place it and document the criteria. Careful: the page
//      [USTACKTOP - PGSIZE, USTACKTOP) is already occupied by the main
//      thread's stack (see region_alloc in load_icode), and each thread
//      needs its own range without overlapping the others.
//   3. Set up the stack so that returning from func calls exit(), and so
//      that func receives arg as its argument.
//   4. Call sys_thread_create(func, stack_top) to create the thread.
envid_t
thread_create(void (*func)(void *), void *arg)
{
	// Part 4: Your code here
	panic("thread_create not implemented");
}
