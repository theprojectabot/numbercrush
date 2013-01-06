APP = NumberSmash
SDK_DIR = /Users/benb/Dropbox/Programming/sifteo


STIR = $(SDK_DIR)/bin/stir
LD = $(SDK_DIR)/bin/slinky
CC = $(SDK_DIR)/bin/arm-clang

BIN = /Users/benb/Desktop/$(APP).elf

include $(SDK_DIR)/Makefile.defs

OBJS = $(ASSETS).gen.o main.o
ASSETDEPS += *.png $(ASSETS).lua
CCFLAGS += -DCUBE_ALLOCATION=24

include $(SDK_DIR)/Makefile.rules
