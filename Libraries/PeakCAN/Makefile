#	SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
#
#	CAN Interface API, Version 3 (for PEAK-System PCAN Interfaces)
#
#	Copyright (c) 2005-2012 Uwe Vogt, UV Software, Friedrichshafen
#	Copyright (c) 2013-2024 Uwe Vogt, UV Software, Berlin (info@uv-software.de.com)
#	All rights reserved.
#
#	This file is part of PCANBasic-Wrapper.
#
#	PCANBasic-Wrapper is dual-licensed under the BSD 2-Clause "Simplified" License
#	and under the GNU General Public License v3.0 (or any later version). You can
#	choose between one of them if you use PCANBasic-Wrapper in whole or in part.
#
#	BSD 2-Clause "Simplified" License:
#	Redistribution and use in source and binary forms, with or without
#	modification, are permitted provided that the following conditions are met:
#	1. Redistributions of source code must retain the above copyright notice, this
#	   list of conditions and the following disclaimer.
#	2. Redistributions in binary form must reproduce the above copyright notice,
#	   this list of conditions and the following disclaimer in the documentation
#	   and/or other materials provided with the distribution.
#
#	PCANBasic-Wrapper IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#	OF PCANBasic-Wrapper, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#	GNU General Public License v3.0 or later:
#	PCANBasic-Wrapper is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	PCANBasic-Wrapper is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with PCANBasic-Wrapper.  If not, see <https://www.gnu.org/licenses/>.
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))

MAJOR = 0
MINOR = 2
PATCH = 8

ifeq ($(PATCH),0)
VERSION = $(MAJOR).$(MINOR)
else
VERSION = $(MAJOR).$(MINOR).$(PATCH)
endif

HOME_DIR = ../..

SOURCE_DIR = $(HOME_DIR)/Sources
CANAPI_DIR = $(HOME_DIR)/Sources/CANAPI
PCBUSB_DIR = $(HOME_DIR)/Sources/PCANBasic
WRAPPER_DIR = $(HOME_DIR)/Sources/Wrapper

OBJECTS = $(OUTDIR)/PeakCAN.o $(OUTDIR)/can_api.o $(OUTDIR)/can_btr.o

DEFINES = -DOPTION_CAN_2_0_ONLY=0 \
	-DOPTION_CANAPI_DRIVER=1 \
	-DOPTION_CANAPI_RETVALS=0 \
	-DOPTION_CANAPI_COMPANIONS=1

HEADERS = -I$(SOURCE_DIR) \
	-I$(CANAPI_DIR) \
	-I$(PCBUSB_DIR)/macOS \
	-I$(WRAPPER_DIR)


ifeq ($(current_OS),Darwin)  # macOS - libPeakCAN.dylib

LIBRARY = libPeakCAN

TARGET  = $(LIBRARY).$(VERSION).dylib
STATIC  = $(LIBRARY).a

INSTALL_NAME = $(TARGET)
CURRENT_VERSION = $(MAJOR).$(MINOR).$(PATCH)
COMPATIBILITY_VERSION = $(MAJOR).$(MINOR).0

INSTALL = /usr/local/lib

DEFINES += -DOPTION_CANAPI_PCBUSB_DYLIB=0 \
	-DOPTION_PEAKCAN_DYLIB=1

CFLAGS += -O2 -Wall -Wextra -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O2 -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LDFLAGS += -nostartfiles -dynamiclib -fvisibility=hidden \
	-Wl,-install_name,@rpath/$(INSTALL_NAME) \
	-Wl,-current_version,$(CURRENT_VERSION) \
	-Wl,-compatibility_version,$(COMPATIBILITY_VERSION) \
	-rpath $(INSTALL)

LTFLAGS += -static

ifeq ($(BINARY),UNIVERSAL)
CFLAGS += -arch arm64 -arch x86_64
CXXFLAGS += -arch arm64 -arch x86_64
LDFLAGS += -arch arm64 -arch x86_64
endif

