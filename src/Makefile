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

CC = @$(PREFIX)cc

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

SRC_BLOCKS = $(wildcard blocks/*.c)
OBJ_BLOCKS = $(SRC_BLOCKS:.c=.o)

SRC_CHAMELEON = $(wildcard kernels/chameleon/*.c)
OBJ_CHAMELEON_DEFAULT = $(SRC_CHAMELEON:.c=_default.o)
OBJ_CHAMELEON_DISPERSION = $(SRC_CHAMELEON:.c=_dispersion.o)

SRC_ARGONAUT = $(wildcard kernels/argonaut/*.c)
OBJ_ARGONAUT_DEFAULT = $(SRC_ARGONAUT:.c=_default.o)
OBJ_ARGONAUT_POST_PROCESSING = $(SRC_ARGONAUT:.c=_post_processing.o)

.PHONY: header footer kernels blocks

all: header $(TARGET)$(EXTENSION) footer

header:
	@echo ${bold}Compiling${normal} ...

footer:
	@echo Build completed successfully.
	@echo
	
$(TARGET)$(EXTENSION): $(OBJ) $(OBJ_BLOCKS) $(OBJ_CHAMELEON_DEFAULT) $(OBJ_CHAMELEON_DISPERSION) $(OBJ_ARGONAUT_DEFAULT) $(OBJ_ARGONAUT_POST_PROCESSING)
	@echo
	@echo ${bold}Linking${normal} ...
	@echo $^ $(ARROW) ${bold}$(TARGET)$(EXTENSION)${normal}
	$(CC) -o $(TARGET)$(EXTENSION) $^ $(CFLAGS)

%_default.o: %.c
	@echo $^ $(ARROW) $@
	$(CC) -c $(CFLAGS) $< -o $@

%_dispersion.o: %.c
	@echo $^ $(ARROW) $@
	$(CC) -c $(CFLAGS) -DSHARC_CHAMELEON_DISPERSION $< -o $@

%_post_processing.o: %.c
	@echo $^ $(ARROW) $@
	$(CC) -c $(CFLAGS) -DSHARC_ARGONAUT_POST_PROCESSING $< -o $@

%.o: %.c
	@echo $^ $(ARROW) $@
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	@echo ${bold}Cleaning build directories${normal} ...
	@rm -f $(OBJ) $(OBJ_BLOCKS) $(OBJ_CHAMELEON_DEFAULT) $(OBJ_CHAMELEON_DISPERSION) $(OBJ_ARGONAUT_DEFAULT) $(OBJ_ARGONAUT_POST_PROCESSING)
	@rm -f $(TARGET)$(EXTENSION)
	@echo Done.
	@echo
