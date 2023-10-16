RM := rm -f

BUILD := build

KERNEL := isodir/boot/kernull.bin
ISO := isodir/kernull.iso

LINKER_SCRIPT := linker/linker.ld
GRUB_CFG := isodir/boot/grub/grub.cfg

OBJ := $(BUILD)/*.o

LDFLAGS := -T$(LINKER_SCRIPT) -n


.PHONY: all run iso
.PHONY: clean

all: $(KERNEL)

clean:
	$(RM) -r $(BUILD)
	$(RM) $(KERNEL)
	$(RM) $(ISO)

run: $(ISO)
	@qemu-system-x86_64 -cdrom $(ISO)

iso: $(ISO)

$(ISO): $(KERNEL) $(GRUB_CFG)
	@mkdir -p $(BUILD)/isofiles/boot/grub
	@cp $(KERNEL) $(BUILD)/isofiles/boot/kernull.bin
	@cp $(GRUB_CFG) $(BUILD)/isofiles/boot/grub
	@grub-mkrescue -o $(ISO) $(BUILD)/isofiles 2> /dev/null
	@rm -r $(BUILD)/isofiles

$(KERNEL): $(LINKER_SCRIPT)
	$(MAKE) -C boot -s
	# $(MAKE) -C kernel -s
	x86_64-elf-ld $(LDFLAGS) -o $(KERNEL) $(OBJ)
