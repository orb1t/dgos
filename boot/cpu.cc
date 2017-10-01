#include "cpu.h"
#include "paging.h"
#include "bootsect.h"
#include "exception.h"

#define GDT_ACCESS_PRESENT_BIT   7
#define GDT_ACCESS_DPL_BIT       5
#define GDT_ACCESS_EXEC_BIT      3
#define GDT_ACCESS_DOWN_BIT      2
#define GDT_ACCESS_RW_BIT        1

#define GDT_ACCESS_PRESENT      (1 << GDT_ACCESS_PRESENT_BIT)
#define GDT_ACCESS_EXEC         (1 << GDT_ACCESS_EXEC_BIT)
#define GDT_ACCESS_DOWN         (1 << GDT_ACCESS_DOWN_BIT)
#define GDT_ACCESS_RW           (1 << GDT_ACCESS_RW_BIT)

#define GDT_ACCESS_DPL_BITS     2
#define GDT_ACCESS_DPL_MASK     ((1 << GDT_ACCESS_DPL_BITS)-1)
#define GDT_ACCESS_DPL          (GDT_ACCESS_DPL_MASK << GDT_ACCESS_DPL_BIT)
#define GDT_ACCESS_DPL_n(dpl)   ((dpl) << GDT_ACCESS_DPL_BIT)

#define GDT_FLAGS_GRAN_BIT      7
#define GDT_FLAGS_IS32_BIT      6
#define GDT_FLAGS_IS64_BIT      5

#define GDT_FLAGS_GRAN          (1 << GDT_FLAGS_GRAN_BIT)
#define GDT_FLAGS_IS32          (1 << GDT_FLAGS_IS32_BIT)
#define GDT_FLAGS_IS64          (1 << GDT_FLAGS_IS64_BIT)

#define GDT_LIMIT_LOW_MASK      0xFFFF
#define GDT_BASE_LOW_MASK       0xFFFF

#define GDT_BASE_MIDDLE_BIT     16
#define GDT_BASE_MIDDLE         0xFF

#define GDT_LIMIT_HIGH_BIT      16
#define GDT_LIMIT_HIGH          0x0F

#define GDT_BASE_HIGH_BIT       24
#define GDT_BASE_HIGH           0xFF

#define GDT_MAKE_SEGMENT_DESCRIPTOR(base, \
            limit, \
            present, \
            privilege, \
            executable, \
            downward, \
            rw, \
            granularity, \
            is32, \
            is64) \
{ \
    ((limit) & GDT_LIMIT_LOW_MASK), \
    ((base) & GDT_BASE_LOW_MASK), \
    (((base) >> GDT_BASE_MIDDLE_BIT) & GDT_BASE_MIDDLE), \
    ( \
        ((present) ? GDT_ACCESS_PRESENT : 0) | \
        GDT_ACCESS_DPL_n(privilege) | \
        (1 << 4) | \
        ((executable) ? GDT_ACCESS_EXEC : 0) | \
        ((downward) ? GDT_ACCESS_DOWN : 0) | \
        ((rw) ? GDT_ACCESS_RW : 0) \
    ), \
    ( \
        ((granularity) ? GDT_FLAGS_GRAN : 0) | \
        ((is32) ? GDT_FLAGS_IS32 : 0) | \
        ((is64) ? GDT_FLAGS_IS64 : 0) | \
        (((limit) >> GDT_LIMIT_HIGH_BIT) & GDT_LIMIT_HIGH) \
    ), \
    (((base) >> GDT_BASE_HIGH_BIT) & GDT_BASE_HIGH) \
}

//
// 32-bit selectors

