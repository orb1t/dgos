
#include "asm_constants.h"
#include "control_regs_constants.h"
#include "../cfi_helpers.h"
#include "interrupts.h"

.code64

.struct 0
	frame_rbp:  .struct frame_rbp+8
    frame_intr: .struct frame_intr+8
    frame_err:  .struct frame_err+8
    frame_rip:  .struct frame_rip+8
    frame_cs:   .struct frame_cs+8
    frame_efl:  .struct frame_efl+8
    frame_rsp:  .struct frame_rsp+8
    frame_ss:   .struct frame_ss+8
    frame_end:

.section .text.isr, "ax"

.macro isr_entry has_code int_num
.global isr_entry_\int_num\()
.hidden isr_entry_\int_num\()
.type isr_entry_\int_num\(),@function
.align 16
isr_entry_\int_num\():
    .cfi_startproc
    .if \has_code == 0
        .cfi_def_cfa_offset 8
        push_cfi $0
    .else
        .cfi_def_cfa_offset 16
    .endif
    push_cfi $\int_num
    jmp isr_common
    .cfi_endproc
.endm

.irp int_num,255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240
     isr_entry 0 \int_num
.endr
.irp int_num,239,238,237,236,235,234,233,232,231,230,229,228,227,226,225,224
     isr_entry 0 \int_num
.endr
.irp int_num,223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208
     isr_entry 0 \int_num
.endr
.irp int_num,207,206,205,204,203,202,201,200,199,198,197,196,195,194,193,192
     isr_entry 0 \int_num
.endr
.irp int_num,191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176
     isr_entry 0 \int_num
.endr
.irp int_num,175,174,173,172,171,170,169,168,167,166,165,164,163,162,161,160
     isr_entry 0 \int_num
.endr
.irp int_num,159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144
     isr_entry 0 \int_num
.endr
.irp int_num,143,142,141,140,139,138,137,136,135,134,133,132,131,130,129,128
     isr_entry 0 \int_num
.endr
.irp int_num,127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112
     isr_entry 0 \int_num
.endr
.irp int_num,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96
     isr_entry 0 \int_num
.endr
.irp int_num,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80
     isr_entry 0 \int_num
.endr
.irp int_num,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64
     isr_entry 0 \int_num
.endr
.irp int_num,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48
     isr_entry 0 \int_num
.endr

// PIC IRQ handlers (16 IRQs)
.irp int_num,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32
     isr_entry 0 \int_num
.endr

// Reserved (unused) exception vectors
.irp int_num,31,30,29,28,27,26,25,24,23,22,21
     isr_entry 0 \int_num
.endr

// Separate exception entry points for conveniently placing breakpoints
isr_entry 0 INTR_EX_VIRTUALIZE
isr_entry 0 INTR_EX_SIMD
isr_entry 0 INTR_EX_MACHINE
isr_entry 1 INTR_EX_ALIGNMENT
isr_entry 0 INTR_EX_MATH
isr_entry 0 15
isr_entry 1 INTR_EX_GPF
isr_entry 1 INTR_EX_STACK
isr_entry 1 INTR_EX_SEGMENT
isr_entry 1 INTR_EX_TSS
isr_entry 0 INTR_EX_COPR_SEG
isr_entry 1 INTR_EX_DBLFAULT
isr_entry 0 INTR_EX_DEV_NOT_AV
isr_entry 0 INTR_EX_OPCODE
isr_entry 0 INTR_EX_BOUND
isr_entry 0 INTR_EX_OVF
isr_entry 0 INTR_EX_BREAKPOINT
isr_entry 0 INTR_EX_NMI
isr_entry 0 INTR_EX_DEBUG
isr_entry 0 INTR_EX_DIV

// Hottest vectors
isr_entry 1 INTR_EX_PAGE
isr_entry 0 INTR_TLB_SHOOTDOWN
isr_entry 0 INTR_THREAD_YIELD
isr_entry 0 INTR_APIC_TIMER

.section .text.isr

