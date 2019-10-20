# #############################################################################
#
# project   :  CAN - Controller Area Network.
#
# purpose   :  CAN API V3 Tester (PCANBasic)
#
# copyright :  (C) 2012 by UV Software, Friedrichshafen
#              (C) 2013-2019 by UV Software, Berlin
#
# compiler  :  Apple LLVM version 10.0.0 (clang-1000.11.45.5)
#
# author    :  Uwe Vogt, UV Software
#
# e-mail    :  uwe.vogt@uv-software.de
#
# #############################################################################

current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))

# #############################################################################

DRV	= ../api

ifeq ($(current_OS),Darwin) # MacOS - libUVCANPCB.dylib - - - - - - - - - - - -

VERSION = 0

INSTALL = ~/bin

TARGET	= pcb_test

CFLAGS += -O2 -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	-D_CANAPI_EXTERN \
	-I$(DRV)

LIBRARIES = -lPCBUSB

LDFLAGS  +=

endif

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/can_api.o $(OUTDIR)/bitrates.o $(OUTDIR)/printmsg.o

MAIN_DEPS = $(DRV)/can_api.h $(DRV)/can_defs.h bitrates.h printmsg.h

CAN_API_DEPS = $(DRV)/can_api.h $(DRV)/can_defs.h $(DRV)/can_vers.h

BITRATES_DEPS = bitrates.h

PRINTMSG_DEPS = printmsg.h

CC = gcc
LD = gcc
RM = rm -f
CP = cp -f

OUTDIR = .objects


.PHONY: outdir info


all: info outdir $(TARGET)

info:
	@echo $(CC)" on "$(current_OS)
	@echo "Target: "$(TARGET)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o

install:
	$(CP) $(TARGET) $(INSTALL)

distclean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(DRV)/*~ *~


$(OUTDIR)/main.o: main.c $(MAIN_DEPS)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUTDIR)/bitrates.o: bitrates.c $(BITRATES_DEPS)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUTDIR)/printmsg.o: printmsg.c $(PRINTMSG_DEPS)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUTDIR)/can_api.o: $(DRV)/can_api.c $(CAN_API_DEPS)
	$(CC) $(CFLAGS) -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	@echo "Target '"$@"' successfully build"


# #############################################################################
