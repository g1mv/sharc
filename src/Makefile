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

ifeq ($(OS),Windows_NT)
    bold =
    normal =
else
    bold = `tput bold`
    normal = `tput sgr0`
endif

TARGET = sharc
LIBSSC_SRC_DIRECTORY = ../modules/libssc/src/

CFLAGS = -std=c99 -D_FILE_OFFSET_BITS=64

ifeq ($(OS),Windows_NT)
    LIBS = c:/windows/system/
    EXTENSION = .exe
    ARROW = ^-^>
else
    LIBS = /usr/lib/
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

CC = @$(PREFIX)cc

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

ALL_OBJ = $(OBJ)

.PHONY: compile-header compile link-header link libssc-compile libssc-clean

all: compile link

$(LIBSSC_SRC_DIRECTORY)Makefile:
	@git submodule init
	@git submodule update

libssc-compile: $(LIBSSC_SRC_DIRECTORY)Makefile
	@cd $(LIBSSC_SRC_DIRECTORY) && $(MAKE) compile

compile-header:
	@echo ${bold}Compiling sharc${normal} ...

link-header:
	@echo ${bold}Linking sharc${normal} ...

compile: libssc-compile compile-header $(ALL_OBJ)
	@echo Done.
	@echo

link: compile link-header $(TARGET)$(EXTENSION)
	@echo Done.
	@echo
	
$(TARGET)$(EXTENSION): $(ALL_OBJ)
	@echo $^ $(ARROW) ${bold}$(TARGET)$(EXTENSION)${normal}
	$(CC) -o $(TARGET)$(EXTENSION) $(LIBSSC_SRC_DIRECTORY)/*.o $^ $(CFLAGS)

%.o: %.c
	@echo $^ $(ARROW) $@
	$(CC) -c $(CFLAGS) $< -o $@

libssc-clean:
	@cd $(LIBSSC_SRC_DIRECTORY) && $(MAKE) clean
	
clean: libssc-clean
	@echo ${bold}Cleaning sharc build directories${normal} ...
	@rm -f $(ALL_OBJ)
	@rm -f $(TARGET)$(EXTENSION)
	@echo Done.
	@echo
