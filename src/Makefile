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

TARGET = sharc

CFLAGS = -O4 -std=gnu99 -D_FILE_OFFSET_BITS=64

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
endif

CC = @$(CROSS-PREFIX)cc

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
.PHONY: header footer

all: header $(TARGET)$(EXTENSION) footer

header:
	@echo Compiling sharc ...
	
footer:
	@echo Build completed successfully.
	
$(TARGET)$(EXTENSION): $(OBJ)
	@echo Linking sharc ...
	@echo $^ $(ARROW) $(TARGET)$(EXTENSION)
	$(CC) -o $(TARGET)$(EXTENSION) $^ $(CFLAGS)
	
%.o: %.c
	@echo $^ $(ARROW) $@
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	@echo Cleaning build directory...
	@rm -f $(OBJ)
	@rm -f $(TARGET)$(EXTENSION)
	@echo Done.