.type isr_common,@function
.align 16
isr_common:
    .cfi_startproc
    .cfi_def_cfa_offset 24
    .cfi_rel_offset rsp,16

    // push order: rbp, rbx, r15-r10, rax, r9-8, rcx, rdx, rsi, rdi
    push_cfi %rbp
    .cfi_rel_offset rbp,0
    mov %rsp,%rbp
    .cfi_register rsp,rbp
    .cfi_def_cfa_register rbp
    .cfi_def_cfa_offset 32

    //   7*8(%rbp) -> ss            ┐
    //   6*8(%rbp) -> rsp           │
    //   5*8(%rbp) -> flags         │
    //   4*8(%rbp) -> cs            ├ interrupt frame
    //   3*8(%rbp) -> rip           │
    //   2*8(%rbp) -> error code    │
    //   1*8(%rbp) -> interrupt     ┘
    //   0*8(%rbp) -> caller's rbp  ┐
    //  -1*8(%rbp) -> rbx           │
    //  -2*8(%rbp) -> r15           ├ call preserved
    //  -3*8(%rbp) -> r14           │
    //  -4*8(%rbp) -> r13           │
    //  -5*8(%rbp) -> r12           ┘
    //  -6*8(%rbp) -> r11           ┐
    //  -7*8(%rbp) -> r10           │
    //  -8*8(%rbp) -> rax           │
    //  -9*8(%rbp) -> r9            │
    // -10*8(%rbp) -> r8            ├ call clobbered
    // -11*8(%rbp) -> rcx           │
    // -12*8(%rbp) -> rdx           │
    // -13*8(%rbp) -> rsi           │
    // -14*8(%rbp) -> rdi           ┘

    pushq %rbx
    .cfi_rel_offset rbx,1*8

    pushq %r15
    .cfi_rel_offset r15,2*8

    // r15d=ds, r14d=es, r13d=fs, r12d=gs
    movl %ds,%r15d

    pushq %r14
    .cfi_rel_offset r14,3*8

    movl %es,%r14d

    pushq %r13
    .cfi_rel_offset r13,4*8

    movl %fs,%r13d

    pushq %r12
    .cfi_rel_offset r12,5*8

    movl %gs,%r12d

    pushq %r11
    .cfi_rel_offset r11,6*8

    // shift es, fs, gs into position to pack into r13
    shll $16,%r14d

    pushq %r10
    .cfi_rel_offset r10,7*8

    shlq $32,%r13

    pushq %rax
    .cfi_rel_offset rax,8*8

    shlq $48,%r12

    movq %cr3,%rax

    // Merge ds,es into r15
    orl %r14d,%r15d

    cld
    pushq %r9
    .cfi_rel_offset r9,9*8

    // Merge fs,gs into r13
    orq %r12,%r13

    pushq %r8
    .cfi_rel_offset r8,10*8

    // Merge ds|es,fs|gs into r13
    orq %r15,%r13

    pushq %rcx
    .cfi_rel_offset rcx,11*8

    // Get interrupted code segment
    movzwl frame_cs(%rbp),%ecx

    pushq %rdx
    .cfi_rel_offset rdx,12*8

    pushq %rsi
    .cfi_rel_offset rsi,13*8

    pushq %rdi
    .cfi_rel_offset rdi,14*8

    // Assume null FPU context pointer
    xorl %edi,%edi

    // Save CR3
    pushq %rax

    // Save segments
    pushq %r13

    // Check for doublefault nonsense cs
    testl %ecx,%ecx
    jz .Lfrom_kernel

    // See if we are coming from kernel code
    cmpl $(GDT_SEL_KERNEL_CODE64),%ecx
    je .Lfrom_kernel

    // ...came from user code
    swapgs

