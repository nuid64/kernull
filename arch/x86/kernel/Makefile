ASMSRC := $(wildcard *.asm)
ASMOBJ := $(patsubst %.asm, \
		     $(BUILD)/%.o, $(ASMSRC))

CSRC := $(wildcard *.c)
COBJ := $(patsubst %.c, \
		   $(BUILD)/%.o, $(CSRC))

NASMFLAGS := -felf64

.PHONY: kernel

kernel: $(ASMSRC) $(CSRC) $(ASMOBJ) $(COBJ)

$(BUILD)/%.o: %.c
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) $(CINCLUDE) -c $< -o $@

$(BUILD)/%.o: %.asm
	@mkdir -p $(shell dirname $@)
	@nasm $(NASMFLAGS) $< -o $@
