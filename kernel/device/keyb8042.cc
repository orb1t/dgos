#include "keyb8042.h"
#include "keyboard.h"
#include "mouse.h"

#include "irq.h"
#include "cpu/ioport.h"
#include "cpu/atomic.h"
#include "printk.h"
#include "time.h"
#include "string.h"

// Read/write
#define KEYB_DATA   0x60

// Read only
#define KEYB_STATUS 0x64

// Write only
#define KEYB_CMD    0x64

// Status bits
#define KEYB_STAT_RDOK_BIT      0
#define KEYB_STAT_WRNOTOK_BIT   1

#define KEYB_STAT_RDOK          (1<<KEYB_STAT_RDOK_BIT)
#define KEYB_STAT_WRNOTOK       (1<<KEYB_STAT_WRNOTOK_BIT)

#define KEYB_KEY_IRQ            1
#define KEYB_MOUSE_IRQ          12

#define KEYB_CMD_DISABLE_PORT1  0xAD
#define KEYB_CMD_DISABLE_PORT2  0xA7
#define KEYB_CMD_ENABLE_PORT1   0xAE
#define KEYB_CMD_ENABLE_PORT2   0xA8
#define KEYB_CMD_RDCONFIG       0x20
#define KEYB_CMD_WRCONFIG       0x60
#define KEYB_CMD_CTLTEST        0xAA
#define KEYB_CMD_TEST_PORT1     0xAB
#define KEYB_CMD_TEST_PORT2     0xA9
#define KEYB_REPLY_PASSED       0x55
#define KEYB_REPLY_RESETOK_1    0xFA
#define KEYB_REPLY_RESETOK_2    0xAA
#define KEYB_CMD_MOUSECMD       0xD4

#define PS2MOUSE_RESET          0xFF
#define PS2MOUSE_ENABLE         0xF4
#define PS2MOUSE_SETSAMPLERATE  0xF3
#define PS2MOUSE_SETRES         0xE8
#define PS2MOUSE_REPLY_ACK      0xFA
#define PS2MOUSE_REPLY_RESEND   0xFE
#define PS2MOUSE_READ_ID        0xF2

#define PS2MOUSE_RES_1CPMM      0
#define PS2MOUSE_RES_2CPMM      1
#define PS2MOUSE_RES_4CPMM      2
#define PS2MOUSE_RES_8CPMM      3

#define KEYB_CONFIG_IRQ_PORT1_BIT       0
#define KEYB_CONFIG_IRQ_PORT2_BIT       1
#define KEYB_CONFIG_POST_OK_BIT         2
#define KEYB_CONFIG_CLKDIS_PORT1_BIT    4
#define KEYB_CONFIG_CLKDIS_PORT2_BIT    5
#define KEYB_CONFIG_XLAT_PORT1_BIT      6

#define KEYB_CONFIG_IRQEN_PORT1     (1 << KEYB_CONFIG_IRQ_PORT1_BIT)
#define KEYB_CONFIG_IRQEN_PORT2     (1 << KEYB_CONFIG_IRQ_PORT2_BIT)
#define KEYB_CONFIG_POST_OK         (1 << KEYB_CONFIG_POST_OK_BIT)
#define KEYB_CONFIG_CLKDIS_PORT1    (1 << KEYB_CONFIG_CLKDIS_PORT1_BIT)
#define KEYB_CONFIG_CLKDIS_PORT2    (1 << KEYB_CONFIG_CLKDIS_PORT2_BIT)
#define KEYB_CONFIG_XLAT_PORT1      (1 << KEYB_CONFIG_XLAT_PORT1_BIT)

#define KEYB8042_DEBUG  0
#if KEYB8042_DEBUG
#define KEYB8042_DEBUGMSG(p) printk p
#else
#define KEYB8042_DEBUGMSG(p) ((void)0)
#endif

static keyb8042_layout_t *keyb8042_layout = &keyb8042_layout_us;

// Lookup table of keyboard layouts
static keyb8042_layout_t *keyb8042_layouts[] = {
    &keyb8042_layout_us,
    0
};

static char const keyb8042_passthru_lookup[] =
        " \b\n";

enum keyb8042_key_state_t {
    NORMAL,
    IN_E0,
    IN_E1_1,
    IN_E1_2
};

static keyb8042_key_state_t keyb8042_state = NORMAL;

// Mouse packet data
static uint64_t keyb8042_last_mouse_packet_time;
static size_t keyb8042_mouse_packet_level;
static size_t keyb8042_mouse_packet_size;
static size_t keyb8042_mouse_button_count;
static uint8_t keyb8042_mouse_packet[5];

