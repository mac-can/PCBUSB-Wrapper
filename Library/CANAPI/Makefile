#
#	CAN Interface API, Version 3 (for PEAK PCAN-Basic Interfaces)
#
#	Copyright (C) 2010-2021  Uwe Vogt, UV Software, Berlin (info@uv-software.com)
#
#	This library is part of PCANBasic-Wrapper.
#
#	PCANBasic-Wrapper is free software: you can redistribute it and/or modify
#	it under the terms of the GNU Lesser General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	PCANBasic-Wrapper is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU Lesser General Public License for more details.
#
#	You should have received a copy of the GNU Lesser General Public License
#	along with PCANBasic-Wrapper.  If not, see <http://www.gnu.org/licenses/>.
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))

MAJOR = 0
MINOR = 2
PATCH = 0

HOME_DIR = ../..
MAIN_DIR = ./Sources

SOURCE_DIR = $(HOME_DIR)/Sources
CANAPI_DIR = $(HOME_DIR)/Sources/CANAPI
PCBUSB_DIR = $(HOME_DIR)/Sources/PCAN_Wrapper

ifeq ($(current_OS),Darwin) # macOS - libUVCANPCB.dylib

LIBRARY = libUVCANPCB

VERSION = $(MAJOR).$(MINOR)
TARGET  = $(LIBRARY).$(VERSION).dylib

INSTALL_NAME = $(TARGET)
CURRENT_VERSION = $(MAJOR).$(MINOR).0
COMPATIBILITY_VERSION = $(MAJOR).$(MINOR).0

INSTALL = /usr/local/lib

INCLUDE = /usr/local/include

DEFINES = -DOPTION_CAN_2_0_ONLY=0 \
	-DOPTION_CANAPI_PCBUSB_DYLIB=1 \
	-DOPTION_CANAPI_DRIVER=1

HEADERS = -I$(SOURCE_DIR) \
	-I$(CANAPI_DIR) \
	-I$(PCBUSB_DIR)

CFLAGS += -O2 -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LIBRARIES =

LDFLAGS  += -nostartfiles -dynamiclib -fvisibility=hidden \
	-install_name $(INSTALL_NAME) \
	-current_version $(CURRENT_VERSION) \
	-compatibility_version $(COMPATIBILITY_VERSION)

endif

OBJECTS = $(OUTDIR)/can_api.o

CC = gcc
LD = gcc
RM = rm -f
CP = cp -f

OUTDIR = .objects


.PHONY: outdir info


all: info outdir $(TARGET)

info:
	@echo $(CC)" on "$(current_OS)
	@echo "target: "$(TARGET)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

install:
	@echo "Copying library file..."
	$(CP) $(TARGET) $(INSTALL)
ifeq ($(current_OS),Darwin)
	$(RM) $(INSTALL)/$(LIBRARY).dylib ; ln -s $(INSTALL)/$(TARGET) $(INSTALL)/$(LIBRARY).dylib
endif
ifeq ($(current_OS),Linux)
	$(RM) $(INSTALL)/$(SONAME) ; ln -s $(INSTALL)/$(TARGET) $(INSTALL)/$(SONAME)
	$(RM) $(INSTALL)/$(LIBRARY).so ; ln -s $(INSTALL)/$(SONAME) $(INSTALL)/$(LIBRARY).so
endif
	@echo "Copying header files..."
	$(CP) $(CANAPI_DIR)/can_api.h $(INCLUDE)
	$(CP) $(CANAPI_DIR)/CANAPI_Types.h $(INCLUDE)
#	$(CP) $(CANAPI_DIR)/CANAPI_PCANBasic.h $(INCLUDE)


$(OUTDIR)/can_api.o: $(PCBUSB_DIR)/can_api.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	@echo "Target '"$@"' successfully build"