LIBRARIES = -lpthread

CXX = clang++
CC = clang
LD = clang++
LT = libtool
endif


ifeq ($(current_OS),Linux)  # Linux - libpeakcan.so

LIBRARY = libpeakcan

SONAME  = $(LIBRARY).so
TARGET  = $(SONAME).$(VERSION)
STATIC  = $(LIBRARY).a

INSTALL = /usr/local/lib

DEFINES += -DOPTION_CANAPI_PCANBASIC_SO=0 \
	-DOPTION_PEAKCAN_SO=1

CFLAGS += -fPIC -O2 -Wall -Wextra -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -fPIC -O2 -g -Wall -Wextra -pthread\
	$(DEFINES) \
	$(HEADERS)

LDFLAGS  += --shared -Wl,-soname,$(SONAME) -lc

ARFLAGS = r

LIBRARIES = -lc -lpthread

CXX = g++
CC = gcc
LD = g++
AR = ar
endif


RM = rm -f
CP = cp -f
LN = ln -s

OUTDIR = .objects
BINDIR = $(HOME_DIR)/Binaries
INCDIR = $(HOME_DIR)/Includes

.PHONY: info outdir bindir incdir


all: info outdir bindir incdir $(STATIC) $(TARGET)

info:
	@echo $(CXX)" on "$(current_OS)
	@echo "target: "$(TARGET)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

bindir:
	@mkdir -p $(BINDIR)

incdir:
	@mkdir -p $(INCDIR)
	@cp $(SOURCE_DIR)/PeakCAN.h $(INCDIR)
	@cp $(SOURCE_DIR)/PeakCAN_Defines.h $(INCDIR)
	@cp $(SOURCE_DIR)/PeakCAN_Defaults.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANAPI_Types.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANAPI_Defines.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANBTR_Defaults.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANAPI.h $(INCDIR)

clean:
	$(RM) $(TARGET) $(STATIC) $(OUTDIR)/*.o $(OUTDIR)/*.d

pristine:
ifeq ($(current_OS),Darwin)
	$(RM) *.dylib *.a $(OUTDIR)/*.o $(OUTDIR)/*.d
	$(RM) $(BINDIR)/*.dylib $(BINDIR)/*.a
endif
ifeq ($(current_OS),Linux)
	$(RM) *.so.* *.a $(OUTDIR)/*.o $(OUTDIR)/*.d
	$(RM) $(BINDIR)/*.so.* $(BINDIR)/*.a
endif
	$(RM) $(INCDIR)/*.h

install:
	@echo "Copying library file..."
	$(RM) $(INSTALL)/$(TARGET); $(CP) $(TARGET) $(INSTALL)
ifeq ($(current_OS),Darwin)
	$(RM) $(INSTALL)/$(LIBRARY).dylib ; $(LN) $(INSTALL)/$(TARGET) $(INSTALL)/$(LIBRARY).dylib
endif
ifeq ($(current_OS),Linux)
	$(RM) $(INSTALL)/$(SONAME) ; ln -s $(INSTALL)/$(TARGET) $(INSTALL)/$(SONAME)
	$(RM) $(INSTALL)/$(LIBRARY).so ; ln -s $(INSTALL)/$(SONAME) $(INSTALL)/$(LIBRARY).so
endif


$(OUTDIR)/can_api.o: $(WRAPPER_DIR)/can_api.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/can_btr.o: $(CANAPI_DIR)/can_btr.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/PeakCAN.o: $(SOURCE_DIR)/PeakCAN.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<


$(STATIC): $(OBJECTS)
ifeq ($(current_OS),Darwin)
	$(LT) $(LTFLAGS) -o $@ $(OBJECTS)
	@lipo -archs $@
endif
ifeq ($(current_OS),Linux)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)
endif
	$(CP) $@ $(BINDIR)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
ifeq ($(current_OS),Darwin)
	@lipo -archs $@
endif
	$(CP) $@ $(BINDIR)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
