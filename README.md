### macOS&reg; Wrapper Library for PCAN-USB Interfaces from PEAK-System

_Copyright&copy; 2005-2010, 2012-2023  Uwe Vogt, UV Software, Berlin (info@mac-can.com)_

# CAN API V3 for PCAN-USB Interfaces

CAN API V3 is a wrapper specification to have a uniform CAN Interface API for various CAN interfaces from different vendors running under multiple operating systems.

## PCBUSB-Wrapper

This repo contains the source code for a CAN API V3 compatible wrapper library under macOS for PCAN-USB interfaces from PEAK-System Technik GmbH.
The wrapper library is build upon the PCBUSB library from UV&nbsp;Software.

The [PCBUSB](https://www.mac-can.com) library is a user-space driver for PCAN-USB interfaces under macOS, and is almost compatible to PEAK´s PCANBasic DLL.

Note: _The PCBUSB library is not included in this repo, and must be installed separately!_

### CAN Interface API, Version 3

```C++
/// \name   PeakCAN API
/// \brief  CAN API V3 wrapper for PEAK-System PCAN-USB interfaces
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
    CANAPI_Return_t ReadMessage(CANAPI_Message_t &message, uint16_t timeout = CANREAD_INFINITE);

    CANAPI_Return_t GetStatus(CANAPI_Status_t &status);
    CANAPI_Return_t GetBusLoad(uint8_t &load);

    CANAPI_Return_t GetBitrate(CANAPI_Bitrate_t &bitrate);
    CANAPI_Return_t GetBusSpeed(CANAPI_BusSpeed_t &speed);

    CANAPI_Return_t GetProperty(uint16_t param, void *value, uint32_t nbyte);
    CANAPI_Return_t SetProperty(uint16_t param, const void *value, uint32_t nbyte);

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
See header file `PeakCAN.h` for a description of the provided methods.

### Build Targets

_Important note_: To build any of the following build targets run the script `build_no.sh` to generate a pseudo build number.
```
uv-pc013mac:~ eris$ cd ~/Projects/CAN/Drivers/PeakCAN
uv-pc013mac:PeakCAN eris$ ./build_no.sh
```
Repeat this step after each `git commit`, `git pull`, `git clone`, etc.

Then you can build all targets by typing the usual commands:
```
uv-pc013mac:~ eris$ cd ~/Projects/CAN/Drivers/PeakCAN
uv-pc013mac:PeakCAN eris$ make clean
uv-pc013mac:PeakCAN eris$ make all
uv-pc013mac:PeakCAN eris$ sudo make install
uv-pc013mac:PeakCAN eris$
```
_(The version number of the library can be adapted by editing the appropriated `Makefile` and changing the variable `VERSION` accordingly. Don´t forget to set the version number also in the source files.)_

#### libPeakCAN

___libPeakCAN___ is a dynamic library with a CAN API V3 compatible application programming interface for use in __C++__ applications.
See header file `PeakCAN.h` for a description of all class members.

#### libUVCANPCB

___libUVCANPCB___ is a dynamic library with a CAN API V3 compatible application programming interface for use in __C__ applications.
See header file `can_api.h` for a description of all API functions.

#### can_moni

`can_moni` is a command line tool to view incoming CAN messages.
I hate this messing around with binary masks for identifier filtering.
So I wrote this little program to have an exclude list for single identifiers or identifier ranges (see program option `--exclude` or just `-x`). Precede the list with a `~` and you get an include list.

Type `can_moni --help` to display all program options.

#### can_test

`can_test` is a command line tool to test CAN communication.
Originally developed for electronic environmental tests on an embedded Linux system with SocketCAN, I´m using it for many years as a traffic generator for CAN stress-tests.

Type `can_test --help` to display all program options.

### Target Platform

- macOS 11.0 and later (Intel x64 and Apple silicon)

### Development Environment

#### macOS Ventura

- macOS Ventura (13.5.1) on a Mac mini (M1, 2020)
- Apple clang version 14.0.3 (clang-1403.0.22.14.1)
- Xcode Version 14.3.1 (14E300c)

#### macOS Big Sur

- macOS Big Sur (11.7.9) on a MacBook Pro (2019)
- Apple clang version 13.0.0 (clang-1300.0.29.30)
- Xcode Version 13.2.1 (13C100)

#### macOS High Sierra

- macOS High Sierra (10.13.6) on a MacBook Pro (late 2011)
- Apple LLVM version 10.0.0 (clang-1000.11.45.5)
- Xcode Version 10.1 (10B61)

### Required PCBUSB Library

- `libPCBUSB.x.y.dylib` - Version 0.9 or later _(Latest is Greatest!)_

### Testing

The Xcode project for the trial program includes an xctest target with one test suite for each CAN API V3 **C** interface function.
To run the test suites or single test cases two CAN devices are required.
General test settings can be adapted in the file `Settings.h`.

## Known Bugs and Caveats

- For a list of known bugs and caveats see tab [Issues](https://github.com/mac-can/PCBUSB-Wrapper/issues) in the GitHub repo.
- For a list of known bugs and caveats in the underlying PCBUSB library read the documentation of the appropriated library version.
- PCAN-USB Pro FD devices are supported since version 0.10 of the PCBUSB library, _but only the first channel_ (CAN1).

## This and That

### PCBUSB Library for macOS&reg;
The PCBUSB library can be downloaded form the [MacCAN](https://mac-can.com/) website (binaries only). \
Please note the copyright and license agreements.

### Wrapper Library for Windows&reg;

A CAN API V3 compatible Wrapper Library for Windows is also available.
It is build upon Peak´s PCANBasic DLL and can be downloaded from / cloned at [GitHub](https://github.com/uv-software/PCANBasic-Wrapper).

### Dual-License

This work is dual-licensed under the terms of the BSD 2-Clause "Simplified" License
and under the terms of the GNU General Public License v3.0 (or any later version).
You can choose between one of them if you use this work in whole or in part.

`SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later`

### Trademarks

Mac and macOS are trademarks of Apple Inc., registered in the U.S. and other countries. \
PCAN is a registered trademark of PEAK-System Technik GmbH, Darmstadt, Germany. \
Windows is a registered trademark of Microsoft Corporation in the United States and/or other countries. \
Linux is a registered trademark of Linus Torvalds. \
All other company, product and service names mentioned herein may be trademarks, registered trademarks or service marks of their respective owners.

### Hazard Note

_If you connect your CAN device to a real CAN network when using this library, you might damage your application._

### Contact

E-Mail: mailto://info@mac.can.com \
Internet: https://www.mac-can.net
