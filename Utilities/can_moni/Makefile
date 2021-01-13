#
#	CAN Monior for PEAK PCAN-Basic Interfaces
#
#	Copyright (C) 2007,2012-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
#
#	This program is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program   If not, see <http://www.gnu.org/licenses/>.
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))

HOME_DIR = ../..
MAIN_DIR = ./Sources

SOURCE_DIR = $(HOME_DIR)/Sources
CANAPI_DIR = $(HOME_DIR)/Sources/CANAPI
PCBUSB_DIR = $(HOME_DIR)/Sources/PCAN_Wrapper

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/Timer.o $(OUTDIR)/Message.o $(OUTDIR)/PCAN.o \
	$(OUTDIR)/can_api.o $(OUTDIR)/can_btr.o  $(OUTDIR)/can_msg.o


ifeq ($(current_OS),Darwin) # macOS - libPCAN.dylib

VERSION = 0

TARGET  = can_moni

INSTALL = ~/bin

DEFINES = -DOPTION_CAN_2_0_ONLY=0 \
	-DOPTION_CANAPI_DRIVER=1 \
	-DOPTION_CANAPI_COMPANIONS=1

HEADERS = -I$(SOURCE_DIR) \
	-I$(CANAPI_DIR) \
	-I$(PCBUSB_DIR) \
	-I$(MAIN_DIR)

CFLAGS += -O2 -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LIBRARIES =

LDFLAGS  += -lpthread

endif


CXX = g++
CC = gcc
LD = g++
RM = rm -f
CP = cp -f

OUTDIR = .objects


.PHONY: info outdir


all: info outdir $(TARGET)

info:
	@echo $(CXX)" on "$(current_OS)
	@echo "target: "$(TARGET)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

install:
	$(CP) $(TARGET) $(INSTALL)


$(OUTDIR)/main.o: $(MAIN_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/PCAN.o: $(SOURCE_DIR)/PCAN.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/can_api.o: $(PCBUSB_DIR)/can_api.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/can_btr.o: $(CANAPI_DIR)/can_btr.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/can_msg.o: $(CANAPI_DIR)/can_msg.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/Message.o: $(MAIN_DIR)/Message.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/Timer.o: $(MAIN_DIR)/Timer.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
