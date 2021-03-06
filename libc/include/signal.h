#pragma once

#include <time.h>
#include <stdlib.h>
#include <sys/types.h>

// The <signal.h> header shall define the following macros,
// which shall expand to constant expressions with distinct values
// that have a type compatible with the second argument to, and
// the return value of, the signal() function, and whose values
// shall compare unequal to the address of any declarable function.

// Request for default signal handling.
#define SIG_DFL

// Return value from signal() in case of error.
#define SIG_ERR

//  Request that signal be held.
#define SIG_HOLD

// Request that signal be ignored.
#define SIG_IGN

// The <signal.h> header shall define the
// pthread_t, size_t, and uid_t types as described in <sys/types.h>.

// The <signal.h> header shall define the timespec structure as
// described in <time.h>.

// The <signal.h> header shall define the following data types:

#ifndef __SIG_ATOMIC_TYPE__
#define __SIG_ATOMIC_TYPE__ long
#endif

// Possibly volatile-qualified integer type of an object that can be
// accessed as an atomic entity, even in the presence of
// asynchronous interrupts.
typedef __SIG_ATOMIC_TYPE__ sig_atomic_t;

// Integer or structure type of an object used to represent sets of signals.
typedef long sigset_t;

// The <signal.h> header shall define the pthread_attr_t type as
// described in <sys/types.h>.
struct __pthread_attr_t {
    // ...
};

union sigval {
    int    sival_int;
    void  *sival_ptr;
};

// The <signal.h> header shall define the sigevent structure,
// which shall include at least the following members:

typedef struct sigevent {
    // Notification type.
    int sigev_notify;
    // Signal number.
    int sigev_signo;
    // Signal value.
    union sigval sigev_value;
    // Notification function.
    void (*sigev_notify_function)(union sigval);
} sigevent;

// pthread_attr_t *sigev_notify_attributes  Notification attributes.

// The <signal.h> header shall define the following symbolic constants
//  for the values of sigev_notify:

// No asynchronous notification is delivered when the event of interest occurs.
#define SIGEV_NONE
// A queued signal, with an application-defined value, is generated when
// the event of interest occurs.
#define SIGEV_SIGNAL
// A notification function is called to perform notification.
#define SIGEV_THREAD

// The <signal.h> header shall declare the SIGRTMIN and SIGRTMAX macros,
// which shall expand to positive integer expressions with type int, but which
// need not be constant expressions. These macros specify a range of
// signal numbers that are reserved for application use and for which the
// realtime signal behavior specified in this volume of POSIX.1-2008 is
// supported. The signal numbers in this range do not overlap any of the
// signals specified in the following table.

// The range SIGRTMIN through SIGRTMAX inclusive shall include
// at least {RTSIG_MAX} signal numbers.

// It is implementation-defined whether realtime signal behavior is
// supported for other signals.

// The <signal.h> header shall define the following macros that are used
// to refer to the signals that occur in the system. Signals defined here
// begin with the letters SIG followed by an uppercase letter. The macros
// shall expand to positive integer constant expressions with type int and
// distinct values. The value 0 is reserved for use as the null signal
// (see kill()). Additional implementation-defined signals may occur
// in the system.

// The ISO C standard only requires the signal names
// SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, and SIGTERM
// to be defined. An implementation need not generate any of these six
// signals, except as a result of explicit use of interfaces that generate
// signals, such as raise(), kill(), the General Terminal Interface
// (see Special Characters), and the kill utility, unless otherwise
// stated (see, for example, XSH Memory Protection).

// The following signals shall be supported on all implementations
// (default actions are explained below the table):

// Process abort signal.
#define SIGABRT

// Alarm clock.
#define SIGALRM

// Access to an undefined portion of a memory object.
#define SIGBUS

// Child process terminated, stopped, or continued.
#define SIGCHLD

// Continue executing, if stopped.
#define SIGCONT

// Erroneous arithmetic operation.
#define SIGFPE

// Hangup.
#define SIGHUP

// Illegal instruction.
#define SIGILL

// Terminal interrupt signal.
#define SIGINT

// Kill (cannot be caught or ignored).
#define SIGKILL

// Write on a pipe with no one to read it.
#define SIGPIPE

// Terminal quit signal.
#define SIGQUIT

// Invalid memory reference.
#define SIGSEGV

// Stop executing (cannot be caught or ignored).
#define SIGSTOP

// Termination signal.
#define SIGTERM

// Terminal stop signal.
#define SIGTSTP

// Background process attempting read.
#define SIGTTIN

// Background process attempting write.
#define SIGTTOU

// User-defined signal 1.
#define SIGUSR1

// User-defined signal 2.
#define SIGUSR2

// Pollable event.
#define SIGPOLL

