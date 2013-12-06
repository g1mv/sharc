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
DENSITY_SRC_DIRECTORY = ./src/density/src/
SHARC_SRC_DIRECTORY = ./src/sharc/src/

CFLAGS = -flto -std=c99 -D_FILE_OFFSET_BITS=64

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

CC = @$(PREFIX)cc$(SUFFIX)

SRC = $(wildcard $(SHARC_SRC_DIRECTORY)*.c)
OBJ = $(SRC:.c=.o)

ALL_OBJ = $(OBJ)

.PHONY: compile link-header link sharc-compile sharc-clean density-compile density-clean

all: link

$(DENSITY_SRC_DIRECTORY)Makefile:
	@echo ${bold}Cloning Density${normal} ...
	@git submodule init
	@git submodule update
	@echo

density-compile: $(DENSITY_SRC_DIRECTORY)Makefile
	@cd $(DENSITY_SRC_DIRECTORY) && $(MAKE) compile

sharc-compile:
	@cd $(SHARC_SRC_DIRECTORY) && $(MAKE) compile

link-header:
	@echo ${bold}Linking Sharc${normal} ...

compile: density-compile sharc-compile

link: compile link-header $(TARGET)$(EXTENSION)
	@echo Done.
	@echo
	
$(TARGET)$(EXTENSION):
	@echo *.o $(ARROW) $(TARGET)$(EXTENSION)
	$(CC) -o $(TARGET)$(EXTENSION) $(SHARC_SRC_DIRECTORY)*.o $(DENSITY_SRC_DIRECTORY)*.o $(CFLAGS)

density-clean:
	@cd $(DENSITY_SRC_DIRECTORY) && $(MAKE) clean
	
sharc-clean:
	@cd $(SHARC_SRC_DIRECTORY) && $(MAKE) clean

clean: density-clean sharc-clean
	@echo ${bold}Removing $(TARGET)$(EXTENSION)${normal} ...
	@rm -f $(TARGET)$(EXTENSION)
	@echo Done.
