#include <kernel/printk.h>
#include <kernel/types.h>
#include <arch/x86/vga_print.h>
#include <stdarg.h>

void itoa(char *buf, size_t num, u8 base)
{
    static const int digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                   '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    u8 idx = 0;
    if (num == 0) {
        buf[idx++] = '0';
    } else {
        while (num != 0) {
            u64 rem = num % base;
            buf[idx++] = digits[rem];
            num /= base;
        }
    }
    buf[idx] = '\0';

    // reverse digits (to the right order)
    u8 end = idx - 1; // \0 is not a part of buf
    u8 pivot = (idx+1) / 2;
    while (idx >= pivot) {
        u8 tmp = buf[end - idx];
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
                    char str[24] = {0};
                    itoa(str, va_arg(list, u64), 10);
                    vga_print(str);
                    break;
                }
                case 's': {
                    vga_print(va_arg(list, char *));
                    break;
                }
                case 'x': {
                    char str[24] = {0};
                    itoa(str, va_arg(list, u64), 16);
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
} __attribute((format (printf, 1, 2)));
