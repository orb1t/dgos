#include "vesa.h"
#include "vesainfo.h"
#include "farptr.h"
#include "string.h"
#include "malloc.h"
#include "screen.h"
#include "paging.h"
#include "cpu.h"
#include "bioscall.h"
#include "vesainfo.h"

#define VESA_DEBUG 1
#if VESA_DEBUG
#define VESA_TRACE(...) print_line("vesa: " __VA_ARGS__)
#else
#define VESA_TRACE(...) ((void)0)
#endif

// All VBE BIOS calls have AH=4Fh
// Successful VBE calls return with AX=4F00h
// Errors have nonzero error code in AL

static uint16_t vbe_get_info(void *info, uint16_t ax, uint16_t cx)
{
    bios_regs_t regs{};

    regs.eax = ax;
    regs.ecx = cx;
    regs.edi = (uint32_t)info;

    bioscall(&regs, 0x10);

    return regs.eax & 0xFFFF;
}

static int vbe_detect(vbe_info_t *info)
{
    info->sig[0] = 'V';
    info->sig[1] = 'B';
    info->sig[2] = 'E';
    info->sig[3] = '2';
    return vbe_get_info(info, 0x4F00, 0) == 0x4F;
}

static int vbe_mode_info(vbe_mode_info_t *info, uint16_t mode)
{
    return vbe_get_info(info, 0x4F01, mode) == 0x4F;
}

static uint16_t vbe_set_mode(uint16_t mode)
{
    bios_regs_t regs;

    regs.eax = 0x4F02;
    // 0x4000 means use linear framebuffer
    regs.ebx = 0x4000 | mode;

    bioscall(&regs, 0x10);

    return (regs.eax & 0xFFFF) == 0x4F;
}

__const
static unsigned gcd(unsigned a, unsigned b)
{
    if (!a || !b)
        return a > b ? a : b;

    while (a != b) {
        if (a > b)
            a -= b;
        else
            b -= a;
    }

    return a;
}

static void aspect_ratio(uint16_t *n, uint16_t *d, uint16_t w, uint16_t h)
{
    uint16_t div = gcd(w, h);
    *n = w / div;
    *d = h / div;
}

uint16_t vbe_select_mode(uint16_t width, uint16_t height, uint16_t verbose)
{
    vbe_info_t *info;
    vbe_mode_info_t *mode_info;

    info = (vbe_info_t *)malloc(sizeof(*info));
    mode_info = (vbe_mode_info_t *)malloc(sizeof(*mode_info));

    vbe_selected_mode_t sel;

    uint16_t mode;
    uint16_t done = 0;
    if (vbe_detect(info)) {
        VESA_TRACE("VBE Memory %dMB\n", info->mem_size_64k >> 4);

        for (int ofs = 0; !done; ofs += sizeof(uint16_t)) {
            // Get mode number
            far_copy_to(&mode,
                        far_adj(info->mode_list_ptr, ofs),
                        sizeof(mode));

            if (mode == 0xFFFF)
                break;

            // Get mode information
            if (vbe_mode_info(mode_info, mode)) {
                // Ignore palette modes
                if (!mode_info->mask_size_r &&
                        !mode_info->mask_size_g &&
                        !mode_info->mask_size_b &&
                        !mode_info->mask_size_rsvd)
                    continue;

                aspect_ratio(&sel.aspect_n, &sel.aspect_d,
                             mode_info->res_x,
                             mode_info->res_y);

                if (verbose) {
                    VESA_TRACE("vbe mode %u w=%u h=%u"
                               " %d:%d:%d:%d phys_addr=%lx %d:%d",
                               mode,
                               mode_info->res_x,
                               mode_info->res_y,
                               mode_info->mask_size_r,
                               mode_info->mask_size_g,
                               mode_info->mask_size_b,
                               mode_info->mask_size_rsvd,
                               mode_info->phys_base_ptr,
                               sel.aspect_n, sel.aspect_d);
                }

                if (mode_info->res_x == width &&
                        mode_info->res_y == height &&
                        mode_info->bpp == 32) {
                    sel.width = mode_info->res_x;
                    sel.height = mode_info->res_y;
                    sel.pitch = mode_info->bytes_scanline;
                    sel.framebuffer_addr = mode_info->phys_base_ptr;
                    sel.framebuffer_bytes = info->mem_size_64k << 16;
                    sel.mask_size_r = mode_info->mask_size_r;
                    sel.mask_size_g = mode_info->mask_size_g;
                    sel.mask_size_b = mode_info->mask_size_b;
                    sel.mask_size_a = mode_info->mask_size_rsvd;
                    sel.mask_pos_r = mode_info->mask_pos_r;
                    sel.mask_pos_g = mode_info->mask_pos_g;
                    sel.mask_pos_b = mode_info->mask_pos_b;
                    sel.mask_pos_a = mode_info->mask_pos_rsvd;
                    memset(sel.reserved, 0, sizeof(sel.reserved));
                    done = 1;
                    vbe_set_mode(mode);
                    break;
                }
            }
        }
    }

    far_ptr_t kernel_data;
    kernel_data.segment = 0;
    kernel_data.offset = 0;

    if (done) {
        kernel_data.segment = far_malloc(sizeof(sel));
        kernel_data.offset = 0;
        far_copy_from(kernel_data, &sel, sizeof(sel));

        paging_map_range(kernel_data.segment << 4, sizeof(sel),
                         kernel_data.segment << 4,
                         PTE_PRESENT | PTE_WRITABLE, 2);

        uint8_t log2_pagesize = 12;
        uint64_t pte_flags = PTE_PRESENT | PTE_WRITABLE |
                (-cpu_has_global_pages() & PTE_GLOBAL);

        if (((sel.framebuffer_addr & -(1 << 21)) ==
             sel.framebuffer_addr) &&
                ((sel.framebuffer_bytes & -(1 << 21)) ==
                 sel.framebuffer_bytes)) {
            log2_pagesize = 21;
            pte_flags |= PTE_PAGESIZE;
        }

        paging_map_range(sel.framebuffer_addr, sel.framebuffer_bytes,
                         sel.framebuffer_addr, pte_flags, 2, log2_pagesize);
    }

    free(mode_info);
    free(info);

    return kernel_data.segment;
}
