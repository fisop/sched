#include <inc/assert.h>
#include <inc/x86.h>
#include <kern/spinlock.h>
#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/monitor.h>
#include <kern/trap.h>

void sched_halt(void);

// Wake up environments that were sleeping and whose env_sleep_until <= ticks.
// The global counter 'ticks' is declared in kern/trap.h.
// Call from trap_dispatch after incrementing ticks and before sched_yield.
void
sched_wakeup_sleeping(void)
{
	// Part 2: Your code here - wake up sleeping envs
}

// Choose a user environment to run and run it.
void
sched_yield(void)
{
#ifdef SCHED_ROUND_ROBIN
	// Implement round-robin scheduling with timer preemption.
	//
	// Walk 'envs' circularly looking for an ENV_RUNNABLE environment,
	// starting right after the last environment run on this CPU. Switch
	// to the first one found.
	//
	// If no env is runnable but the environment that was running on this
	// CPU is still ENV_RUNNING, it's fine to pick it again.
	//
	// Never pick an environment that's currently running on another CPU
	// (env_status == ENV_RUNNING). If none is available, fall through
	// to sched_halt.

	// Your code here - Round robin
#endif

#ifdef SCHED_PRIORITIES
	// Implement priority scheduling with anti-starvation aging.
	//
	// Select the ENV_RUNNABLE environment with the highest env_priority.
	// Increment env_wait_ticks for the environments that are not selected.
	// Apply aging: if an environment has been waiting too long, temporarily
	// increase its effective priority to avoid starvation.
	//
	// If the selected environment is the same as curenv and it's still
	// ENV_RUNNING, it's fine to run it again.

	// Your code here - Priorities
#endif

	// TODO: remove after implementing the scheduler
	// Without scheduler, keep runing the last environment while it exists
	if (curenv) {
		env_run(curenv);
	}

	// sched_halt never returns
	sched_halt();
}

// Halt this CPU when there is nothing to do. Wait until the
// timer interrupt wakes it up. This function never returns.
//
void
sched_halt(void)
{
	int i;

	// For debugging and testing purposes, if there are no runnable
	// environments in the system, then drop into the kernel monitor.
	//
	// Part 2: careful, a sleeping process (sys_sleep) is ENV_NOT_RUNNABLE,
	// so it doesn't count as "runnable" in this check. What happens, then,
	// if every process is sleeping at the same time? Review this condition
	// to distinguish "nothing to do right now" (should wait for the timer)
	// from "no process is left alive" (only then should statistics be
	// printed and the kernel halted for good).
	for (i = 0; i < NENV; i++) {
		if ((envs[i].env_status == ENV_RUNNABLE ||
		     envs[i].env_status == ENV_RUNNING ||
		     envs[i].env_status == ENV_DYING))
			break;
	}
	if (i == NENV) {
		cprintf("No runnable environments in the system!\n");

		// Once the scheduler has finishied it's work, print statistics
		// on performance. Your code here

		while (1)
			monitor(NULL);
	}

	// Mark that no environment is running on this CPU
	curenv = NULL;
	lcr3(PADDR(kern_pgdir));

	// Mark that this CPU is in the HALT state, so that when
	// timer interupts come in, we know we should re-acquire the
	// big kernel lock
	xchg(&thiscpu->cpu_status, CPU_HALTED);

	// Release the big kernel lock as if we were "leaving" the kernel
	unlock_kernel();

	// Reset stack pointer, enable interrupts and then halt.
	asm volatile("movl $0, %%ebp\n"
	             "movl %0, %%esp\n"
	             "pushl $0\n"
	             "pushl $0\n"
	             "sti\n"
	             "1:\n"
	             "hlt\n"
	             "jmp 1b\n"
	             :
	             : "a"(thiscpu->cpu_ts.ts_esp0));
}
