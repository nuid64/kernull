#include <arch/x86/vga_print.h>

#include <stddef.h>
#include <stdint.h>

#include <kernel/string.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t vga_row;
size_t vga_column;
uint8_t vga_color;
uint16_t *vga_buffer;

static inline uint8_t entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t entry(char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}

void vga_terminal_initialize(void)
{
	vga_row = 0;
	vga_column = 0;
	vga_color = entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	vga_buffer = (uint16_t *)0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			vga_buffer[index] = entry(' ', vga_color);
		}
	}
}

void vga_set_color(uint8_t color)
{
	vga_color = color;
}

void vga_set_pos(uint8_t x, uint8_t y)
{
	vga_column = x;
	vga_row = y;
}

static void vga_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	vga_buffer[index] = entry(c, color);
}

static void scroll(uint8_t by)
{
	for (size_t y = by; y != VGA_HEIGHT; ++y)
		for (size_t x = 0; x != VGA_WIDTH; ++x) {
			const size_t src = y * VGA_WIDTH + x;
			const size_t dest = (y - by) * VGA_WIDTH + x;
			vga_buffer[dest] = vga_buffer[src];
			vga_putentryat(' ', vga_color, x, y);
		}
}

void vga_putchar(char c)
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

void vga_write(const char *s, size_t size)
{
	for (size_t i = 0; i < size; i++)
		vga_putchar(s[i]);
}

void vga_print(const char *s)
{
	vga_write(s, strlen(s));
}
