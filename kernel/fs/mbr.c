#define PART_T mbr
#define STORAGE_IMPL
#include "string.h"
#include "mbr.h"
#include "dev_storage.h"

DECLARE_part_DEVICE(mbr);

typedef struct part_dev_t part_dev_t;

struct part_dev_t {
    part_vtbl_t *vtbl;
    storage_dev_base_t *drive;
    uint64_t lba_st;
    uint64_t lba_len;
};

typedef struct partition_tbl_ent_t {
    uint8_t  boot;					//0: Boot indicator bit flag: 0 = no, 0x80 = bootable (or "active")
    uint8_t  start_head;			// H

    uint16_t start_sector : 6;		// S
    uint16_t start_cyl : 10;		// C

    uint8_t  system_id;
    uint8_t  end_head;				// H

    uint16_t end_sector : 6;		// S
    uint16_t end_cyl : 10;			// C

    uint32_t start_lba;
    uint32_t total_sectors;
} __attribute__((packed)) partition_tbl_ent_t;

#define MAX_PARTITIONS  128
static part_dev_t partitions[MAX_PARTITIONS];
static size_t partition_count;

static if_list_t mbr_detect(storage_dev_base_t *drive)
{
    char sector[512];
    char sig[2];

    drive->vtbl->read(drive, sector, 1, 0);

    memcpy(sig, sector + sizeof(sector) - sizeof(sig), sizeof(sig));

    partition_tbl_ent_t ptbl[4];
    memcpy(ptbl, sector + 446, sizeof(ptbl));

    for (int i = 0;
         i < 4 && partition_count < MAX_PARTITIONS; ++i) {
        if (ptbl[i].system_id == 0x0C) {
            part_dev_t *part = partitions + partition_count++;
            part->drive = drive;
            part->vtbl = &mbr_device_vtbl;
            part->lba_st = ptbl[i].start_lba;
            part->lba_len = ptbl[i].total_sectors;
        }
    }

    // Returns list of all partitions, not just the partitions
    // for this drive!
    if_list_t list;
    list.stride = sizeof(part_dev_t);
    list.base = partitions;
    list.count = partition_count;

    return list;
}

REGISTER_part_DEVICE(mbr);