// Profiling timer expired.
#define SIGPROF

// Bad system call.
#define SIGSYS

// Trace/breakpoint trap.
#define SIGTRAP

// High bandwidth data is available at a socket.
#define SIGURG

// Virtual timer expired.
#define SIGVTALRM

// CPU time limit exceeded.
#define SIGXCPU

// File size limit exceeded.
#define SIGXFSZ

// The <signal.h> header shall declare the sigaction structure,
// which shall include at least the following members:

typedef struct __siginfo_t siginfo_t;
typedef struct __stack_t stack_t;
typedef struct __ucontext_t ucontext_t;

struct sigaction {
    // Pointer to a signal-catching function or one of the
    // SIG_IGN or SIG_DFL.
    void (*sa_handler)(int);

    // Set of signals to be blocked during execution
    // of the signal handling function.
    sigset_t sa_mask;

    // Special flags.
    int sa_flags;

    // Pointer to a signal-catching function.
    void (*sa_sigaction)(int, siginfo_t *, void *);
};

// The storage occupied by sa_handler and sa_sigaction may overlap, and
// a conforming application shall not use both simultaneously.

// The <signal.h> header shall define the following macros which shall
// expand to integer constant expressions that need not be usable in
// #if preprocessing directives:

// The resulting set is the union of the current set and the signal set
// pointed to by the argument set.
#define SIG_BLOCK

// The resulting set is the intersection of the current set and the
// complement of the signal set pointed to by the argument set.
#define SIG_UNBLOCK

// The resulting set is the signal set pointed to by the argument set.
#define SIG_SETMASK

// The <signal.h> header shall also define the following symbolic constants:

// Do not generate SIGCHLD when children stop or stopped children continue.
#define SA_NOCLDSTOP

// Causes signal delivery to occur on an alternate stack.
#define SA_ONSTACK

// Causes signal dispositions to be set to SIG_DFL on entry to signal handlers.
#define SA_RESETHAND

// Causes certain functions to become restartable.
#define SA_RESTART

// Causes extra information to be passed to signal handlers at the time
// of receipt of a signal.
#define SA_SIGINFO

// Causes implementations not to create zombie processes or status
// information on child termination. See sigaction.
#define SA_NOCLDWAIT

// Causes signal not to be automatically blocked on entry to signal handler.
#define SA_NODEFER

// Process is executing on an alternate signal stack.
#define SS_ONSTACK

// Alternate signal stack is disabled.
#define SS_DISABLE

// Minimum stack size for a signal handler.
#define MINSIGSTKSZ

// Default size in bytes for the alternate signal stack.
#define SIGSTKSZ

// The <signal.h> header shall define the mcontext_t type through typedef.

typedef struct mcontext_t {
} mcontext_t;

// The <signal.h> header shall define the stack_t type as a structure,
// which shall include at least the following members:

struct __stack_t {
    // Stack base or pointer.
    void     *ss_sp;

    // Stack size.
    size_t    ss_size;
    // Flags.
    int       ss_flags;
};

// The <signal.h> header shall define the ucontext_t type as a structure
// that shall include at least the following members:

struct __ucontext_t {
    // Pointer to the context that is resumed when this context returns.
    ucontext_t *uc_link;
    // The set of signals that are blocked when this context is active.
    sigset_t uc_sigmask;
    // The stack used by this context.
    stack_t uc_stack;
    // A machine-specific representation of the saved context.
    mcontext_t uc_mcontext;
};

// The <signal.h> header shall define the siginfo_t type as a structure,
// which shall include at least the following members:

struct __siginfo_t {
    // Signal number.
    int           si_signo;
    // Signal code.
    int           si_code;


    // If non-zero, an errno value associated with this signal, as
    // described in <errno.h>.
    int           si_errno;


    // Sending process ID.
    pid_t         si_pid;
    // Real user ID of sending process.
    uid_t         si_uid;
    // Address of faulting instruction.
    void         *si_addr;
    // Exit value or signal.
    int           si_status;


    // Band event for SIGPOLL.
    long          si_band;


    // Signal value.
    union sigval  si_value;
};

// The <signal.h> header shall define the symbolic constants in the
// Code column of the following table for use as values of si_code that
// are signal-specific or non-signal-specific reasons why the
// signal was generated.

// Illegal opcode.
// SIGILL
#define ILL_ILLOPC

// Illegal operand.
#define ILL_ILLOPN

// Illegal addressing mode.
#define ILL_ILLADR

// Illegal trap.
#define ILL_ILLTRP

// Privileged opcode.
#define ILL_PRVOPC

// Privileged register.
#define ILL_PRVREG

// Coprocessor error.
#define ILL_COPROC

// Internal stack error.
#define ILL_BADSTK

