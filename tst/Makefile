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

DRV	= ../api

TARGET	= pcb_test

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/can_api.o $(OUTDIR)/bitrates.o $(OUTDIR)/printmsg.o

MAIN_DEPS = $(DRV)/can_api.h $(DRV)/can_defs.h bitrates.h printmsg.h

CAN_API_DEPS = $(DRV)/can_api.h $(DRV)/can_defs.h $(DRV)/can_vers.h

BITRATES_DEPS = bitrates.h

PRINTMSG_DEPS = printmsg.h

CFLAGS	= -O2 -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	-D_CANAPI_EXTERN \
	-I$(DRV)

LIBRARIES = -lPCBUSB

LDFLAGS =

CC = gcc
LD = gcc
RM = rm -f
CP = cp -f

OUTDIR = .objects


.PHONY: outdir


all: outdir $(TARGET)

outdir:
	@mkdir -p $(OUTDIR)

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o

install:
	$(CP) $(TARGET) ~/bin

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
