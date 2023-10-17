# kernull
This is a personal playground for tinkering with OS kernel development. It's not meant for production or serious use; it's a space for experimenting, learning, and having fun with low-level code.

Nonetheless...
## Building
For building `kernull` you'll need:
- `nasm` assembler.
- Cross-compiled `binutils` and `gcc` for the `x86_64-elf` target. To acquire them, you can either follow [these steps](https://wiki.osdev.org/GCC_Cross-Compiler) or check if a package for this target is available in your package manager.
- `make` tool.
- `grub-mkrescue` from the `grub` package.
- An emulator, such as `qemu`.
