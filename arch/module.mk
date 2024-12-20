ifeq ($(TARGET_ARCH), x86_64)
include $(dir $(lastword $(MAKEFILE_LIST)))x86_64/module.mk
endif
