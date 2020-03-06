# #############################################################################
#
# project   :  CAN - Controller Area Network.
#
# purpose   :  CAN Interface API, Version 3 (PCANBasic)
#
# copyright :  (C) 2019 by UV Software, Berlin
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

VERSION = 0.3.dev

LIBRARY = libUVCANPCB

INSTALL = /usr/local/lib

TARGET  = $(LIBRARY).$(VERSION).dylib

CFLAGS += -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	-D_PCBUSB_SHARED_LIBRARY \
	-D_CANAPI_SHARED_LIBRARY \
	-D_CANAPI_EXTERN \
	-I$(DRV)

#LIBRARIES = -lPCBUSB
LIBRARIES =

LDFLAGS  += -nostartfiles -dynamiclib

endif

OBJECTS = $(OUTDIR)/can_api.o

CAN_API_DEPS = $(DRV)/can_api.h $(DRV)/can_defs.h $(DRV)/CANAPI_Types.h $(DRV)/build_no.h

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
	$(CP) $(TARGET) $(INSTALL)/$(TARGET)
ifeq ($(current_OS),Darwin)
	$(RM) $(INSTALL)/$(LIBRARY).dylib; ln -s $(INSTALL)/$(TARGET) $(INSTALL)/$(LIBRARY).dylib
endif
ifeq ($(current_OS),Linux)
	$(RM) $(INSTALL)/$(SONAME) ; ln -s $(INSTALL)/$(TARGET) $(INSTALL)/$(SONAME)
	$(RM) $(INSTALL)/$(LIBRARY).so ; ln -s $(INSTALL)/$(SONAME) $(INSTALL)/$(LIBRARY).so
endif

distclean:
	$(RM) $(LIBRARY).$(VERSION).dylib $(OUTDIR)/*.o $(DRV)/*~ *~


$(OUTDIR)/main.o: main.c $(MAIN_DEPS)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUTDIR)/can_api.o: $(DRV)/can_api.c $(CAN_API_DEPS)
	$(CC) $(CFLAGS) -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	@echo "Target '"$@"' successfully build"


# #############################################################################
