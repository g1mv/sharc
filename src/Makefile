#
# Copyright (c) 2013, Guillaume Voirin
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Centaurean nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Centaurean BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Sharc
# www.centaurean.com
#
# 01/06/13 17:27
#


# Build options
OUTPUTNAME = sharc

ifndef ARCH
#make ARCH=32
#make ARCH=64
ARCH=64
endif

ifndef OPTIM
# gcc -O option
OPTIM=3
endif

#windows
ifdef SystemRoot
RM = del 
CP = copy /y

EXT = .exe
EXEC=
else
#linux
RM = rm -f
CP = cp -f

EXT=
EXEC=./
endif

CFLAGS = -O$(OPTIM) -std=gnu99 -m$(ARCH) -Winline
CC = gcc $(CFLAGS) $^ -o $@

SRC = sharc.c

TARGET = $(OUTPUTNAME)_$(ARCH)_$(OPTIM)$(EXT)

.DELETE_ON_ERROR:

#Receipes

all :

# main full described name target
$(TARGET) : $(SRC)
	$(CC)

# short target name
$(OUTPUTNAME)$(EXT) : $(TARGET) 
	$(CP) $< $@

all :  $(OUTPUTNAME)$(EXT) 

clean :
	-$(RM) $(TARGET) $(OUTPUTNAME)$(EXT)

unittest : $(TARGET) 
	$(EXEC)$(TARGET) $(TEST)

test:
	$(MAKE) unittest ARCH=32 OPTIM=1 TEST=$(TEST)
	$(MAKE) unittest ARCH=32 OPTIM=2 TEST=$(TEST)
	$(MAKE) unittest ARCH=32 OPTIM=3 TEST=$(TEST)
	$(MAKE) unittest ARCH=64 OPTIM=1 TEST=$(TEST)
	$(MAKE) unittest ARCH=64 OPTIM=2 TEST=$(TEST)
	$(MAKE) unittest ARCH=64 OPTIM=3 TEST=$(TEST)

cleantest :
	-$(RM) sharc sharc_32_1 sharc_32_2 sharc_32_3 sharc_64_1 sharc_64_2 sharc_64_3

