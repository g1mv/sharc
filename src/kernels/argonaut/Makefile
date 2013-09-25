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
# 25/09/13 14:37
#

bold=`tput bold`
normal=`tput sgr0`

TARGET_A = libkernel_argonaut_direct.a
TARGET_B = libkernel_argonaut_post_processing.a

CFLAGS = -std=c99 -I. -I../ -I../../

ifeq ($(OS),Windows_NT)
    ARROW = ^-^>
else
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
OBJ_A = $(SRC:.c=_direct.o)
OBJ_B = $(SRC:.c=_post_processing.o)
.PHONY: header footer

all: header_a $(TARGET_A) footer_a header_b $(TARGET_B) footer_b

header_a:
	@echo ${bold}Compiling kernel argonaut direct${normal} ...

footer_a:
	@echo Build completed successfully.
	@echo

header_b:
	@echo ${bold}Compiling kernel argonaut post processing${normal} ...

footer_b:
	@echo Build completed successfully.
	@echo

$(TARGET_A): $(OBJ_A)
	@echo ${bold}Linking${normal} ...
	@echo $^ $(ARROW) ${bold}$(TARGET_A)${normal}
	@ar rcs $(TARGET_A) $(OBJ_A)

$(TARGET_B): $(OBJ_B)
	@echo ${bold}Linking${normal} ...
	@echo $^ $(ARROW) ${bold}$(TARGET_B)${normal}
	@ar rcs $(TARGET_B) $(OBJ_B)

%_direct.o: %.c
	@echo $^ $(ARROW) $@
	$(CC) -c $(CFLAGS) $< -o $@

%_post_processing.o: %.c
	@echo $^ $(ARROW) $@
	$(CC) -c $(CFLAGS) -DSHARC_ARGONAUT_POST_PROCESSING $< -o $@

clean:
	@rm -f $(OBJ_A)
	@rm -f $(OBJ_B)
	@rm -f $(TARGET_A)
	@rm -f $(TARGET_B)
