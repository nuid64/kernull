#include <kernel/ps2_keyboard.h>

#include <kernel/types.h>
#include <kernel/ps2_controller.h>
#include <kernel/printk.h>
#include <abort.h>

// TODO: Handle command responses properly

/* Helper struct to use in command queue */
struct ps2_kbd_command {
    u8 com;
    u8 arg;
};
/* Stub value for command without argument */
#define NO_ARG 0xFF

/* Keyboard command ring buffer */
#define COMMAND_QUEUE_SIZE 8
static struct ps2_kbd_command command_queue[COMMAND_QUEUE_SIZE];
static size_t next_command_idx = 0;
static size_t pending_commands = 0;

/* Driver state */
#define STATE_READY           0
#define STATE_WAITING_COM_ACK 1
#define STATE_WAITING_ARG_ACK 2
static u8 driver_state = STATE_READY;

/* Current scan code set */
static u8 kbd_scan_code_set = 1;

#define ACK 0xFA
#define RESEND 0xFE
#define SCAN_CODE_SET_1 0x43
#define SCAN_CODE_SET_2 0x41
#define SCAN_CODE_SET_3 0x3f

static void queue_add(u8 command, u8 arg)
{
    if (pending_commands < COMMAND_QUEUE_SIZE) {
        command_queue[next_command_idx].com = command;
        command_queue[next_command_idx].arg = arg;

        next_command_idx = (next_command_idx + 1) % COMMAND_QUEUE_SIZE;
        ++pending_commands;
    }
}

static void queue_discard()
{
    if (pending_commands > 0) {
        --pending_commands;
    }
}

void ps2_kbd_command_arg(u8 command, u8 arg)
{
    ps2_port1_disable();

    if (pending_commands < COMMAND_QUEUE_SIZE) {
        queue_add(command, arg);

        if (pending_commands == 1) {
            driver_state = STATE_WAITING_COM_ACK;
            ps2_port1_write(command);
        }

        /* Ask keyboard what scan set it's using AFTER COMMAND TO USE SCAN CODE SET X
         * WARN: Will cause fuckup if buffer is full, but that's unlikely to happen, right?
         * How the fuck my beautiful buffer can be flooded so bad?
         */
        if (command == PS2_KBD_COM_SET_SCAN_SET && arg != 0) {
            queue_add(PS2_KBD_COM_SET_SCAN_SET, 0x00);
        }
    }

    ps2_port1_enable();
}

void ps2_kbd_command(u8 command)
{
    ps2_kbd_command_arg(command, NO_ARG);
}

static struct ps2_kbd_command current_command()
{
    if (pending_commands > 0) {
        size_t idx = ((next_command_idx + COMMAND_QUEUE_SIZE) - pending_commands) % COMMAND_QUEUE_SIZE;
        return command_queue[idx];
    }

    struct ps2_kbd_command ret = {0};
    return ret;
}

/* Called within interrupt handler only. Can't be reentered thus */
static void handle_next_command()
{
    queue_discard();
    if (pending_commands > 0) {
        driver_state = STATE_WAITING_COM_ACK;
        ps2_port1_write(current_command().com);
    } else {
        driver_state = STATE_READY;
    }
}

/* Keyboard interrupt handler */
void keyboard_handler(u8 data)
{
    struct ps2_kbd_command command = current_command();

    if (driver_state == STATE_READY) {
        // TODO: Handle properly
        printk("%x", data);

        return;
    }

    if (data != ACK && command.com != PS2_KBD_COM_ECHO) {
        printk("ps2_kbd: expected ACK for ");

        /* Distinguish ACK for command and for argument */
        if (driver_state == STATE_WAITING_ARG_ACK) {
            printk("arg of");
        }

        printk("command (0x%x, 0x%x). Got 0x%x\n",
               command.com, command.arg, data);
    }

    if (driver_state == STATE_WAITING_COM_ACK) {
        if (data == RESEND) {
            ps2_port1_write(command.com);

            return;
        }

        /* Send arg if command has one */
        if (command.arg != NO_ARG) {
            driver_state = STATE_WAITING_ARG_ACK;
            ps2_port1_write(command.arg);

            return;
        }
        
        if (command.com == PS2_KBD_COM_ECHO) {
            printk("\nps2_kbd: ECHO\n");
        }
    } else if (driver_state == STATE_WAITING_ARG_ACK) {
        if (data == RESEND) {
            ps2_port1_write(command.arg);
            return;
        }

        /* Asked for current set */
        if (command.com == PS2_KBD_COM_SET_SCAN_SET) {
            if (command.arg == 0) {
                data = ps2_data_read();

                if (data == SCAN_CODE_SET_1) {
                    kbd_scan_code_set = 1;
                } else if (data == SCAN_CODE_SET_2) {
                    kbd_scan_code_set = 2;
                } else if (data == SCAN_CODE_SET_3) {
                    kbd_scan_code_set = 3;
                } else {
                    printk("ps2_kbd: Strange scan code set byte 0x%x\n", data);
                }
            }
        }
    } else {
        /* XXX: Find better way to handle this.
         * Flush command queue and act like nothing happened?
         */
        printk("ps2_kbd: Corrupted state 0x%x\n", driver_state);
        abort();
    }

    handle_next_command();
}

u8 ps2_kbd_get_scan_code_set()
{
    return kbd_scan_code_set;
}