.align 16
.Lfrom_kernel:
    // Push FPU context pointer field, initially null
    pushq $0

    // Outgoing cleanup
    pushq $0
    pushq $0

    // Pass intr, ctx
    movq %rsp,%rsi
    movl frame_intr(%rbp),%edi

    // Align stack pointer
    subq $8,%rsp

    // Interrupt dispatch
    // 0x00-0x1F -> exception_isr_handler
    // 0x20-0x2F -> pic_dispatcher
    // 0x30-0xEF -> apic_dispatcher
    // 0xF0-0xFF -> intr_invoke

    // 0x00-0x1F -> exception_isr_handler
    leaq exception_isr_handler(%rip),%rax

    // 0x20-0x2F -> pic_dispatcher
    movq $pic8259_dispatcher,%rcx
    cmpl $INTR_PIC1_IRQ_BASE,%edi
    cmovaeq %rcx,%rax

    // 0x30-0xEF -> apic_dispatcher
    movq $apic_dispatcher,%rcx
    cmpl $INTR_APIC_IRQ_BASE,%edi
    cmovaeq %rcx,%rax

    // 0xF0-0xFF -> intr_invoke
    movq $intr_invoke,%rcx
    cmpl $INTR_SOFT_BASE,%edi
    cmovaeq %rcx,%rax

    // Fast path page fault
    movq $mmu_page_fault_handler,%rcx
    cmpl $INTR_EX_PAGE,%edi
    cmoveq %rcx,%rax

    // Call handler
    call *%rax

    // If context pointer is null, invoke the exception handler for this thread
    test %rax,%rax
    jz .Linvoke_catch

    // isr can return a new stack pointer, or just return
    // the passed one to continue with this thread
    mov %rax,%rsp

    // Infer pointer to incoming thread's interrupt frame from ctx pointer
    lea 19*8(%rax),%rbp

    // Pop outgoing cleanup data
    // Used to adjust outgoing thread state after switching stack
    popq %rax
    popq %rdi
    testq %rax,%rax
    jz .Lno_cleanup_call
    call *%rax
.Lno_cleanup_call:

    // Pop the pointer to the FPU context
    popq %rdi

    // pop packed segments qword into rdx
    // if not returning to kernel
    //   update tss rsp0
    //   swapgs
    //   if any data segment is not GDT_SEL_USER_DATA | 3
    //     load all segment registers with GDT_SEL_USER_DATA | 3
    //   endif
    //   restore fsbase
    // endif

    // Load return cs
    movzwl frame_cs(%rbp),%ecx

    // Pop segments
    popq %rdx

    // See if we're not returning to user code
    // Branch past swapgs and restoration of segments if not
    cmpl $(GDT_SEL_KERNEL_CODE64),%ecx
    je .Lreturning_to_kernel

    // ...returning to user code

    // Fetch pointer to this CPU's TSS for TSS.RSP0 update
    movq %gs:CPU_INFO_TSS_PTR_OFS,%rbx
    leaq frame_end(%rbp),%rax
    movq %rax,TSS_RSP0_OFS(%rbx)

    // Restore user gs
    swapgs

    // If segments are not changing, avoid 136*4 cycles
    cmpq %r13,%rdx
    jnz .Lsegments_changed

.Lsegments_restored:
.Lreturning_to_kernel:
    // Restore CR3
    popq %rax
    movq %rax,%cr3

    .cfi_remember_state

    popq %rdi
    .cfi_restore rdi

    popq %rsi
    .cfi_restore rsi

    popq %rdx
    .cfi_restore rdx

    popq %rcx
    .cfi_restore rcx

    popq %r8
    .cfi_restore r8

    popq %r9
    .cfi_restore r9

    popq %rax
    .cfi_restore rax

    popq %r10
    .cfi_restore r10

    popq %r11
    .cfi_restore r11

    popq %r12
    .cfi_restore r12

    popq %r13
    .cfi_restore r13

    popq %r14
    .cfi_restore r14

    popq %r15
    .cfi_restore r15

    popq %rbx
    .cfi_restore rbx

    popq %rbp
    .cfi_restore rbp
    .cfi_def_cfa_register rsp
    .cfi_def_cfa_offset 24

    add_rsp 16

    iretq

.align 16
.Lsegments_changed:
   .cfi_restore_state
   // Restore segments
   movw %dx,%ds
   shrq $16,%rdx
   movw %dx,%es
   shrq $16,%rdx
   movw %dx,%fs
   shrl $16,%edx
   movw %dx,%gs
   jmp .Lsegments_restored

   .cfi_endproc

