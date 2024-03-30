.PHONY: all

CUR_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

CINCLUDE += -I $(CUR_DIR)include

ASMSRC := $(wildcard *.asm)
ASMOBJ := $(patsubst %.asm, \
		     $(BUILD)/%.o, $(ASMSRC))

CSRC := $(wildcard *.c)
COBJ := $(patsubst %.c, \
		   $(BUILD)/%.o, $(CSRC))

all: $(ASMSRC) $(CSRC) $(ASMOBJ) $(COBJ)
	$(MAKE) -C boot -s
	$(MAKE) -C mm -s


$(BUILD)/%.o: %.c
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) $(CINCLUDE) $(CDEFINE) -c $< -o $@

$(BUILD)/%.o: %.asm
	@mkdir -p $(shell dirname $@)
	@nasm $(NASMFLAGS) $< -o $@
