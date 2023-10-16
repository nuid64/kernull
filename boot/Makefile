BUILD := ../build

ASM_SRC := $(wildcard *.asm)
ASM_OBJ := $(patsubst %.asm, \
             $(BUILD)/%.o, $(ASM_SRC))
NASMFLAGS := -felf64

.PHONY: boot

boot: $(ASM_OBJ)

$(BUILD)/%.o: %.asm
	@mkdir -p $(shell dirname $@)
	@nasm $(NASMFLAGS) $< -o $@
