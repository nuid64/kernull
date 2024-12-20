MODULES := arch

# Disable make's built-ins
MAKEFLAGS += -rR

RM := rm -f

# Assuming you want to build for the only architecture supported
TARGET_ARCH ?= x86_64
ifeq ($(TARGET_ARCH), x86_64)
TARGET := --target=x86_64
QEMU := qemu-system-x86_64
endif

PROJDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILDDIR := $(PROJDIR)build/
ISODIR := $(PROJDIR)isodir/
KERNELBIN := $(ISODIR)boot/kernull-$(TARGET_ARCH).bin
KERNELISO := $(ISODIR)kernull-$(TARGET_ARCH).iso

GRUB_CFG := $(ISODIR)boot/grub/grub.cfg

LDFLAGS := -Tlink/$(TARGET_ARCH).ld -n

CC := clang

CFLAGS ?= -nostdlib -Wall -Wextra -O2 \
          -ffreestanding -fno-strict-aliasing \
          -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
CINCL ?=

# Modules will add to this
ASMSRC :=
CSRC :=

# Include modules' makefiles
include $(patsubst %, %/module.mk, $(MODULES))

OBJ := \
$(patsubst %.c, $(BUILDDIR)%.o, $(filter %.c,$(CSRC))) \
$(patsubst %.S, $(BUILDDIR)%.o, $(filter %.S,$(ASMSRC)))


.PHONY: all clean clean_all format run

all: $(KERNELISO)

clean:
	$(RM) -r $(BUILDDIR)*

clean_all: clean
	$(RM) $(KERNELISO)
	$(RM) $(KERNELBIN)

format:
	clang-format --style=file -i $(CSRC)

run: $(KERNELISO)
	@$(QEMU) -cdrom $(KERNELISO)

$(KERNELISO): $(KERNELBIN)
	mkdir -p $(dir $@)
	mkdir -p $(BUILDDIR)isofiles/boot/grub/
	cp $(KERNELBIN) $(BUILDDIR)isofiles/boot/kernull.bin
	cp $(GRUB_CFG) $(BUILDDIR)isofiles/boot/grub/
	grub-mkrescue -o $(KERNELISO) $(BUILDDIR)isofiles 2> /dev/null
	$(RM) -r $(BUILDDIR)isofiles

$(KERNELBIN): $(OBJ)
	mkdir -p $(BUILDDIR)
	mkdir -p $(dir $@)
	ld.lld -o $@ $(LDFLAGS) $(OBJ)

$(BUILDDIR)%.o: %.S
	mkdir -p $(dir $@)
	@$(CC) -o $@ $(CFLAGS) $(CINCLUDE) $(CDEFINE) $(TARGET) -c $<

$(BUILDDIR)%.o: %.c
	mkdir -p $(dir $@)
	@$(CC) -o $@ $(CFLAGS) $(CINCLUDE) $(CDEFINE) $(TARGET) -c $<
