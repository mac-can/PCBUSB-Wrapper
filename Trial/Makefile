#
#	Trial Program
#	PCBUSB-Wrapper
#	Bart Simpson didn´t do it
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))


TARGET	= pcb_test

INSTALL = ~/bin

HOME_DIR = ..
MAIN_DIR = ./Sources

SOURCE_DIR = $(HOME_DIR)/Sources
CANAPI_DIR = $(HOME_DIR)/Sources/CANAPI
PCBUSB_DIR = $(HOME_DIR)/Sources/PCANBasic
WRAPPER_DIR = $(HOME_DIR)/Sources/Wrapper

OBJECTS = $(OUTDIR)/can_api.o $(OUTDIR)/can_btr.o \
	$(OUTDIR)/PeakCAN.o $(OUTDIR)/main.o


DEFINES = -DOPTION_CAN_2_0_ONLY=0 \
	-DOPTION_CANAPI_DRIVER=1 \
	-DOPTION_CANAPI_RETVALS=0 \
	-DOPTION_CANAPI_COMPANIONS=1 \
	-DOPTION_CANBTR_PEAK_FREQUENCIES=1

HEADERS = -I$(SOURCE_DIR) \
	-I$(CANAPI_DIR) \
	-I$(WRAPPER_DIR) \
	-I$(MAIN_DIR)


ifeq ($(current_OS),Darwin)  # macOS - libPeakCAN.dylib

OBJECTS += $(OUTDIR)/PCBUSB.o

HEADERS += -I$(PCBUSB_DIR)/macOS

CFLAGS += -O0 -g -Wall -Wextra -Wno-parentheses \
	-fmessage-length=0 -fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O0 -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LDFLAGS  += -rpath /usr/local/lib

ifeq ($(BINARY),UNIVERSAL)
CFLAGS += -arch arm64 -arch x86_64
CXXFLAGS += -arch arm64 -arch x86_64
LDFLAGS += -arch arm64 -arch x86_64
endif

LIBRARIES = -lpthread

CHECKER  = warning,information
IGNORE   = -i can_msg.c -i can_dev.c
ifeq ($(HUNTER),BUGS)
CHECKER += --bug-hunting
endif

ifeq ($(XCTEST),FULL)
TESTING =
else
TESTING = -only-testing Testing/Testing/testSmokeTest
endif

CXX = clang++
CC = clang
LD = clang++
endif

ifeq ($(current_OS),Linux)  # Linux - libpeakcan.so

HEADERS += -I$(PCBUSB_DIR)/Linux

CFLAGS += -O0 -g -Wall -Wextra -Wno-parentheses \
	-fmessage-length=0 -fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O0 -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LIBRARIES =

LDFLAGS  += -lpthread

CXX = g++
CC = gcc
LD = g++
endif

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

check:
	cppcheck --enable=$(CHECKER) --suppressions-list=$(HOME_DIR)/suppress.txt \
	-D__APPLE__ $(DEFINES) $(HEADERS) $(IGNORE) $(SOURCE_DIR)

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

pristine:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

install:
	$(CP) $(TARGET) $(INSTALL)

test: info outdir $(TARGET)
	./$(TARGET) INFO EXIT

xctest:
	xcodebuild clean build test -project PCBUSB.xcodeproj -scheme Testing $(TESTING)


ifeq ($(current_OS),Darwin)
$(OUTDIR)/PCBUSB.o: $(PCBUSB_DIR)/macOS/PCBUSB.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<
endif
$(OUTDIR)/can_api.o: $(WRAPPER_DIR)/can_api.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/can_btr.o: $(CANAPI_DIR)/can_btr.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/PeakCAN.o: $(SOURCE_DIR)/PeakCAN.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/main.o: $(MAIN_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
ifeq ($(current_OS),Darwin)
	@lipo -archs $@
endif
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
