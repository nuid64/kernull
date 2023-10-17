BUILD := ../build

ASM_SRC := $(wildcard *.asm)
ASM_OBJ := $(patsubst %.asm, \
		     $(BUILD)/%.o, $(ASM_SRC))

C_SRC := $(wildcard *.c)
C_OBJ := $(patsubst %.c, \
		   $(BUILD)/%.o, $(C_SRC))

CFLAGS := -ffreestanding -nostdlib -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra -O2
NASMFLAGS := -felf64


.PHONY: kernel

kernel: $(ASM_OBJ) $(C_OBJ)

$(BUILD)/%.o: %.c
	@mkdir -p $(shell dirname $@)
	@x86_64-elf-gcc $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.asm
	@mkdir -p $(shell dirname $@)
	@nasm $(NASMFLAGS) $< -o $@
