# #############################################################################
#
# project   :  CAN - Controller Area Network.
#
# purpose   :  CAN API V3 Tester (PCANBasic)
#
# copyright :  (C) 2012 by UV Software, Friedrichshafen
#              (C) 2013-2021 by UV Software, Berlin
#
# compiler  :  Apple clang version 12.0.0 (clang-1200.0.32.28)
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

HOME_DIR = ..
MAIN_DIR = ./Sources

SOURCE_DIR = $(HOME_DIR)/Sources
CANAPI_DIR = $(HOME_DIR)/Sources/CANAPI

ifeq ($(current_OS),Darwin) # macOS - libUVCANPCB.dylib - - - - - - - - - - - -

VERSION = 0

INSTALL = ~/bin

TARGET	= PCBUSB

CFLAGS += -O2 -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	-DOPTION_CAN_2_0_ONLY=0 \
	-DOPTION_CANAPI_DRIVER=1 \
	-I$(SOURCE_DIR) \
	-I$(CANAPI_DIR) \
	-I$(MAIN_DIR)

#LIBRARIES = -lPCBUSB
LIBRARIES =

LDFLAGS  +=

endif

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/printmsg.o \
	$(OUTDIR)/can_api.o

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
	$(CP) $(TARGET) $(INSTALL)

distclean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d


$(OUTDIR)/main.o: $(MAIN_DIR)/main.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/printmsg.o: $(MAIN_DIR)/printmsg.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/can_api.o: $(SOURCE_DIR)/can_api.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	@echo "Target '"$@"' successfully build"


# #############################################################################