static keybd_fsa_t keyb8042_fsa;

static void keyb8042_keyboard_handler(void)
{
    uint8_t scancode = 0;

    scancode = inb(KEYB_DATA);

    int32_t vk = 0;
    int sign = !!(scancode & 0x80) * -2 + 1;

    switch (keyb8042_state) {
    case NORMAL:
        if (scancode == 0xE0) {
            keyb8042_state = IN_E0;
            break;
        }
        if (scancode == 0xE1) {
            keyb8042_state = IN_E1_1;
            break;
        }
        scancode &= 0x7F;
        vk = keyb8042_layout->scancode[scancode];
        break;

    case IN_E0:
        scancode &= 0x7F;
        vk = keyb8042_layout->scancode_0xE0[scancode];
        keyb8042_state = NORMAL;
        break;

    case IN_E1_1:
        keyb8042_state = IN_E1_2;
        break;

    case IN_E1_2:
        keyb8042_state = NORMAL;
        vk = KEYB_VK_PAUSE;
        break;

    default:
        keyb8042_state = NORMAL;
        break;
    }

    keyb8042_fsa.deliver_vk(vk * sign);
}

static void keyb8042_process_mouse_packet(uint8_t const *packet)
{
    mouse_raw_event_t event;
    event.hdist = 0;
    event.vdist = 0;
    event.wdist = 0;
    event.buttons = 0;

    switch (keyb8042_mouse_packet_size) {
    case 4:
        if (keyb8042_mouse_button_count == 5)
            event.buttons |= (packet[3] >> 1) & 0x18;

        // Sign extend 4 bit wheel distance
        event.wdist = (int16_t)((uint16_t)packet[3] << 12) >> 12;
    }
    // First byte:
    // +---+---+----+----+---+-----+-----+-----+
    // | ? | ? | VS | HS | 1 | MMB | RMB | LMB |
    // +---+---+----+----+---+-----+-----+-----+
    //   7   6   5    4    3    2     1     0
    //
    // Second byte is horizontal motion, positive = right
    // Third byte is vertical motion, positive = up
    //
    // Distance range -256 <= dist < 255

    // Setup sign bits
    event.hdist = ((int16_t)((uint16_t)packet[0] << 11) & 0x8000);
    event.vdist = ((int16_t)((uint16_t)packet[0] << 10) & 0x8000);

    // Sign extend
    event.hdist >>= 8;
    event.vdist >>= 8;

    event.hdist |= packet[1];
    event.vdist |= packet[2];

    event.buttons |= packet[0] & 0x07;

    mouse_event(event);
}

static void keyb8042_mouse_handler(void)
{
    uint8_t data = inb(KEYB_DATA);

    // If it has been > 500ms since the last IRQ,
    // then discard the queued bytes (if any)
    // and start from the beginning. This handles
    // the unlikely case of getting out of sync
    // with the mouse

    uint64_t now = time_ns();
    if (keyb8042_last_mouse_packet_time + 500000000 < now)
        keyb8042_mouse_packet_level = 0;
    keyb8042_last_mouse_packet_time = now;

    if (keyb8042_mouse_packet_size > 0)
        keyb8042_mouse_packet[keyb8042_mouse_packet_level++] = data;

    if (keyb8042_mouse_packet_size &&
            keyb8042_mouse_packet_level ==
            keyb8042_mouse_packet_size) {
        keyb8042_process_mouse_packet(keyb8042_mouse_packet);
        keyb8042_mouse_packet_level = 0;
    }
}

static isr_context_t *keyb8042_handler(int irq, isr_context_t *ctx)
{
    if (irq == KEYB_KEY_IRQ)
        keyb8042_keyboard_handler();
    else if (irq == KEYB_MOUSE_IRQ)
        keyb8042_mouse_handler();

    return ctx;
}

// Prints error message and returns -1 on timeout
static int keyb8042_send_command(uint8_t command)
{
    uint32_t max_tries = 100000;
    while ((inb(KEYB_STATUS) & KEYB_STAT_WRNOTOK) != 0 &&
           --max_tries)
        pause();

    if (max_tries > 0)
        outb(KEYB_CMD, command);

    return max_tries != 0 ? 0 : -1;
}

// Prints error message and returns -1 on timeout
static int keyb8042_read_data(void)
{
    uint32_t max_tries = 100000;
    while ((inb(KEYB_STATUS) & KEYB_STAT_RDOK) == 0 &&
           --max_tries)
        pause();

    if (!max_tries)
        printk("Timeout reading keyboard\n");

    return max_tries ? inb(KEYB_DATA) : -1;
}

