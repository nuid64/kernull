RM := rm -f

# Disable make's built-ins
MAKEFLAGS += -rR

PROJ_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILD := $(PROJ_ROOT)build

# Assuming you want to build for the only architecture supported :/
ARCH := x86_64

CC := clang
ifeq ($(ARCH), x86_64)
  CC += --target=x86_64
endif

CFLAGS := -nostdlib -Wall -Wextra -O2
CFLAGS += -ffreestanding -fno-strict-aliasing
CFLAGS += -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
CINCLUDE := -I $(PROJ_ROOT)include

ifeq ($(ARCH), x86_64)
CDEFINE += -DARCH_X86_64
endif

NASMFLAGS := -felf64

ifeq ($(ARCH), x86_64)
  QEMU := qemu-system-x86_64
endif

export PROJ_ROOT
export BUILD
export ARCH
export CC
export CFLAGS
export CINCLUDE
export CDEFINE
export NASMFLAGS


KERNEL := isodir/boot/kernull-$(ARCH).bin
ISO := isodir/kernull-$(ARCH).iso

GRUB_CFG := isodir/boot/grub/grub.cfg

OBJ := $(BUILD)/*.o

LDFLAGS := -Tlinker/$(ARCH).ld -n

.PHONY: all run iso
.PHONY: clean clean_all

all: $(KERNEL)

clean:
	$(info Ripping bin and image)
	@$(RM) $(KERNEL)
	@$(RM) $(ISO)

clean_all: clean
	$(info Removing objects)
	@$(RM) -r $(BUILD)

run: $(ISO)
	$(info Running QEMU)
	@$(QEMU) -cdrom $(ISO)

iso: $(ISO)

$(ISO): $(KERNEL) $(GRUB_CFG)
	$(info Making image)
	@mkdir -p $(BUILD)/isofiles/boot/grub
	@cp $(KERNEL) $(BUILD)/isofiles/boot/kernull.bin
	@cp $(GRUB_CFG) $(BUILD)/isofiles/boot/grub
	@grub-mkrescue -o $(ISO) $(BUILD)/isofiles 2> /dev/null
	@rm -r $(BUILD)/isofiles

$(KERNEL):
	$(info Building kernel)
	@$(MAKE) -C arch/$(ARCH) -s
	@$(MAKE) -C kernel -s
	@ld.lld $(LDFLAGS) -o $(KERNEL) $(OBJ)