// Integer divide by zero.
// SIGFPE
#define FPE_INTDIV

// Integer overflow.
#define FPE_INTOVF

// Floating-point divide by zero.
#define FPE_FLTDIV

// Floating-point overflow.
#define FPE_FLTOVF

// Floating-point underflow.
#define FPE_FLTUND

// Floating-point inexact result.
#define FPE_FLTRES

// Invalid floating-point operation.
#define FPE_FLTINV

// Subscript out of range.
#define FPE_FLTSUB


// Address not mapped to object.
// SIGSEGV
#define SEGV_MAPERR

// Invalid permissions for mapped object.
#define SEGV_ACCERR

// Invalid address alignment.
// SIGBUS
#define BUS_ADRALN

// Nonexistent physical address.
#define BUS_ADRERR

// Object-specific hardware error.
#define BUS_OBJERR

// Process breakpoint.
// SIGTRAP
#define TRAP_BRKPT

// Process trace trap.
#define TRAP_TRACE

// Child has exited.
// SIGCHLD
#define CLD_EXITED

// Child has terminated abnormally and did not create a core file.
#define CLD_KILLED

// Child has terminated abnormally and created a core file.
#define CLD_DUMPED

// Traced child has trapped.
#define CLD_TRAPPED

// Child has stopped.
#define CLD_STOPPED

// Stopped child has continued.
#define CLD_CONTINUED

// Data input available.
// SIGPOLL
#define POLL_IN

// Output buffers available.
#define POLL_OUT

// Input message available.
#define POLL_MSG

// I/O error.
#define POLL_ERR

// High priority input available.
#define POLL_PRI

// Device disconnected.
#define POLL_HUP

// Signal sent by kill().
#define SI_USER

// Signal sent by sigqueue().
#define SI_QUEUE

// Signal generated by expiration of a timer set by timer_settime().
#define SI_TIMER

// Signal generated by completion of an asynchronous I/O request.
#define SI_ASYNCIO

// Signal generated by arrival of a message on an empty message queue.
#define SI_MESGQ

// Implementations may support additional si_code values not included in
// this list, may generate values included in this list under circumstances
// other than those described in this list, and may contain extensions
// or limitations that prevent some values from being generated.
// Implementations do not generate a different value from the ones
// described in this list for circumstances described in this list.
// In addition, the following signal-specific information shall be available:

// Signal
//
// Member
//
// Value
//
// SIGILL
// SIGFPE
//
// Address of faulting instruction.
// void * si_addr
//
//
// SIGSEGV
// SIGBUS
//
// Address of faulting memory reference.
// void * si_addr
//
//
// SIGCHLD
//
// Child process ID.
// pid_t si_pid
//
//
//
//
// If si_code is equal to CLD_EXITED, then si_status holds the exit value of
// the process; otherwise, it is equal to the signal that caused the process
// to change state. The exit value in si_status shall be equal to the full
// exit value (that is, the value passed to _exit(), _Exit(), or exit(),
// or returned from main()); it shall not be limited to the least
// significant eight bits of the value.
// int si_status
//
//
//
//
// uid_t si_uid
//
// Real user ID of the process that sent the signal.
//
// SIGPOLL
//
// long si_band

// Band event for POLL_IN, POLL_OUT, or POLL_MSG.

// For some implementations, the value of si_addr may be inaccurate.

// The following shall be declared as functions and may also be defined
// as macros. Function prototypes shall be provided.


int kill(pid_t, int);


int killpg(pid_t, int);


void psiginfo(const siginfo_t *, const char *);
void psignal(int, const char *);
int pthread_kill(pthread_t, int);
int pthread_sigmask(int, const sigset_t *restrict,
           sigset_t *restrict);

int raise(int);

int sigaction(int, const struct sigaction *restrict,
           struct sigaction *restrict);
int sigaddset(sigset_t *, int);


int sigaltstack(const stack_t *restrict, stack_t *restrict);


int sigdelset(sigset_t *, int);
int sigemptyset(sigset_t *);
int sigfillset(sigset_t *);


int sighold(int);
int sigignore(int);
int siginterrupt(int, int);


int sigismember(const sigset_t *, int);

void (*signal(int, void (*)(int)))(int);

int sigpause(int);


int sigpending(sigset_t *);
int sigprocmask(int, const sigset_t *restrict, sigset_t *restrict);
int sigqueue(pid_t, int, union sigval);


int sigrelse(int);
void (*sigset(int, void (*)(int)))(int);


int sigsuspend(const sigset_t *);
int sigtimedwait(const sigset_t *restrict, siginfo_t *restrict,
           const struct timespec *restrict);
int sigwait(const sigset_t *restrict, int *restrict);
int sigwaitinfo(const sigset_t *restrict, siginfo_t *restrict);