#define GDT_MAKE_EMPTY() \
    GDT_MAKE_SEGMENT_DESCRIPTOR(0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

// Native code (32 bit)
#define GDT_MAKE_CODESEG32(ring) \
    GDT_MAKE_SEGMENT_DESCRIPTOR(0, 0xFFFFF, 1, ring, 1, 0, 1, 1, 1, 0)

// Native data (32 bit)
#define GDT_MAKE_DATASEG32(ring) \
    GDT_MAKE_SEGMENT_DESCRIPTOR(0, 0xFFFFF, 1, ring, 0, 0, 1, 1, 1, 0)

// Foregn code (16 bit)
#define GDT_MAKE_CODESEG16(ring) \
    GDT_MAKE_SEGMENT_DESCRIPTOR(0, 0x0FFFF, 1, ring, 1, 0, 1, 0, 0, 0)

// Foreign data (16 bit)
#define GDT_MAKE_DATASEG16(ring) \
    GDT_MAKE_SEGMENT_DESCRIPTOR(0, 0x0FFFF, 1, ring, 0, 0, 1, 0, 0, 0)

//
// 64-bit selectors

// Native code (64 bit)
#define GDT_MAKE_CODESEG64(ring) \
    GDT_MAKE_SEGMENT_DESCRIPTOR(0, 0xFFFFF, 1, ring, 1, 0, 1, 1, 0, 1)

// Native data (64 bit)
// 3.4.5 "When not in IA-32e mode or for non-code segments, bit 21 is
// reserved and should always be set to 0"
#define GDT_MAKE_DATASEG64(ring) \
    GDT_MAKE_SEGMENT_DESCRIPTOR(0, 0xFFFFF, 1, ring, 0, 0, 1, 1, 0, 0)

static gdt_entry_t gdt[] = {
    GDT_MAKE_EMPTY(),

    // 64 bit kernel code and data
    GDT_MAKE_CODESEG64(0),
    GDT_MAKE_DATASEG64(0),

    // 32 bit kernel code and data
    GDT_MAKE_CODESEG32(0),
    GDT_MAKE_DATASEG32(0),

    // 16 bit kernel code and data
    GDT_MAKE_CODESEG16(0),
    GDT_MAKE_DATASEG16(0),

    // 64 bit user code and data
    GDT_MAKE_CODESEG64(3),
    GDT_MAKE_DATASEG64(3),

    // 32 bit user code and data
    GDT_MAKE_CODESEG32(3),
    GDT_MAKE_DATASEG32(3)
};

static table_register_64_t idtr_64;
static table_register_t idtr_16 = {
    8 * 256,
    0,
    0
};

static idt_entry_64_t idt[32];

#define GDT_SEL_KERNEL_CODE64   0x08
#define GDT_SEL_KERNEL_DATA64   0x10
#define GDT_SEL_KERNEL_CODE32   0x18
#define GDT_SEL_KERNEL_DATA32   0x20
#define GDT_SEL_KERNEL_CODE16   0x28
#define GDT_SEL_KERNEL_DATA16   0x30
#define GDT_SEL_USER_CODE64     0x38
#define GDT_SEL_USER_DATA64     0x40
#define GDT_SEL_USER_CODE32     0x48
#define GDT_SEL_USER_DATA32     0x50

// See if A20 is blocked or enabled
static uint16_t check_a20()
{
    uint16_t enabled;
    __asm__ __volatile__ (
        "mov $0xFFFF,%%ax\n\t"
        "mov %%ax,%%fs\n\t"
        "pushf\n\t"
        "cli\n\t"
        "wbinvd\n\t"
        "pushw 0\n\t"
        "pushw %%fs:16\n\t"
        "movw $0,0\n\t"
        "wbinvd\n\t"
        "movw $0xface,%%fs:16\n\t"
        "wbinvd\n\t"
        "xorw %%ax,%%ax\n\t"
        "cmpw %%ax,0\n\t"
        "sete %%al\n\t"
        "popw %%fs:16\n\t"
        "popw 0\n\t"
        "popf\n\t"
        : "=a" (enabled)
    );
    return enabled;
}

// Returns BIOS error code, or zero on success
//  01h keyboard controller is in secure mode
//  86h function not supported
static uint16_t toggle_a20(uint8_t enable)
{
    uint8_t value;
    enable = (enable != 0) << 1;
    __asm__ __volatile__ (
        "inb $0x92,%1\n\t"
        "andb $~2,%1\n\t"
        "orb %0,%1\n\t"
        "wbinvd\n\t"
        "outb %1,$0x92\n\t"
        : "+c" (enable)
        , "=a" (value)
    );
    return 0;
}

// Returns true if the CPU supports that leaf
uint16_t cpuid(cpuid_t *output, uint32_t eax, uint32_t ecx)
{
    // Automatically check for support for the leaf
    if ((eax & 0x7FFFFFFF) != 0) {
        cpuid(output, eax & 0x80000000U, 0);
        if (output->eax < eax)
            return 0;
    }

    __asm__ __volatile__ (
        "cpuid"
        : "=a" (output->eax), "=c" (output->ecx)
        , "=d" (output->edx), "=b" (output->ebx)
        : "a" (eax), "c" (ecx)
    );

    return 1;
}

uint16_t cpu_has_long_mode()
{
    cpuid_t cpuinfo;
    return cpuid(&cpuinfo, 0x80000001U, 0) &&
            (cpuinfo.edx & (1<<29));
}

uint16_t cpu_has_no_execute()
{
    cpuid_t cpuinfo;
    return cpuid(&cpuinfo, 0x80000001U, 0) &&
            (cpuinfo.edx & (1<<20));
}

uint16_t cpu_has_global_pages()
{
    cpuid_t cpuinfo;
    return cpuid(&cpuinfo, 1U, 0) &&
            (cpuinfo.edx & (1<<13));
}

static uint16_t disable_interrupts()
{
    uint32_t int_enabled;
    __asm__ __volatile__ (
        "pushfl\n"
        "popl %0\n"
        "shrl $9,%0\n\t"
        "andl $1,%0\n\t"
        "cli\n\t"
        : "=r" (int_enabled)
    );
    return !!int_enabled;
}

static void enable_interrupts()
{
    __asm__ __volatile__ ("sti");
}

static void toggle_interrupts(uint16_t enable)
{
    if (enable)
        enable_interrupts();
    else
        disable_interrupts();
}

bool need_a20_toggle;
table_register_64_t gdtr;

bool nx_available;
uint32_t gp_available;

static void idt_init()
{
    for (size_t i = 0; i < 32; ++i) {
        idt[i].type_attr = IDT_PRESENT | IDT_INTR;
        idt[i].selector = IDT_SEL;
        idt[i].offset_lo = isr_table[i];
    }

    idtr_64.base_lo = (uint16_t)(uint32_t)idt;
    idtr_64.limit = 32 * sizeof(*idt) - 1;
}

static void gdt_init()
{
	gdtr.limit = sizeof(gdt)-1;
	gdtr.base_lo = (uint16_t)(uint32_t)gdt;
}

void cpu_init()
{
	idt_init();
	gdt_init();
	need_a20_toggle = !check_a20();
	if (need_a20_toggle) {
		toggle_a20(1);
		if (!check_a20()) {
			halt("A20 not enabled and enabling it failed!");
		}
	}
	nx_available = cpu_has_no_execute();
	gp_available = cpu_has_global_pages() ? (1 << 7) : 0;
}

// If src is > 0, copy size bytes from src to address
// If src is == 0, jumps to entry point in address, passing size as an argument
void copy_or_enter(uint64_t address, uint32_t src, uint32_t size)
{
    uint16_t intf = disable_interrupts();

	if (need_a20_toggle) {
        toggle_a20(1);
    }

    uint32_t pdbr = paging_root_addr();

	struct {
		uint64_t address;
		uint32_t src;
		uint32_t size;
	} params = {
		address,
		src,
		size
	};

    __asm__ __volatile__ (
        "lgdt %[gdtr]\n\t"

        // Enable protected mode
        "movl %%cr0,%%eax\n\t"
        "incl %%eax\n\t"
        "movl %%eax,%%cr0\n\t"

        // Clear prefetch queue
        "jmp 0f\n\t"
		"0:\n\t"

        // Far jump to load cs selector
        "ljmpl %[gdt_code32],$0f\n\t"

        // In protected mode
        // Switch assembler to assume 32 bit mode
        ".code32\n\t"
        "0:\n\t"

        // Load 32-bit data segments
        "movl %[gdt_data32],%%eax\n\t"
        "movl %%eax,%%ds\n\t"
        "movl %%eax,%%es\n\t"
        "movl %%eax,%%fs\n\t"
        "movl %%eax,%%ss\n\t"

        // Enable CR4.PAE (bit 5)
        "movl %%cr4,%%eax\n\t"
        "btsl $5,%%eax\n\t"
        "orl %[gp_available],%%eax\n\t"
        "movl %%eax,%%cr4\n\t"

        // Load PDBR
        "movl %[pdbr],%%eax\n\t"
        "movl %%eax,%%cr3\n\t"

        // Enable long mode IA32_EFER.LME (bit 8) MSR 0xC0000080
        "movl $0xC0000080,%%ecx\n"
        "rdmsr\n\t"
        "btsl $8,%%eax\n\t"
        "cmpw $0,%[nx_available]\n\t"
        "jz 0f\n\t"
        "btsl $11,%%eax\n\t"
        "0:"
        "wrmsr\n\t"

        // Enable paging (CR0.PG (bit 31)
        "movl %%cr0,%%eax\n\t"
        "btsl $31,%%eax\n\t"
        "movl %%eax,%%cr0\n\t"

        "jmp 0f\n\t"
        "0:"

        // Now in 64 bit compatibility mode (still really 32 bit)

        "lgdt %[gdtr]\n\t"

        // Far jump to selector that has L bit set (64 bit)
        "lea %[idtr_64],%%eax\n\t"
        "lcall %[gdt_code64],$0f\n\t"

        // Returned from long mode, back to compatibility mode

        // Load 32 bit data segments
        "movl %[gdt_data32],%%eax\n\t"
        "movl %%eax,%%ds\n\t"
        "movl %%eax,%%es\n\t"
        "movl %%eax,%%fs\n\t"
        "movl %%eax,%%ss\n\t"

        // Jump over 64 bit code
        "jmp 1f\n\t"

        // Long mode
        ".code64\n\t"
        "0:\n\t"

        "lidtq (%%eax)\n\t"

        // Deliberate crash to test exception handlers
        //"movl $0x56363,%%eax\n\t"
        //"decl (%%eax)\n\t"

        // Load 64 bit data segments
        "movl %[gdt_data64],%%eax\n\t"
        "movl %%eax,%%ds\n\t"
        "movl %%eax,%%es\n\t"
        "movl %%eax,%%fs\n\t"
        "movl %%eax,%%ss\n\t"

		// Load copy/entry parameters
		// (before screwing up stack pointer with call)
		"mov (%[params]),%%rdi\n\t"
		"movl 8(%[params]),%%esi\n\t"
		"movl 12(%[params]),%%ecx\n\t"

		// Check whether it is copy or entry
        "testl %%esi,%%esi\n\t"
        "jz 2f\n\t"

        // Copy memory
        "cld\n\t"
        "rep movsb\n\t"
        "jmp 3f\n\t"

        // Enter kernel
        "2:\n\t"
        "mov %%rsp,%%r15\n\t"
        "andq $-16,%%rsp\n\t"
        "call *%%rdi\n\t"
        // Should not be possible to reach here
        "mov %%r15,%%rsp\n\t"

        "3:\n\t"

        // Far return to 32 bit compatibility mode code segment
		"lret\n\t"

        "1:\n\t"
        ".code32\n\t"

        // Disable paging
        "mov %%cr0,%%eax\n\t"
        "btr $31,%%eax\n\t"
        "mov %%eax,%%cr0\n\t"

        // Disable long mode
        "movl $0xC0000080,%%ecx\n"
        "rdmsr\n\t"
        "andl $~0x100,%%eax\n\t"
        "wrmsr\n\t"

        "lgdt %[gdtr]\n\t"

        // Load 16 bit selectors
        "movl %[gdt_data16],%%eax\n\t"
        "movl %%eax,%%ds\n\t"
        "movl %%eax,%%es\n\t"
        "movl %%eax,%%fs\n\t"
        "movl %%eax,%%ss\n\t"

        // Load 16-bit code segment
        "ljmp %[gdt_code16],$0f\n\t"
        "nop\n\t"

        // 16-bit addressing mode reenabled
        ".code16gcc\n\t"
        "0:\n\t"

        // Disable protected mode
        "movl %%cr0,%%eax\n\t"
        "decl %%eax\n\t"
        "movl %%eax,%%cr0\n\t"

        // Clear prefetch queue
        "jmp 0f\n\t"
        "nop\n\t"
        "0:\n\t"

        // Jump to real mode
        "ljmp $0,$0f\n\t"
        "nop\n\t"
        "0:"

        // In real mode
        // Load real mode segments
        "xorw %%ax,%%ax\n\t"
        "movw %%ax,%%ds\n\t"
        "movw %%ax,%%es\n\t"
        "movw %%ax,%%fs\n\t"
        "movw %%ax,%%ss\n\t"

        "leaw %[idtr_16],%%si\n\t"
        "lidt (%%si)\n\t"
        :
		: [params] "b" (&params)
        , [pdbr] "m" (pdbr)
        , [gdtr] "m" (gdtr)
        , [idtr_64] "m" (idtr_64)
        , [idtr_16] "m" (idtr_16)
        , [gdt_code64] "i" (GDT_SEL_KERNEL_CODE64)
        , [gdt_data64] "i" (GDT_SEL_KERNEL_DATA64)
        , [gdt_code32] "i" (GDT_SEL_KERNEL_CODE32)
        , [gdt_data32] "i" (GDT_SEL_KERNEL_DATA32)
        , [gdt_code16] "i" (GDT_SEL_KERNEL_CODE16)
        , [gdt_data16] "i" (GDT_SEL_KERNEL_DATA16)
        , [nx_available] "m" (nx_available)
        , [gp_available] "m" (gp_available)
        : "eax", "ecx", "edx", "esi", "edi", "memory"
    );

	if (need_a20_toggle)
        toggle_a20(0);

    toggle_interrupts(intf);
}
