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
SRC_DIRECTORY = ./src/
DENSITY_SRC_DIRECTORY = ./src/density/src/
SPOOKYHASH_SRC_DIRECTORY = ./src/density/src/spookyhash/src/

ifeq ($(OS),Windows_NT)
    EXTENSION = .exe
    ARROW = ^-^>
else
    EXTENSION =
    ARROW = \-\>
endif

SRC = $(wildcard $(SRC_DIRECTORY)*.c $(DENSITY_SRC_DIRECTORY)*.c $(SPOOKYHASH_SRC_DIRECTORY)*.c)
OBJ = $(SRC:.c=.o)

ALL_OBJ = $(OBJ)

.PHONY: link-header link

all: compile link-header link

$(DENSITY_SRC_DIRECTORY)Makefile:
	@echo ${bold}Cloning Density${normal} ...
	@git submodule init
	@git submodule update
	@echo

$(SPOOKYHASH_SRC_DIRECTORY)Makefile: $(DENSITY_SRC_DIRECTORY)Makefile
	@echo ${bold}Cloning SpookyHash${normal} ...
	@cd $(DENSITY_SRC_DIRECTORY)
	@git submodule init
	@git submodule update
	@echo

link-header:
	@echo ${bold}Linking Sharc${normal} ...

compile: $(DENSITY_SRC_DIRECTORY)Makefile $(SPOOKYHASH_SRC_DIRECTORY)Makefile
	@cd $(SPOOKYHASH_SRC_DIRECTORY) && $(MAKE) compile
	@cd $(DENSITY_SRC_DIRECTORY) && $(MAKE) compile
	@cd $(SRC_DIRECTORY) && $(MAKE) compile

link: compile link-header $(TARGET)$(STAT_EXT) $(TARGET)$(DYN_EXT)
	@echo Done.
	@echo

$(TARGET)$(EXTENSION): $(ALL_OBJ)
	@echo *.o $(ARROW) $(TARGET)$(EXTENSION)
	@$(CC) -o $(TARGET)$(EXTENSION) $(OBJ)

clean:
	@rm -f $(TARGET)$(EXTENSION)
	@cd $(SPOOKYHASH_SRC_DIRECTORY) && $(MAKE) clean
	@cd $(DENSITY_SRC_DIRECTORY) && $(MAKE) clean
	@cd $(SRC_DIRECTORY) && $(MAKE) clean