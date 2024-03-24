#include <kernel/ps2_keyboard.h>

#include <kernel/types.h>
#include <kernel/ps2_controller.h>
#include <kernel/printk.h>
#include <kernel/input.h>
#include <abort.h>

// TODO: Handle command responses properly

/* Helper struct to use in command queue */
struct ps2_kbd_command {
    u8 com;
/* Stub value for command without argument */
#define COMMAND_NO_ARG 0xFF
    u8 arg;
};

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

#define EMUL0           0xe0
#define EMUL1           0xe1
#define RELEASE         0xf0

// TODO: Other scan code sets
u8 scan2_norm_to_keycode[] = {
    /* 0x00 */       0,      KEY_F9,              0,  KEY_F5,        KEY_F3,    KEY_F1,        KEY_F2,     KEY_F12, /* 0x08 */            0,        KEY_F10,    KEY_F8,         KEY_F6,         KEY_F4,       KEY_TAB,      KEY_GRAVE, 0,
    /* 0x10 */       0, KEY_LEFTALT,  KEY_LEFTSHIFT,       0,  KEY_LEFTCTRL,     KEY_Q,         KEY_1,           0, /* 0x18 */            0,              0,     KEY_Z,          KEY_S,          KEY_A,         KEY_W,          KEY_2, 0,
    /* 0x20 */       0,       KEY_C,          KEY_X,   KEY_D,         KEY_E,     KEY_4,         KEY_3,           0, /* 0x28 */            0,      KEY_SPACE,     KEY_V,          KEY_F,          KEY_T,         KEY_R,          KEY_5, 0,
    /* 0x30 */       0,       KEY_N,          KEY_B,   KEY_H,         KEY_G,     KEY_Y,         KEY_6,           0, /* 0x38 */            0,              0,     KEY_M,          KEY_J,          KEY_U,         KEY_7,          KEY_8, 0,
    /* 0x40 */       0,   KEY_COMMA,          KEY_K,   KEY_I,         KEY_O,     KEY_0,         KEY_9,           0, /* 0x48 */            0,        KEY_DOT, KEY_SLASH,          KEY_L,  KEY_SEMICOLON,         KEY_P,      KEY_MINUS, 0,
    /* 0x50 */       0,           0, KEY_APOSTROPHE,       0, KEY_LEFTBRACE, KEY_EQUAL,             0,           0, /* 0x58 */ KEY_CAPSLOCK, KEY_RIGHTSHIFT, KEY_ENTER, KEY_RIGHTBRACE,              0, KEY_BACKSLASH,              0, 0,
    /* 0x60 */       0,           0,              0,       0,             0,         0, KEY_BACKSPACE,           0, /* 0x68 */            0,        KEY_KP1,         0,              0,              0,       KEY_KP7,              0, 0,
    /* 0x70 */ KEY_KP0,   KEY_KPDOT,        KEY_KP2, KEY_KP5,       KEY_KP6,   KEY_KP8,       KEY_ESC, KEY_NUMLOCK, /* 0x78 */      KEY_F11,     KEY_KPPLUS,   KEY_KP3,    KEY_KPMINUS, KEY_KPASTERISK,       KEY_KP9, KEY_SCROLLLOCK, 0,
    /* 0x80 */       0,           0,              0,  KEY_F7,
};
u8 scan2_ext_to_keycode[] = {
    /* 0x00 */            0,              0,            0,        0,             0,                0, 0,             0, /* 0x08 */             0,       0,            0,          0,        0,            0,          0,                    0,
    /* 0x10 */      KEY_WWW,   KEY_RIGHTALT,            0,        0, KEY_RIGHTCTRL, KEY_PREVIOUSSONG, 0,             0, /* 0x18 */ KEY_BOOKMARKS,       0,            0,          0,        0,            0,          0,         KEY_LEFTMETA,
    /* 0x20 */  KEY_REFRESH, KEY_VOLUMEDOWN,            0, KEY_MUTE,             0,                0, 0, KEY_RIGHTMETA, /* 0x28 */      KEY_STOP,       0,     KEY_CALC,          0,        0,            0,          0, KEY_ALL_APPLICATIONS,
    /* 0x30 */  KEY_FORWARD,              0, KEY_VOLUMEUP,        0,      KEY_PLAY,                0, 0,     KEY_POWER, /* 0x38 */      KEY_BACK,       0, KEY_HOMEPAGE, KEY_STOPCD,        0,            0,          0,            KEY_SLEEP,
    /* 0x40 */ KEY_COMPUTER,              0,            0,        0,             0,                0, 0,             0, /* 0x48 */     KEY_EMAIL,       0,  KEY_KPSLASH,          0,        0, KEY_NEXTSONG,          0,                    0,
    /* 0x50 */    KEY_MEDIA,              0,            0,        0,             0,                0, 0,             0, /* 0x58 */             0,       0,  KEY_KPENTER,          0,        0,            0, KEY_WAKEUP,                    0,
    /* 0x60 */            0,              0,            0,        0,             0,                0, 0,             0, /* 0x68 */             0, KEY_END,            0,   KEY_LEFT, KEY_HOME,            0,          0,                    0,
    /* 0x70 */   KEY_INSERT,     KEY_DELETE,     KEY_DOWN,        0,     KEY_RIGHT,           KEY_UP, 0,             0, /* 0x78 */             0,       0, KEY_PAGEDOWN,          0,        0, KEY_PAGEDOWN,          0,                    0,
};

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
    ps2_kbd_command_arg(command, COMMAND_NO_ARG);
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

