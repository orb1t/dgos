#include "paging.h"
#include "screen.h"
#include "malloc.h"
#include "screen.h"
#include "farptr.h"

// Builds 64-bit page tables
//
// Each page is 4KB and contains 512 64-bit entries
// There are 4 levels of page tables
//  Linear
//  Address
//  Bits
//  -----------
//   47:39 Page directory (512GB regions) (PML4e)
//   38:30 Page directory (1GB regions)   (PDPTE)
//   29:21 Page directory (2MB regions)   (PDE)
//   20:12 Page table     (4KB regions)   (PTE)
//   11:0  Offset (bytes)

#define DEBUG_PAGING	0
#if DEBUG_PAGING
#define PAGING_TRACE(...) print_line("paging: " __VA_ARGS__)
#else
#define PAGING_TRACE(...) ((void)0)
#endif

// Structure to hold the segment and slot for the PTE for a given address
struct pte_ref_t {
    uint16_t segment;
    uint16_t slot;
};

static uint16_t root_page_dir;

static uint64_t *pte_ptr(uint16_t segment, uint16_t slot)
{
    return (uint64_t*)seg_to_ptr(segment) + slot;
}

// Read a 64-bit entry from the specified slot of the specified segment
static uint64_t read_pte(uint16_t segment, uint16_t slot)
{
    return *pte_ptr(segment, slot);
}

// Write a 64-bit entry to the specified slot of the specified segment
static void write_pte(uint16_t segment, uint16_t slot, pte_t pte)
{
    *pte_ptr(segment, slot) = pte;
}

static void clear_page_table(uint16_t segment)
{
    memset(pte_ptr(segment, 0), 0, 512 * sizeof(pte_t));
}

static uint16_t allocate_page_table()
{
    uint16_t segment = far_malloc_aligned(PAGE_SIZE);
    clear_page_table(segment);
    return segment;
}

// Returns with segment == 0 if it does mapping does not exist
static pte_ref_t paging_find_pte(addr64_t linear_addr, bool create,
                                 uint8_t log2_pagesize = 12)
{
    pte_ref_t ref;
    ref.segment = root_page_dir;

    pte_t pte;

    // Process the address bits from high to low
    // in groups of 9 bits

    for (uint8_t shift = 39; ; shift -= 9) {
        // Extract 9 bits of the linear address
        ref.slot = (uint16_t)(linear_addr >> shift) & 0x1FF;

        // If we are in the last level page table, then done
        if (shift == log2_pagesize)
            break;

        // Read page table entry
        pte = read_pte(ref.segment, ref.slot);

        uint16_t next_segment = (uint16_t)(pte >> 4) & 0xFF00;

        if (next_segment == 0) {
            if (!create) {
                ref.segment = 0;
                break;
            }

            //print_line("Creating page directory for %llx",
            //           (pte_t)(linear_addr >> shift) << shift);

            // Allocate a page table on first use
            next_segment = allocate_page_table();

            pte = (uint64_t)next_segment << 4;
            pte |= PTE_PRESENT | PTE_WRITABLE;
            write_pte(ref.segment, ref.slot, pte);
        }

        ref.segment = next_segment;
    }

    return ref;
}

// If keep is != 0 and the page had a mapping, return 0
// Otherwise, return 1
static uint16_t paging_map_page(
        uint64_t linear_addr,
        uint64_t phys_addr,
        uint64_t pte_flags,
        uint16_t keep,
        uint8_t log2_pagesize = 12)
{
    pte_ref_t ref = paging_find_pte(linear_addr, 1, log2_pagesize);

    // Read page table entry
    uint64_t pte = read_pte(ref.segment, ref.slot);

    // If keep flag is set, avoid overwriting mapping
    if (keep && (pte & PTE_ADDR))
        return 0;

    //print_line("mapping %llx to physaddr %llx pageseg=%x slot=%x",
    //           linear_addr, phys_addr,
    //           ref.segment, ref.slot);

    write_pte(ref.segment, ref.slot, phys_addr | pte_flags);

    return 1;
}

void paging_alias_range(addr64_t alias_addr,
                        addr64_t linear_addr,
                        size64_t size,
                        pte_t alias_flags)
{
    PAGING_TRACE("aliasing %llu bytes at lin %llx to physaddr %llx\n",
                 size, linear_addr, alias_addr);

    for (uint64_t offset = 0; offset < size; offset += PAGE_SIZE) {
        pte_ref_t original = paging_find_pte(linear_addr, 0);
        pte_ref_t alias_ref = paging_find_pte(alias_addr, 1);

        uint64_t pte;

        if (original.segment != 0) {
            pte = read_pte(original.segment, original.slot);

            write_pte(alias_ref.segment, alias_ref.slot,
                      (pte & PTE_ADDR) | alias_flags);
        } else {
            write_pte(alias_ref.segment, alias_ref.slot,
                      alias_flags & ~(PTE_PRESENT | PTE_ADDR));
        }
    }
}

// If keep is 1, then only advance phys_addr when a new
// page was assigned, otherwise
// If keep is 2, then always advance phys_addr
uint64_t paging_map_range(
        uint64_t linear_base,
        uint64_t length,
        uint64_t phys_addr,
        uint64_t pte_flags,
        uint16_t keep,
        uint8_t log2_pagesize)
{
    uint32_t page_size = 1U << log2_pagesize;
    size_t misalignment = linear_base & (page_size - 1);
    linear_base -= misalignment;
    length += misalignment;
    length = (length + (page_size - 1)) & -(int)page_size;


    uint64_t allocated = 0;
    for (uint64_t offset = 0; offset < length; offset += page_size) {
        if (paging_map_page(linear_base + offset,
                            phys_addr,
                            pte_flags,
                            keep, log2_pagesize) || keep > 1)
        {
            ++allocated;
            phys_addr += page_size;
        }
    }
    return allocated;
}

uint32_t paging_root_addr()
{
    return root_page_dir << 4;
}

// Identity map the first 64KB of physical addresses and
// prepare to populate tables
void paging_init()
{
    // Clear the root page directory
    root_page_dir = far_malloc_aligned(PAGE_SIZE);

    clear_page_table(root_page_dir);

    // Identity map first 64KB
    paging_map_range(0, 0x10000, 0,
                     PTE_PRESENT | PTE_WRITABLE, 0);
}

void paging_modify_flags(addr64_t addr, size64_t size,
                         pte_t clear, pte_t set)
{
    for (addr64_t offset = 0; offset < size; offset += PAGE_SIZE) {
        pte_ref_t original = paging_find_pte(addr + offset, 0);
        if (original.segment) {
            pte_t pte = read_pte(original.segment, original.slot);
            pte &= ~clear;
            pte |= set;
            write_pte(original.segment, original.slot, pte);
        }
    }
}
