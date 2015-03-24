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

#UPDATE_SUBMODULES := $(shell git submodule update --init --recursive)

TARGET = sharc
CFLAGS = -Ofast -fomit-frame-pointer -w -flto -std=c99 -D_FILE_OFFSET_BITS=64

SRC_DIRECTORY = ./src/
DENSITY_DIRECTORY = ./src/density/
DENSITY_SRC_DIRECTORY = $(DENSITY_DIRECTORY)src/
SPOOKYHASH_DIRECTORY = $(DENSITY_SRC_DIRECTORY)spookyhash/
SPOOKYHASH_SRC_DIRECTORY = $(SPOOKYHASH_DIRECTORY)src/

ifeq ($(OS),Windows_NT)
    bold =
    normal =
    EXTENSION = .exe
    ARROW = ^-^>
else
    bold = `tput bold`
    normal = `tput sgr0`
    EXTENSION =
    ARROW = \-\>
endif

SHARC_SRC = $(wildcard $(SRC_DIRECTORY)*.c)
SHARC_OBJ = $(SHARC_SRC:.c=.o)
OTHER_SRC = $(wildcard $(DENSITY_SRC_DIRECTORY)*.c $(SPOOKYHASH_SRC_DIRECTORY)*.c)
OTHER_OBJ = $(OTHER_SRC:.c=.o)
ALL_OBJ = $(SHARC_OBJ) $(OTHER_OBJ)

.PHONY: compile-header link-header

all: $(TARGET)$(EXTENSION)

%.o: %.c
	@echo $^ $(ARROW) $@
	@$(CC) -c $(CFLAGS) $< -o $@

compile-submodules:
	@cd $(DENSITY_DIRECTORY) && $(MAKE) compile

compile-header: compile-submodules
	@echo ${bold}Compiling Sharc${normal} ...

compile: compile-header $(SHARC_OBJ)
	@echo Done.
	@echo

link-header: compile
	@echo ${bold}Linking Sharc${normal} ...

$(TARGET)$(EXTENSION): link-header $(ALL_OBJ)
	@echo *.o $(ARROW) $(TARGET)$(EXTENSION)
	@$(CC) -o $(TARGET)$(EXTENSION) $(ALL_OBJ)
	@echo Done.
	@echo

clean-submodules:
	@cd $(DENSITY_DIRECTORY) && $(MAKE) clean

clean: clean-submodules
	@echo ${bold}Cleaning Sharc objects${normal} ...
	@rm -f $(SHARC_OBJ)
	@rm -f $(TARGET)$(EXTENSION)
	@echo Done.
	@echo