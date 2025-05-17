#include <kernel/printk.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <arch/x86/vga_print.h>

// TODO: Number size and negative numbers
void itoa(char *buf, size_t num, uint8_t base)
{
	static const int digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
									'8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	uint8_t idx = 0;
	if (num == 0) {
		buf[idx++] = '0';
	} else {
		while (num != 0) {
			uint32_t rem = num % base;
			buf[idx++] = digits[rem];
			num /= base;
		}
	}
	buf[idx] = '\0';

	/* Reverse digits */
	uint8_t end = idx - 1; /* \0 */
	uint8_t pivot = (idx + 1) / 2;
	while (idx >= pivot) {
		uint8_t tmp = buf[end - idx];
		buf[end - idx] = buf[idx];
		buf[idx--] = tmp;
	}
}

void vprintk(const char *restrict fmt, va_list list)
{
	const char *c = fmt;
	while (*c != '\0') {
		if (*c == '%') {
			++c;
			switch (*c) {
			case 'c': {
				vga_putchar(va_arg(list, int));
				break;
			}
			case 'd': {
				char str[24] = { 0 };
				itoa(str, va_arg(list, uint32_t), 10);
				vga_print(str);
				break;
			}
			case 's': {
				vga_print(va_arg(list, char *));
				break;
			}
			case 'x': {
				char str[24] = { 0 };
				itoa(str, va_arg(list, uint32_t), 16);
				vga_print(str);
				break;
			}
			}
		} else {
			vga_putchar(*c);
		}

		++c;
	}
}

void printk(char *fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	vprintk(fmt, list);
	va_end(list);
}
__attribute((format(printf, 1, 2)));
