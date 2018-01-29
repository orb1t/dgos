#pragma once
#include "types.h"
#include "pcibits.h"
#include "assert.h"
#include "irq.h"

struct pci_addr_t {
    // Legacy PCI supports 256 busses, 32 slots, 8 functions, and 64 dwords
    // PCIe supports 16777216 busses, 32 slots, 8 functions, 1024 dwords
    // PCIe organizes the busses into up to 65536 segments of 256 busses

    //        43       28 27   20 19  15 14  12 11       2 1  0
    //       +-----------+-------+------+------+----------+----+
    //  PCIe |  segment  |  bus  | slot | func |   dword  |byte|
    //       +-----------+-------+------+------+----------+----+
    //            16         8       5      3       10       2
    //
    //                    27   20 19  15 14  12 .. 7     2 1  0
    //                   +-------+------+------+--+-------+----+
    //  PCI              |  bus  | slot | func |xx| dword |byte|
    //                   +-------+------+------+--+-------+----+
    //                       8       5      3       10       2
    //
    //                        31       16 15    8 7    3 2    0
    //                       +-----------+-------+------+------+
    //  addr                 |  segment  |  bus  | slot | func |
    //                       +-----------+-------+------+------+
    //                            16         8       5      3

    pci_addr_t()
        : addr(0)
    {
    }

    pci_addr_t(int seg, int bus, int slot, int func)
        : addr((uint32_t(seg) << 16) | (bus << 8) | (slot << 3) | (func))
    {
        assert(seg >= 0);
        assert(seg < 65536);
        assert(bus >= 0);
        assert(bus < 256);
        assert(slot >= 0);
        assert(slot < 32);
        assert(func >= 0);
        assert(func < 8);
    }

    int bus() const
    {
        return (addr >> 8) & 0xFF;
    }

    int slot() const
    {
        return (addr >> 3) & 0x1F;
    }

    int func() const
    {
        return addr & 0x7;
    }

    // Returns true if segment is zero
    bool is_legacy() const
    {
        return (addr < 65536);
    }

    uint64_t get_addr() const
    {
        return addr << 12;
    }

private:
    uint32_t addr;
};

struct pci_config_hdr_t {
    uint16_t vendor;
    uint16_t device;

    uint16_t command;
    uint16_t status;

    uint8_t revision;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t dev_class;

    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;

    uint32_t base_addr[6];

    uint32_t cardbus_cis_ptr;

    uint16_t subsystem_vendor;
    uint16_t subsystem_id;

    uint32_t expansion_rom_addr;
    uint8_t capabilities_ptr;

    uint8_t reserved[7];

    uint8_t irq_line;
    uint8_t irq_pin;
    uint8_t min_grant;
    uint8_t max_latency;
};

#define PCI_DEV_CLASS_UNCLASSIFIED      0x00
#define PCI_DEV_CLASS_STORAGE           0x01
#define PCI_DEV_CLASS_NETWORK           0x02
#define PCI_DEV_CLASS_DISPLAY           0x03
#define PCI_DEV_CLASS_MULTIMEDIA        0x04
#define PCI_DEV_CLASS_MEMORY            0x05
#define PCI_DEV_CLASS_BRIDGE            0x06
#define PCI_DEV_CLASS_COMM              0x07
#define PCI_DEV_CLASS_SYSTEM            0x08
#define PCI_DEV_CLASS_INPUT             0x09
#define PCI_DEV_CLASS_DOCKING           0x0A
#define PCI_DEV_CLASS_PROCESSOR         0x0B
#define PCI_DEV_CLASS_SERIAL            0x0C
#define PCI_DEV_CLASS_WIRELESS          0x0D
#define PCI_DEV_CLASS_INTELLIGENT       0x0E
#define PCI_DEV_CLASS_SATELLITE         0x0F
#define PCI_DEV_CLASS_ENCRYPTION        0x10
#define PCI_DEV_CLASS_DSP               0x11
#define PCI_DEV_CLASS_ACCELERATOR       0x12
#define PCI_DEV_CLASS_INSTRUMENTATION   0x13
#define PCI_DEV_CLASS_COPROCESSOR       0x40
#define PCI_DEV_CLASS_UNASSIGNED        0xFF

// PCI_DEV_CLASS_NETWORK
#define PCI_SUBCLASS_NETWORK_ETHERNET   0x00
#define PCI_SUBCLASS_NETWORK_TOKENRING  0x01
#define PCI_SUBCLASS_NETWORK_FDDI       0x02
#define PCI_SUBCLASS_NETWORK_ATM        0x03
#define PCI_SUBCLASS_NETWORK_ISDN       0x04
#define PCI_SUBCLASS_NETWORK_WFLIP      0x05
#define PCI_SUBCLASS_NETWORK_PICMGMC    0x06
#define PCI_SUBCLASS_NETWORK_OTHER      0x80

// PCI_DEV_CLASS_STORAGE
#define PCI_SUBCLASS_STORAGE_SCSI       0x00
#define PCI_SUBCLASS_STORAGE_IDE        0x01
#define PCI_SUBCLASS_STORAGE_FLOPPY     0x02
#define PCI_SUBCLASS_STORAGE_IPIBUS     0x03
#define PCI_SUBCLASS_STORAGE_RAID       0x04
#define PCI_SUBCLASS_STORAGE_ATA        0x05
#define PCI_SUBCLASS_STORAGE_SATA       0x06
#define PCI_SUBCLASS_STORAGE_SAS        0x07
#define PCI_SUBCLASS_STORAGE_NVM        0x08
#define PCI_SUBCLASS_STORAGE_MASS       0x80

