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
# 25/09/13 14:43
#

bold=`tput bold`
normal=`tput sgr0`

.PHONY: header footer argonaut chameleon

all: header argonaut chameleon footer

header:
	@echo ${bold}Compiling sharc kernels${normal} ...

footer:
	@echo Build completed successfully.
	@echo

argonaut:
	@cd ./argonaut/ && $(MAKE)

chameleon:
	@cd ./chameleon/ && $(MAKE)

clean:
	@cd ./argonaut/ && $(MAKE) clean
	@cd ./chameleon/ && $(MAKE) clean