// Prints error message and returns -1 on timeout
static int keyb8042_write_data(uint8_t data)
{
    uint32_t max_tries = 100000;
    while ((inb(KEYB_STATUS) & KEYB_STAT_WRNOTOK) != 0 &&
           --max_tries)
        pause();

    if (max_tries > 0)
        outb(KEYB_DATA, data);
    else if (!max_tries)
        printk("Timeout writing keyboard\n");

    return max_tries > 0 ? 0 : -1;
}

static int keyb8042_retry_mouse_command(uint8_t command)
{
    int last_data;

    int max_tries = 4;
    do {
        if (keyb8042_send_command(KEYB_CMD_MOUSECMD) < 0)
            return -1;

        if (keyb8042_write_data(command) < 0)
            return -1;

        last_data = keyb8042_read_data();

        if (last_data == PS2MOUSE_REPLY_ACK)
            break;

        printk("Mouse did not acknowledge\n");
    } while (--max_tries);

    return max_tries != 0 ? last_data : -1;
}

static int keyb8042_magic_sequence(uint8_t expected_id, size_t seq_length, ...)
{
    // Send magic sequence
    va_list ap;
    va_start(ap, seq_length);
    for (size_t i = 0; i < seq_length; ++i) {
        uint8_t command = (uint8_t)va_arg(ap, unsigned);
        if (keyb8042_retry_mouse_command(command) < 0)
            return -1;
    }
    va_end(ap);

    // Read ID
    if (keyb8042_retry_mouse_command(PS2MOUSE_READ_ID) < 0)
        return -1;

    int id = keyb8042_read_data();

    if (id != expected_id)
        return 0;

    // Successfully detected
    return 1;
}

static int keyb8042_set_layout_name(char const *name)
{
    for (keyb8042_layout_t **layout = keyb8042_layouts;
         *layout; ++layout) {
        keyb8042_layout_t *this_layout = *layout;

        if (!strcmp(this_layout->name, name)) {
            keyb8042_layout = this_layout;
            return 1;
        }
    }

    // Not found
    return 0;
}

static int keyb8042_get_modifiers()
{
    return keyb8042_fsa.get_modifiers();
}