// PCI_DEV_CLASS_SERIAL
#define PCI_SUBCLASS_SERIAL_IEEE1394    0x00
#define PCI_SUBCLASS_SERIAL_ACCESS      0x01
#define PCI_SUBCLASS_SERIAL_SSA         0x02
#define PCI_SUBCLASS_SERIAL_USB         0x03
#define PCI_SUBCLASS_SERIAL_FIBRECHAN   0x04
#define PCI_SUBCLASS_SERIAL_SMBUS       0x05
#define PCI_SUBCLASS_SERIAL_INFINIBAND  0x06
#define PCI_SUBCLASS_SERIAL_IPMISMIC    0x07
#define PCI_SUBCLASS_SERIAL_SERCOS      0x08
#define PCI_SUBCLASS_SERIAL_CANBUS      0x09

// PCI_SUBCLASS_STORAGE_SATA
#define PCI_PROGIF_STORAGE_SATA_VEND    0x00
#define PCI_PROGIF_STORAGE_SATA_AHCI    0x01
#define PCI_PROGIF_STORAGE_SATA_SERIAL  0x02

// PCI_SUBCLASS_STORAGE_NVM
#define PCI_PROGIF_STORAGE_NVM_NVME		0x02

// PCI_SUBCLASS_SERIAL_USB
#define PCI_PROGIF_SERIAL_USB_UHCI      0x00
#define PCI_PROGIF_SERIAL_USB_OHCI      0x10
#define PCI_PROGIF_SERIAL_USB_EHCI      0x20
#define PCI_PROGIF_SERIAL_USB_XHCI      0x30
#define PCI_PROGIF_SERIAL_USB_UNSPEC    0x80
#define PCI_PROGIF_SERIAL_USB_USBDEV    0xFE

C_ASSERT(offsetof(pci_config_hdr_t, capabilities_ptr) == 0x34);

#define PCI_CFG_STATUS_CAPLIST_BIT  4
#define PCI_CFG_STATUS_CAPLIST      (1<<PCI_CFG_STATUS_CAPLIST_BIT)

struct pci_dev_t {
    pci_config_hdr_t config;
    pci_addr_t addr;
};

struct pci_dev_iterator_t : public pci_dev_t {
    operator pci_addr_t() const
    {
        return pci_addr_t(segment, bus, slot, func);
    }

    int segment;
    int bus;
    int slot;
    int func;

    int dev_class;
    int subclass;

    uint8_t header_type;

    uint8_t bus_todo_len;
    uint8_t bus_todo[64];
};

int pci_init(void);

int pci_enumerate_begin(pci_dev_iterator_t *iter,
                        int dev_class, int subclass);
int pci_enumerate_next(pci_dev_iterator_t *iter);

uint32_t pci_config_read(pci_addr_t addr, int offset, int size);

bool pci_config_write(pci_addr_t addr,
        size_t offset, void *values, size_t size);

void pci_config_copy(pci_addr_t addr,
                     void *dest, int ofs, size_t size);

int pci_find_capability(pci_addr_t addr,
        int capability_id);

int pci_enum_capabilities(pci_addr_t addr,
        int (*callback)(uint8_t, int, uintptr_t), uintptr_t context);

//
// PCI capability IDs

// Power management
#define PCICAP_PM       0x1

// Accelerated Graphics Port
#define PCICAP_AGP      0x2

// Vital Product Data
#define PCICAP_VPD      0x3

// Slot Identification (external expansion)
#define PCICAP_SLOTID   0x4

// Message Signaled Interrupts
#define PCICAP_MSI      0x5

// CompactPCI Hotswap
#define PCICAP_HOTSWAP  0x6

// PCI-X
#define PCICAP_PCIX     0x7

// HyperTransport
#define PCICAP_HTT      0x8

// Vendor specific
#define PCICAP_VENDOR   0x9

// Debug port
#define PCICAP_DEBUG    0xA

// CompactPCI central resource control
#define PCICAP_CPCI     0xB

// PCI Hot-plug
#define PCICAP_HOTPLUG  0xC

// PCI bridge subsystem vendor ID
#define PCICAP_BRID     0xD

// AGP 8x
#define PCICAP_AGP8x    0xE

// Secure device
#define PCICAP_SECDEV   0xF

// PCI Express
#define PCICAP_PCIE     0x10

// MSI-X
#define PCICAP_MSIX     0x11

struct pci_irq_range_t {
    uint8_t base;
    uint8_t count;
};

void pci_init_ecam(size_t ecam_count);
void pci_init_ecam_entry(uint64_t base, uint16_t seg,
                         uint8_t st_bus, uint8_t en_bus);
void pci_init_ecam_enable();

bool pci_try_msi_irq(pci_dev_iterator_t const& pci_dev,
                     pci_irq_range_t *irq_range,
                     int cpu, bool distribute, int req_count,
                     intr_handler_t handler, int const *target_cpus = nullptr);

bool pci_set_msi_irq(pci_addr_t addr,
                    pci_irq_range_t *irq_range,
                    int cpu, bool distribute, int req_count,
                    intr_handler_t handler, int const *target_cpus = nullptr);

void pci_set_irq_line(pci_addr_t addr,
                      uint8_t irq_line);

void pci_set_irq_pin(pci_addr_t addr,
                     uint8_t irq_pin);

void pci_adj_control_bits(pci_dev_t const& pci_dev,
                          uint16_t set, uint16_t clr);

void pci_adj_control_bits(int bus, int slot, int func,
                          uint16_t set, uint16_t clr);

void pci_clear_status_bits(pci_addr_t addr,
                           uint16_t bits);

char const * pci_device_class_text(uint8_t cls);
