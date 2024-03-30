ASMSRC := $(wildcard *.asm)
ASMOBJ := $(patsubst %.asm, \
             $(BUILD)/%.o, $(ASMSRC))

.PHONY: boot

boot: $(ASMSRC) $(ASMOBJ)

$(BUILD)/%.o: %.asm
	@mkdir -p $(shell dirname $@)
	@nasm $(NASMFLAGS) $< -o $@