void keyb8042_init(void)
{
    // Disable both ports
    KEYB8042_DEBUGMSG(("Disabling keyboard controller ports\n"));
    keyb8042_send_command(KEYB_CMD_DISABLE_PORT1);
    keyb8042_send_command(KEYB_CMD_DISABLE_PORT2);

    // Flush incoming byte, if any
    KEYB8042_DEBUGMSG(("Flushing keyboard output buffer\n"));
    inb(KEYB_DATA);

    // Read config
    KEYB8042_DEBUGMSG(("Reading keyboard controller config\n"));
    if (keyb8042_send_command(KEYB_CMD_RDCONFIG) < 0)
        return;
    int config = keyb8042_read_data();
    if (config < 0)
        return;

    KEYB8042_DEBUGMSG(("Keyboard original config = %02x\n", config));

    // Disable IRQs and translation
    config &= ~(KEYB_CONFIG_IRQEN_PORT1 |
                KEYB_CONFIG_IRQEN_PORT2 |
                KEYB_CONFIG_XLAT_PORT1);

    int port2_exists = 1;
            //!(config & KEYB_CONFIG_CLKDIS_PORT2);

    // Write config
    KEYB8042_DEBUGMSG(("Writing keyboard controller config = %02x\n", config));
    if (keyb8042_send_command(KEYB_CMD_WRCONFIG) < 0)
        return;
    if (keyb8042_write_data(config) < 0)
        return;

#if 1
    int ctl_test_result;
    int port1_test_result;

    if (keyb8042_send_command(KEYB_CMD_CTLTEST) < 0)
        return;

    ctl_test_result = keyb8042_read_data();
    if (ctl_test_result < 0)
        return;

    if (ctl_test_result != KEYB_REPLY_PASSED)
        printk("Keyboard controller self test failed! result=%02x\n",
               ctl_test_result);

    if (keyb8042_send_command(KEYB_CMD_TEST_PORT1) < 0)
        return;
    port1_test_result = keyb8042_read_data();
    if (port1_test_result < 0)
        return;

    if (port1_test_result != 0)
        printk("Keyboard port 1 self test failed! result=%02x\n",
               ctl_test_result);

    if (port2_exists) {
        int port2_test_result;
        if (keyb8042_send_command(KEYB_CMD_TEST_PORT2) < 0)
            return;
        port2_test_result = keyb8042_read_data();
        if (port2_test_result < 0)
            return;

        if (port2_test_result != 0) {
            printk("Keyboard port 2 self test failed! result=%02x\n",
                   port2_test_result);
            port2_exists = 0;
        }
    }
#else
    int port1_test_result;
#endif

    // Reset
    KEYB8042_DEBUGMSG(("Resetting keyboard\n"));
    if (keyb8042_write_data(0xFF) < 0)
        return;

    port1_test_result = keyb8042_read_data();
    if (port1_test_result < 0)
        return;
    if (port1_test_result == KEYB_REPLY_RESETOK_1) {
        port1_test_result = keyb8042_read_data();
        if (port1_test_result != KEYB_REPLY_RESETOK_2) {
            KEYB8042_DEBUGMSG(("Keyboard reset failed!\n"));
            return;
        }
    }

    KEYB8042_DEBUGMSG(("Enabling keyboard port\n"));
    if (keyb8042_send_command(KEYB_CMD_ENABLE_PORT1) < 0)
        return;

    KEYB8042_DEBUGMSG(("Enabling mouse port\n"));
    if (keyb8042_send_command(KEYB_CMD_ENABLE_PORT2) < 0)
        return;

    config &= ~KEYB_CONFIG_CLKDIS_PORT1;
    config |= KEYB_CONFIG_IRQEN_PORT1 | KEYB_CONFIG_XLAT_PORT1;
    if (port2_exists) {
        config &= ~KEYB_CONFIG_CLKDIS_PORT2;
        config |= KEYB_CONFIG_IRQEN_PORT2;
    }

    KEYB8042_DEBUGMSG(("Writing keyboard controller config = %02x\n", config));
    if (keyb8042_send_command(KEYB_CMD_WRCONFIG) < 0)
        return;
    if (keyb8042_write_data(config) < 0)
        return;

    // Reset mouse
    KEYB8042_DEBUGMSG(("Resetting mouse\n"));
    if (keyb8042_retry_mouse_command(PS2MOUSE_RESET) < 0)
        return;
    KEYB8042_DEBUGMSG(("Reading reset ok\n"));
    if (keyb8042_read_data() != KEYB_REPLY_RESETOK_2)
        printk("Mouse did not acknowledge\n");
    KEYB8042_DEBUGMSG(("Reading reset result\n"));
    if (keyb8042_read_data() != 0x00)
        printk("Mouse did not acknowledge\n");

    // Set mouse resolution
    KEYB8042_DEBUGMSG(("Setting mouse resolution\n"));
    if (keyb8042_retry_mouse_command(PS2MOUSE_SETRES) < 0)
        return;
    if (keyb8042_retry_mouse_command(PS2MOUSE_RES_1CPMM) < 0)
        return;

    // Enable mouse stream mode
    KEYB8042_DEBUGMSG(("Setting mouse to stream mode\n"));
    if (keyb8042_retry_mouse_command(PS2MOUSE_ENABLE) < 0)
        return;

    keyb8042_mouse_packet_size = 3;
    keyb8042_mouse_button_count = 2;

    // Attempt to detect better mouse

    if (keyb8042_magic_sequence(
                0x04, 6, 0xF3, 0xC8, 0xF3, 0xC8, 0xF3, 0x50) > 0) {
        // Attempt to detect 5 button mouse with wheel
        // (Intellimouse Explorer compatible)
        printk("Detected 5 button mouse with wheel\n");
        keyb8042_mouse_button_count = 5;
        keyb8042_mouse_packet_size = 4;
    } else if (keyb8042_magic_sequence(
                   0x03, 6, 0xF3, 0xC8, 0xF3, 0x64, 0xF3, 0x50) > 0) {
        // Attempt to detect 3 button mouse with wheel
        // (Intellimouse compatible)
        printk("Detected mouse with wheel\n");
        keyb8042_mouse_button_count = 3;
        keyb8042_mouse_packet_size = 4;
    }

    // Set mouse sampling rate
    KEYB8042_DEBUGMSG(("Setting mouse sampling rate\n"));
    if (keyb8042_retry_mouse_command(PS2MOUSE_SETSAMPLERATE) < 0)
        return;
    if (keyb8042_retry_mouse_command(100) < 0)
        return;

    printk("Keyboard/mouse initialization complete\n");

    irq_hook(1, keyb8042_handler, "keyb8042");
    irq_setmask(1, 1);

    if (port2_exists) {
        printk("Mouse enabled\n");
        irq_hook(12, keyb8042_handler, "keyb8042_mouse");
        irq_setmask(12, 1);
    }

    keybd_get_modifiers = keyb8042_get_modifiers;
    keybd_set_layout_name = keyb8042_set_layout_name;

    printk("Keyboard enabled\n");
}
