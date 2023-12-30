.PHONY: all

CUR_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

CINCLUDE += -I $(CUR_DIR)include

all:
	$(MAKE) -C boot -s
	$(MAKE) -C kernel -s
	$(MAKE) -C mm -s
