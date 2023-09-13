OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')

ifeq ($(OS),Darwin)  # macOS - libPeakCAN.dylib
LIBRARY = libPeakCAN.a
CXXFLAGS =
LDFLAGS =
CXX = clang++
LD = clang++
RM = rm -f
endif
ifeq ($(OS),Linux)  # Linux - libpeakcan.so
LIBRARY = libpeakcan.a
CXXFLAGS =
LDFLAGS =
CXX = g++
LD = g++
RM = rm -f
endif
BINARIES = ../../Binaries
INCLUDES = ../../Includes

TARGET1 = can_recv
SOURCE1 = $(TARGET1).cpp $(BINARIES)/$(LIBRARY)
DEFINES1 = -DCHANNEL=0x51 -DBAUDRATE=CANBTR_INDEX_250K

TARGET2 = can_send
SOURCE2 = $(TARGET2).cpp $(BINARIES)/$(LIBRARY)
DEFINES2 = -DCHANNEL=0x52 -DBAUDRATE=CANBTR_INDEX_250K


all: $(TARGET1) $(TARGET2)

$(TARGET1):	$(SOURCE1)
	$(CXX) $(SOURCE1) $(DEFINES1) -I$(INCLUDES) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET1)

$(TARGET2):	$(SOURCE2)
	$(CXX) $(SOURCE2) $(DEFINES2) -I$(INCLUDES) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET2)

clean:
	@-$(RM) *.o *~ $(TARGET1) $(TARGET2)
