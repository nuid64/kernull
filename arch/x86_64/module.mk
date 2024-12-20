$(info x86_64)
ASMSRC += $(wildcard $(dir $(lastword $(MAKEFILE_LIST)))*.S)
ASMSRC += $(wildcard $(dir $(lastword $(MAKEFILE_LIST)))boot/*.S)