static void handle_data(u8 data)
{
    static u8 ext0 = 0;
    static u8 released = 0;
    static u8 ext1 = 0;

    static u8 scan_code_buf[8] = {0};
    static u8 *scan_code_ptr = scan_code_buf;
    static u8 expect = 1;

    static u8 is_pause = 0;
    static u8 is_sysrq = 0;

    *scan_code_ptr = data;
    ++scan_code_ptr;
    --expect;

    if (data == EMUL1) {
        ext1 = 1;
        expect += 2; // WARN: That's inaccurate but the last byte check below handles that
        return;
    } else if (data == EMUL0) {
        ext0 = 1;
        expect += 1;
        return;
    } else if (data == RELEASE) {
        released = 1;
        expect += 1;
        return;
    }

    // Check if it is some fu-~nny sequence
    if (ext0 && !released && data == 0x12) { // PrtSc press first part
        is_sysrq = 1;
        ext0 = 0;
        expect += 1;
        return;
    } else if (ext0 && released && data == 0x7C) { // PrtSc release first part
        is_sysrq = 1;
        ext0 = 0;
        released = 1;
        expect += 1;
        return;
    } else if (ext1 && released && data == 0x77) { // Pause/Break sequence last byte
        is_pause = 1;
        expect = 0;
        return;
    }

    /* Scan code is complete */
    if (expect == 0) {
        struct input_event event = {0};

        if (is_sysrq) {
            event.code = KEY_SYSRQ;
            is_sysrq = 0;
        } else if (is_pause) {
            event.code = KEY_PAUSE;
            is_pause = 0;
        } else {
            event.code  = ext0? scan2_ext_to_keycode[data] : scan2_norm_to_keycode[data];
            event.flags |= released? INPUT_FLAG_KEY_RELEASED : 0;
        }

        // TODO: Pass to some handler
        static char keycode_to_char[256] = "001234567890-=\0\tqwertyuiop[]\0\0asdfghjkl;'`\0\\zxcvbnm,./\0*\0 ";
        if ((event.flags & INPUT_FLAG_KEY_RELEASED) == 0) {
            char c = keycode_to_char[event.code];
            if (c != '\0')
                printk("%c", keycode_to_char[event.code]);
        }

        released = ext0 = ext1 = 0;
        scan_code_ptr = scan_code_buf;
        expect = 1;
    }

    return;
}

/* Keyboard interrupt handler */
void keyboard_handler(u8 data)
{
    if (driver_state == STATE_READY) {
        handle_data(data);
        return;
    }

    struct ps2_kbd_command command = current_command();

    if (data != ACK && command.com != PS2_KBD_COM_ECHO) {
        printk("ps2_kbd: expected ACK for ");

        /* Distinguish ACK for command and for argument */
        if (driver_state == STATE_WAITING_ARG_ACK) {
            printk("arg of ");
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
        if (command.arg != COMMAND_NO_ARG) {
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