// Expects ds loaded with kernel data segment
// Pass context pointer in rdi
// Clobbers rsi,rdx,rax
// Returns rdi=pointer to context
.macro xsave_ctx insn
    .cfi_startproc
    .cfi_def_cfa_register rbp
    .cfi_def_cfa_offset 32

    // Read xsave stack pointer from thread
    movq THREAD_XSAVE_PTR_OFS(%rdi),%rsi
    sub sse_context_size,%rsi

    // Set all bits of edx:eax
    movl $-1,%eax
    movl %eax,%edx

    // Save context using instruction passed to macro
    \insn (%rsi)

    // Update xsave stack pointer in thread
    movq %rsi,THREAD_XSAVE_PTR_OFS(%rdi)
    ret

    .cfi_endproc
.endm

// Expects ds to be kernel data segment
// Expects rdi to point to saved context
// Clobbers eax,edx,ecx
.macro xrstor_ctx insn
    .cfi_startproc
    .cfi_def_cfa_register rbp
    .cfi_def_cfa_offset 32

    movq THREAD_XSAVE_PTR_OFS(%rdi),%rsi

    // Set all bits of edx:eax
    movl $-1,%eax
    movl %eax,%edx

    // Restore context using instruction passed to macro
    \insn (%rdi)

    add sse_context_size,%rsi
    movq %rdi,THREAD_XSAVE_PTR_OFS(%rdi)

    ret

    .cfi_endproc
.endm

// Branch directly to appropriate save/restore

.align 16
.global isr_save_fpu_ctx
.hidden isr_save_fpu_ctx
isr_save_fpu_ctx:
    jmp isr_save_fxsave
.Lsse_context_save_jmp:

.align 16
.global isr_restore_fpu_ctx
.hidden isr_restore_fpu_ctx
isr_restore_fpu_ctx:
    jmp isr_restore_fxrstor
.Lsse_context_restore_jmp:

// Save/restore implementations

.align 16
.global isr_save_xsaveopt
.hidden isr_save_xsaveopt
isr_save_xsaveopt:
    xsave_ctx xsaveopt64

.align 16
.global isr_save_xsave
.hidden isr_save_xsave
isr_save_xsave:
    xsave_ctx xsave64

.align 16
.global isr_save_xsavec
.hidden isr_save_xsavec
isr_save_xsavec:
    xsave_ctx xsavec64

.align 16
.global isr_save_xsaves
.hidden isr_save_xsaves
isr_save_xsaves:
    xsave_ctx xsaves64

.align 16
.global isr_restore_xrstors
.hidden isr_restore_xrstors
isr_restore_xrstors:
    xrstor_ctx xrstors64

.align 16
.global isr_restore_xrstor
.hidden isr_restore_xrstor
isr_restore_xrstor:
    xrstor_ctx xrstor64

.align 16
.global isr_save_fxsave
.hidden isr_save_fxsave
isr_save_fxsave:
    xsave_ctx fxsave64

.align 16
.global isr_restore_fxrstor
.hidden isr_restore_fxrstor
isr_restore_fxrstor:
    xrstor_ctx fxrstor64

// isr_context_t *exception_isr_handler(int intr, isr_context_t *ctx)
.type exception_isr_handler,@function
exception_isr_handler:
    .cfi_startproc

    .cfi_def_cfa_offset 8

    push_cfi %rbp
    push_cfi %rbx
    push_cfi %r12

    movq $intr_has_handler,%rax

    // ebx=intr, r12=ctx
    movl %edi,%ebx
    movq %rsi,%r12

    // If there is no handler...
    callq *%rax
    testl %eax,%eax
    jz 0f

    // There is a handler
    movq $intr_invoke,%rax
    movl %ebx,%edi
    movq %r12,%rsi
    callq *%rax

1:
    movq %r12,%rax
    pop_cfi %r12
    pop_cfi %rbx
    pop_cfi %rbp
    retq

0:
    mov %ebx,%edi
    mov %r12,%rsi
    pop_cfi %r12
    pop_cfi %rbx
    pop_cfi %rbp

// fall through...

