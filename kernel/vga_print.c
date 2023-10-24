#include <stddef.h>
#include <kernel/types.h>

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t vga_row;
size_t vga_column;
u8 vga_color;
u16* vga_buffer;
 
static inline u8 entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}
 
static inline u16 entry(char uc, u8 color)
{
    return (u16) uc | (u16) color << 8;
}
 
size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

void vga_terminal_initialize()
{
    vga_row = 0;
    vga_column = 0;
    vga_color = entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_buffer = (u16*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = entry(' ', vga_color);
        }
    }
}
 
void vga_setcolor(u8 color)
{
    vga_color = color;
}
 
void vga_putentryat(char c, u8 color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    vga_buffer[index] = entry(c, color);
}

void scroll(u8 by)
{
    for (size_t y = by; y != VGA_HEIGHT; ++y)
        for (size_t x = 0; x != VGA_WIDTH; ++x) {
            const size_t src = y * VGA_WIDTH + x;
            const size_t dest = (y - by) * VGA_WIDTH + x;
            vga_buffer[dest] = vga_buffer[src];
            vga_putentryat(' ', vga_color, x, y);
        }
}
 
void putchar(char c)
{
    if (c == '\n') {
        vga_column = 0;
        vga_row++;
    } else {
        vga_putentryat(c, vga_color, vga_column, vga_row);
        if (++vga_column == VGA_WIDTH) {
            vga_column = 0;
            ++vga_row;
        }
    }

    if (vga_row == VGA_HEIGHT) {
        scroll(1);
        --vga_row;
    }
}
 
void vga_write(const char* s, size_t size)
{
    for (size_t i = 0; i < size; i++)
        putchar(s[i]);
}
 
void vga_print(const char* s)
{
    vga_write(s, strlen(s));
}

void vga_print_num(u64 num)
{
    // HINT
    // "nuid64, but 21 byte would be enough to squeeze u64 with \0. Why did you..."
    // ALIGNMENT, FUCKERS
    char str[24];
    u8 idx = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = 'x';
        str[2] = '0';
        str[3] = '\0';
    } else {
        while (num != 0) {
            u64 rem = num % 16;
            str[idx++] = (rem > 9)? (rem - 10) + 'a' : rem + '0';
            num /= 16;
        }

        // place 0x because I want it to be (it'll stand right after reverse)
        str[idx++] = 'x';
        str[idx++] = '0';

        str[idx] = '\0';

        // reverse digits (to the right order)
        --idx;                          // \0 is not a part of str
        u8 end = idx - 1;
        u8 pivot = (idx+1) / 2;
        while (idx >= pivot) {
            u8 tmp = str[end - idx];
            str[end - idx] = str[idx];
            str[idx] = tmp;
            --idx;
        }
    }

    vga_print(str);
}
