#include <stdint.h>
#include <stddef.h>

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
uint8_t vga_color;
uint16_t* vga_buffer;
 
static inline uint8_t entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}
 
static inline uint16_t entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

void vga_terminal_initialize(void)
{
    vga_row = 0;
    vga_column = 0;
    vga_color = entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = entry(' ', vga_color);
        }
    }
}
 
void vga_setcolor(uint8_t color)
{
    vga_color = color;
}
 
void vga_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    vga_buffer[index] = entry(c, color);
}

void scroll(uint8_t by)
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
 
void vga_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        putchar(data[i]);
}
 
void vga_print(const char* data)
{
    vga_write(data, strlen(data));
}