.align 16
.Linvoke_catch:
    mov %edi,%ebx
    mov %rsi,%r12

    // ...no handler or handler rejected it
    movq $__exception_handler_invoke,%rax
    movl %ebx,%edi
    callq *%rax
    testl %eax,%eax
    jnz 2f

    movq $cpu_debug_break,%rax
    call *%rax

    // Tail call to unhandled_exception_handler
.align 16
2:
    movq $unhandled_exception_handler,%rax
    movq %r12,%rdi
    jmpq *%rax

    .cfi_endproc

// __noreturn void isr_sysret64(uintptr_t rip, uintptr_t rsp);
.hidden isr_sysret64
.global isr_sysret64
isr_sysret64:
    .cfi_startproc
    .cfi_def_cfa_offset 8

    push_cfi %rbp
    movq %rsp,%rbp

    // Need to prevent context switch while initializing FPU
    cli

    // Initialize MXCSR and FCW
    pushq $(CPU_FPUCW_ELF_INIT)

    movl $(CPU_MXCSR_MASK_ALL),%eax
    andl default_mxcsr_mask,%eax
    pushq %rax

    fldcw 8(%rsp)

    // Clear the opmask registers

    xorl %eax,%eax

    cmpw $0,sse_avx512_opmask_offset(%rip)
    jne .Lclear_opmask

.Ldone_opmask:
    // Check for AVX
    cmpw $0,sse_avx_offset(%rip)
    je .Lno_avx

    // Clear all vector registers
    vldmxcsr (%rsp)
    vzeroall
    jmp .Ldone_sse

.align 16
.Lno_avx:
    ldmxcsr (%rsp)
    pxor %xmm0,%xmm0
    movq %xmm0,%xmm1
    movq %xmm0,%xmm2
    movq %xmm0,%xmm3
    movq %xmm0,%xmm4
    movq %xmm0,%xmm5
    movq %xmm0,%xmm6
    movq %xmm0,%xmm7
    movq %xmm0,%xmm8
    movq %xmm0,%xmm9
    movq %xmm0,%xmm10
    movq %xmm0,%xmm11
    movq %xmm0,%xmm12
    movq %xmm0,%xmm13
    movq %xmm0,%xmm14
    movq %xmm0,%xmm15

.align 16
.Ldone_sse:
    // Clear the x87 stack
    fld1
    fsub %st,%st
    fld %st
    fld %st
    fld %st
    fld %st
    fld %st
    fld %st
    fld %st
    fstp %st
    fstp %st
    fstp %st
    fstp %st
    fstp %st
    fstp %st
    fstp %st
    fstp %st

    cli
    mov %rdi,%rcx
    mov $(CPU_EFLAGS_IF | 2),%r11d

    // Initialize kernel stack pointer for this thread
    movq %gs:CPU_INFO_SELF_OFS,%rbx
    movq CPU_INFO_TSS_PTR_OFS(%rbx),%rdx
    movq CPU_INFO_CURTHREAD_OFS(%rbx),%rbx
    movq THREAD_STACK_OFS(%rbx),%rbx
    movq %rbx,TSS_RSP0_OFS(%rdx)

    // Avoid leaking any register values to user code
    mov %eax,%ebx
    // not ecx
    mov %eax,%edx
    // not esi
    mov %eax,%edi
    mov %eax,%ebp
    mov %eax,%r8d
    mov %eax,%r9d
    mov %eax,%r10d
    // not r11
    mov %eax,%r12d
    mov %eax,%r13d
    mov %eax,%r14d
    mov %eax,%r15d
    swapgs
    mov %rsi,%rsp
    sysretq

.align 16
.Lclear_opmask:
    kmovw %eax,%k1
    kmovw %eax,%k2
    kmovw %eax,%k3
    kmovw %eax,%k4
    kmovw %eax,%k5
    kmovw %eax,%k6
    kmovw %eax,%k7
    jmp .Ldone_opmask

    .cfi_endproc

.section .rodata
.align 8

// Pointers to patch points
.hidden sse_context_save
.global sse_context_save
sse_context_save:
    .quad .Lsse_context_save_jmp

.hidden sse_context_restore
.global sse_context_restore
sse_context_restore:
    .quad .Lsse_context_restore_jmp
