### macOS&reg; Wrapper Library for PCAN Interfaces from PEAK-System

_Copyright&copy; 2005-2010, 2012-2025  Uwe Vogt, UV Software, Berlin (info@mac-can.com)_

![macOS Build](https://github.com/mac-can/PCBUSB-Wrapper/actions/workflows/macos-build.yml/badge.svg)

# CAN API V3 for PCAN USB Interfaces

CAN API V3 is a wrapper specification to have a uniform CAN Interface API for various CAN interfaces from different vendors running under multiple operating systems.

## PCBUSB-Wrapper

This repo contains the source code for a CAN API V3 compatible wrapper library under macOS for PCAN USB interfaces from PEAK-System Technik GmbH.
The wrapper library is build upon the PCBUSB library from UV&nbsp;Software.

The [PCBUSB](https://www.mac-can.com) library is a user-space driver for PCAN USB interfaces under macOS, and is almost compatible to PEAK´s PCANBasic DLL.

Note: _The PCBUSB library is not included in this repo, and must be installed separately!_

### CAN Interface API, Version 3

```C++
/// \name   PeakCAN API
/// \brief  CAN API V3 wrapper for PEAK-System PCAN USB interfaces
/// \note   See CCanApi for a description of the overridden methods
/// \{
class CPeakCAN : public CCanApi {
public:
    // constructor / destructor
    CPeakCAN();
    ~CPeakCAN();

    // CCanApi overrides
    static bool GetFirstChannel(SChannelInfo &info, void *param = NULL);
    static bool GetNextChannel(SChannelInfo &info, void *param = NULL);

    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param, EChannelState &state);
    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, EChannelState &state);

    CANAPI_Return_t InitializeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param = NULL);
    CANAPI_Return_t TeardownChannel();
    CANAPI_Return_t SignalChannel();

    CANAPI_Return_t StartController(CANAPI_Bitrate_t bitrate);
    CANAPI_Return_t ResetController();

    CANAPI_Return_t WriteMessage(CANAPI_Message_t message, uint16_t timeout = 0U);
    CANAPI_Return_t ReadMessage(CANAPI_Message_t &message, uint16_t timeout = CANWAIT_INFINITE);

    CANAPI_Return_t GetStatus(CANAPI_Status_t &status);
    CANAPI_Return_t GetBusLoad(uint8_t &load);

    CANAPI_Return_t GetBitrate(CANAPI_Bitrate_t &bitrate);
    CANAPI_Return_t GetBusSpeed(CANAPI_BusSpeed_t &speed);

    CANAPI_Return_t GetProperty(uint16_t param, void *value, uint32_t nbyte);
    CANAPI_Return_t SetProperty(uint16_t param, const void *value, uint32_t nbyte);

    CANAPI_Return_t SetFilter11Bit(uint32_t code, uint32_t mask);
    CANAPI_Return_t GetFilter11Bit(uint32_t &code, uint32_t &mask);
    CANAPI_Return_t SetFilter29Bit(uint32_t code, uint32_t mask);
    CANAPI_Return_t GetFilter29Bit(uint32_t &code, uint32_t &mask);
    CANAPI_Return_t ResetFilters();

    char *GetHardwareVersion();  // (for compatibility reasons)
    char *GetFirmwareVersion();  // (for compatibility reasons)
    static char *GetVersion();  // (for compatibility reasons)

    static CANAPI_Return_t MapIndex2Bitrate(int32_t index, CANAPI_Bitrate_t &bitrate);
    static CANAPI_Return_t MapString2Bitrate(const char *string, CANAPI_Bitrate_t &bitrate);
    static CANAPI_Return_t MapBitrate2String(CANAPI_Bitrate_t bitrate, char *string, size_t length);
    static CANAPI_Return_t MapBitrate2Speed(CANAPI_Bitrate_t bitrate, CANAPI_BusSpeed_t &speed);

    static uint8_t Dlc2Len(uint8_t dlc) { return CCanApi::Dlc2Len(dlc); }
    static uint8_t Len2Dlc(uint8_t len) { return CCanApi::Len2Dlc(len); }
};
/// \}
```

### Build Targets

_Important note_: To build any of the following build targets run the script `build_no.sh` to generate a pseudo build number.
```
uv-pc013mac:~ eris$ cd ~/Projects/CAN/PCBUSB-Wrapper
uv-pc013mac:PeakCAN eris$ ./build_no.sh
```
Repeat this step after each `git commit`, `git pull`, `git clone`, etc.

Then you can build all targets by typing the usual commands:
```
uv-pc013mac:~ eris$ cd ~/Projects/CAN/PCBUSB-Wrapper
uv-pc013mac:PeakCAN eris$ make clean
uv-pc013mac:PeakCAN eris$ make all
uv-pc013mac:PeakCAN eris$ sudo make install
uv-pc013mac:PeakCAN eris$
```
_(The version number of the libraries can be adapted by editing the appropriated `Makefile`s and changing the variable `VERSION` accordingly.  Don´t forget to set the version number also in the header file `Version.h`.)_

#### Libraries

##### libPeakCAN

___libPeakCAN___ is a dynamic library with a CAN API V3 compatible application programming interface for use in __C++__ applications.
See header file `PeakCAN.h` for a description of all class members.

##### libUVCANPCB

___libUVCANPCB___ is a dynamic library with a CAN API V3 compatible application programming interface for use in __C__ applications.
See header file `can_api.h` for a description of all API functions.

#### Utilities

##### can_send

`can_send` is a command line tool to send CAN messages that are entered at the program prompt.
The syntax is taken from the Linux SocketCAN utility [can_utils\cansend](https://github.com/linux-can/can-utils/tree/master).

Type `can_send --help` to display all program options.

##### can_moni

`can_moni` is a command line tool to view incoming CAN messages.
I hate this messing around with binary masks for identifier filtering.
So I wrote this little program to have an exclude list for single identifiers or identifier ranges (see program option `--exclude` or just `-x`). Precede the list with a `~` and you get an include list.

Type `can_moni --help` to display all program options.

##### can_test

`can_test` is a command line tool to test CAN communication.
Originally developed for electronic environmental tests on an embedded Linux system with SocketCAN, I´m using it for many years as a traffic generator for CAN stress-tests.

Type `can_test --help` to display all program options.

##### can_port

`can_port` is a command line tool designed to open a network socket for CAN-over-Ethernet communication.
It connects to a CAN device and handles the data exchange between the CAN bus and local or remote clients.

_Be aware that this may expose your computer to security vulnerabilities, unauthorized access, data interception, denial of service attacks, and resource exhaustion.
Implement appropriate security measures to mitigate these risks._

Type `can_port --help` to display all program options.

### Target Platforms
- macOS 13.0 and later (Intel x64 and Apple silicon)
- Debian GNU/Linux 12 (Linux Kernel 6.x)

### Development Environments

#### macOS Sequoia
- macOS Sequoia (15.3.1) on a Mac mini (M4 Pro, 2024)
- Apple clang version 16.0.0 (clang-1600.0.26.6)

#### macOS Ventura
- macOS Ventura (13.7.4) on a MacBook Pro (2019)
- Apple clang version 14.0.3 (clang-1403.0.22.14.1)

#### Debian 12.9 ("bookworm")
- Debian 6.1.128-1 (2025-02-07) x86_64 GNU/Linux
- gcc (Debian 12.2.0-14) 12.2.0

### Required Library

#### macOS
- `libPCBUSB.x.y.dylib` - Version 0.13 or later _(Latest is Greatest!)_

#### Linux
- `libpcanbasic.so` - PCAN Driver and Library for Linux, Version 8.20

## Known Bugs and Caveats

- For a list of known bugs and caveats see tab [Issues](https://github.com/mac-can/PCBUSB-Wrapper/issues) in the GitHub repo.
- For a list of known bugs and caveats in the underlying PCBUSB library read the documentation of the appropriated library version.
- PCAN-USB Pro FD devices are supported since version 0.10 of the PCBUSB library, _but only the first channel_ (CAN1).

## This and That

### PCBUSB Library for macOS&reg;

The PCBUSB library can be downloaded form the [MacCAN](https://mac-can.com/) website (binaries only). \
Please note the copyright and license agreements.

### PCAN Linux&reg; Driver and Library

The PCAN Linux (chardev) driver and library can be downloaded form [Peak's](https://www.peak-system.com/fileadmin/media/linux/index.htm/) website.

### PCAN Wrapper Library for Windows&reg;

A CAN API V3 compatible Wrapper Library for Windows is also available.
It is build upon Peak´s PCANBasic DLL and can be downloaded from / cloned at [GitHub](https://github.com/uv-software/PCANBasic-Wrapper).

### CAN API V3 Reference

A generic documentation of the CAN API V3 application programming interface can be found [here](https://uv-software.github.io/CANAPI-Docs/#/).

### Dual-License

Except where otherwise noted, this work is dual-licensed under the terms of the BSD 2-Clause "Simplified" License
and under the terms of the GNU General Public License v2.0 (or any later version).
You can choose between one of them if you use these portions of this work in whole or in part.

### Trademarks

Mac and macOS are trademarks of Apple Inc., registered in the U.S. and other countries and regions. \
PCAN is a registered trademark of PEAK-System Technik GmbH, Darmstadt, Germany. \
POSIX is a registered trademark of the Institute of Electrical and Electronic Engineers, Inc. \
GNU C/C++ is a registered trademark of Free Software Foundation, Inc. \
Linux is a registered trademark of Linus Torvalds. \
Windows is a registered trademark of Microsoft Corporation in the United States and/or other countries. \
All other company, product and service names mentioned herein may be trademarks, registered trademarks, or service marks of their respective owners.

### Hazard Note

_If you connect your CAN device to a real CAN network when using this library, you might damage your application._

### Contact

E-Mail: mailto://info@mac.can.com \
Internet: https://www.mac-can.net
