#
# Centaurean Sharc
# http://www.centaurean.com/sharc
#
# Copyright (c) 2013, Guillaume Voirin
# All rights reserved.
#
# This software is dual-licensed: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation. For the terms of this
# license, see http://www.gnu.org/licenses/gpl.html
#
# You are free to use this software under the terms of the GNU General
# Public License, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# Alternatively, you can license this library under a commercial
# license, see http://www.centaurean.com/sharc for more
# information.
#
# 01/06/13 17:27
#

bold=`tput bold`
normal=`tput sgr0`

TARGET = sharc

CFLAGS = -std=c99 -D_FILE_OFFSET_BITS=64 -I. -I./kernels/ -I./kernels/argonaut/ -I./kernels/chameleon/ -I./blocks/

ifeq ($(OS),Windows_NT)
    EXTENSION = .exe
    ARROW = ^-^>
else
    EXTENSION =
    ARROW = \-\>
endif

ifdef ARCH
    CFLAGS += -m$(ARCH)
endif

ifndef WARNINGS
    CFLAGS += -w
else
    CFLAGS += -Wall
endif

ifndef DEBUG
    CFLAGS += -O4
else
    CFLAGS += -O0 -g
endif

CC = @$(CROSS-PREFIX)cc

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
OBJ += $(addsuffix .o,$(basename $(wildcard blocks/*.c)))
OBJ += $(addsuffix _dispersion.o,$(basename $(wildcard kernels/chameleon/*.c)))
OBJ += $(addsuffix _direct.o,$(basename $(wildcard kernels/chameleon/*.c)))
OBJ += $(addsuffix _post_processing.o,$(basename $(wildcard kernels/argonaut/*.c)))
OBJ += $(addsuffix _direct.o,$(basename $(wildcard kernels/argonaut/*.c)))
.PHONY: header footer kernels blocks

all: blocks kernels header $(TARGET)$(EXTENSION) footer

header:
	@echo ${bold}Compiling sharc main components${normal} ...

blocks:
	@cd ./blocks/ && $(MAKE)

kernels:
	@cd ./kernels/ && $(MAKE)

footer:
	@echo Build completed successfully.
	@echo
	
$(TARGET)$(EXTENSION): $(OBJ)
	@echo
	@echo ${bold}Linking${normal} ...
	@echo $^ $(ARROW) ${bold}$(TARGET)$(EXTENSION)${normal}
	$(CC) -o $(TARGET)$(EXTENSION) $^ $(CFLAGS)

%.o: %.c
	@echo $^ $(ARROW) $@
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	@echo ${bold}Cleaning build directories${normal} ...
	@cd ./blocks/ && $(MAKE) clean
	@cd ./kernels/ && $(MAKE) clean
	@rm -f $(OBJ)
	@rm -f $(TARGET)$(EXTENSION)
	@echo Done.
	@echo
