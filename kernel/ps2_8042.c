#include <kernel/ps2_controller.h>

#include <kernel/types.h>
#include <kernel/printk.h>
#include <arch/x86/asm.h>
#include <arch/x86/idt.h>

const u8 COMMAND_GET_CONFIG      = 0x20;
const u8 COMMAND_SET_CONFIG      = 0x60;
const u8 COMMAND_TEST            = 0xAA;
const u8 COMMAND_PORT1_ENABLE    = 0xAE;
const u8 COMMAND_PORT1_DISABLE   = 0xAD;
const u8 COMMAND_PORT2_ENABLE    = 0xA8;
const u8 COMMAND_PORT2_DISABLE   = 0xA7;
const u8 COMMAND_SEND_NEXT_PORT2 = 0xD4;

const u8 CONTROLLER_TEST_OK    = 0x55;

typedef union {
    struct {
        u8 output_full  : 1; /* Output buffer is full */
        u8 input_full   : 1; /* Input buffer is full */
        u8 system_flag  : 1; /* Cleared on power-on reset and set by firmware */
        u8 command_data : 1; /* Written data type. 0 = data for device, 1 = command for controller */
        u8 _unknown0    : 1; /* Chipset specific */
        u8 _unknown1    : 1; /* Chipset specific */
        u8 timeout_err  : 1; /* Time-out error */
        u8 parity_err   : 1; /* Parity error */
    } bits;
    u8 full;
} controller_status;

typedef union {
    struct {
        u8 port1_interrupt   : 1;
        u8 port2_interrupt   : 1;
        u8 system_flag       : 1; /* Written to the system flag of controller's status */
        u8 _must_be_zero0    : 1;
        u8 port1_clock_off   : 1;
        u8 port2_clock_off   : 1;
        u8 port1_translation : 1;
        u8 _must_be_zero1    : 1;
    } bits;
    u8 full;
} controller_config;

/* Before writing data/command */
u8 wait_input_buf()
{
    size_t timeout = 64;
    controller_status s;
    s.full = inb(STATUS_REG);
    while (s.bits.input_full && timeout-- > 0) {
        s.full = inb(STATUS_REG);
    }

    if (timeout == 0) {
        return 1;
    }
    return 0;
}

/* Before reading data*/
u8 wait_output_buf()
{
    size_t timeout = 64;
    controller_status s;
    s.full = inb(STATUS_REG);
    while (!s.bits.output_full && timeout-- > 0) {
        s.full = inb(STATUS_REG);
    }

    if (timeout == 0) {
        return 1;
    }
    return 0;
}

u8 ps2_data_read()
{
    wait_output_buf();
    return inb(DATA_PORT);
}

void ps2_write(u8 data)
{
    wait_input_buf();
    outb(DATA_PORT, data);
}

void ps2_command(u8 command)
{
    wait_input_buf();
    outb(COMMAND_REG, command);
}

void ps2_command_arg(u8 command, u8 arg)
{
    wait_input_buf();
    outb(COMMAND_REG, command);
    ps2_write(arg);
}

u8 ps2_command_resp(u8 command)
{
    wait_input_buf();
    outb(COMMAND_REG, command);
    return ps2_data_read();
}

void ps2_port1_write(u8 data)
{
    return ps2_write(data);
}

void ps2_port1_disable()
{
    ps2_command(COMMAND_PORT1_DISABLE);
}

void ps2_port1_enable()
{
    ps2_command(COMMAND_PORT1_ENABLE);
}

void ps2_port2_write(u8 data)
{
    ps2_command(COMMAND_SEND_NEXT_PORT2);
    return ps2_write(data);
}

void ps2_port2_disable()
{
    ps2_command(COMMAND_PORT2_DISABLE);
}

void ps2_port2_enable()
{
    ps2_command(COMMAND_PORT2_ENABLE);
}

extern void keyboard_handler(u8 data);
u32 kbd_int_handler(struct regs *r)
{
    // FIX: Keyboard IRQ occurs right after enabling interrupts
    controller_status s;
    s.full = inb(STATUS_REG);
    if (s.bits.output_full) {
        u8 data = inb(DATA_PORT);
        keyboard_handler(data);
    }

    return 0;
}

// TODO: Check if PS2 controller even exists with ACPI
void ps2_init()
{
    controller_config config;

    /* Disable devices */
    ps2_command(COMMAND_PORT1_DISABLE); // Disable first port
    ps2_command(COMMAND_PORT2_DISABLE); // Disable second port (ignored if there is only one port)

    /* Flush output buffer */
    inb(DATA_PORT);

    /* Set config byte */
    config.full = ps2_command_resp(COMMAND_GET_CONFIG);

    config.bits.port1_interrupt = 1;
    config.bits.port2_interrupt = 0;
    config.bits.port1_translation = 0;

    ps2_command_arg(COMMAND_SET_CONFIG, config.full);

    /* Controller self-test */
    u8 response = ps2_command_resp(COMMAND_TEST); // WARN: Can reset controller
    if (response != CONTROLLER_TEST_OK) {
        // TODO: Handle properly
        printk("ps2: controller self-test failed\n");
        return;
    }

    /* Write nuid64 in RAM. WARN: May cause bugs. No fucks are given although */
    char *nuid64_str = "nuid64";
    for (int i = 0; i < 6; ++i) {
        ps2_command_arg(0x61 + i, *(nuid64_str + i));
    }

    // TODO: Detect devices

    /* Enable devices */
    ps2_command(COMMAND_PORT1_ENABLE); // Enable first port

    /* Reset devices */
    u8 resp;
    do {
        ps2_port1_write(0xFF);
        resp = ps2_data_read();
    } while (resp == 0xFE);
    if (resp != 0xFA || ps2_data_read() != 0xAA) {
        printk("ps2: failed to reset device at port 1");
    }

    /* Set IRQs */
    irq_set_handler(KBD_IRQ, kbd_int_handler);
}
