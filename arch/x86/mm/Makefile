CSRC := $(wildcard *.c)
COBJ := $(patsubst %.c, \
		   $(BUILD)/%.o, $(CSRC))

.PHONY: mm

mm: $(CSRC) $(COBJ)

$(BUILD)/%.o: %.c
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAGS) $(CINCLUDE) -c $< -o $@
