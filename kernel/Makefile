CSRC := $(wildcard *.c)
COBJ := $(patsubst %.c, \
		   $(BUILD)/%.o, $(CSRC))

.PHONY: kernel

kernel: $(CSRC) $(COBJ)

$(BUILD)/%.o: %.c
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) $(CINCLUDE) $(CDEFINE) -c $< -o $@
