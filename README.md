### macOS® Wrapper Library for PCAN-USB Interfaces from Peak-System

_Copyright &copy; 2005-2010, 2012-2021   Uwe Vogt, UV Software, Berlin (info@mac-can.com)_

# CAN API V3 for PCAN-USB Interfaces

CAN API V3 is a wrapper specification to have a uniform CAN Interface API for various CAN interfaces from different vendors running under multiple operating systems.

## PCBUSB-Wrapper

This repo contains the source code for a CAN API V3 compatible wrapper library under macOS for PCAN-USB interfaces from PEAK-System Technik GmbH.
The wrapper library is build upon the PCBUSB library from UV&nbsp;Software.

The [PCBUSB](https://www.mac-can.com) library is a user-space driver for PCAN-USB interfaces under macOS, and is almost compatible to PEAK´s PCANBasic DLL.

Note: _The PCBUSB library is not included in this repo, and must be installed separately!_

### CAN Interface API, Version 3

In case of doubt the source code:

```C++
/// \name   PeakCAN API
/// \brief  CAN API V3 wrapper for PEAK PCAN-USB interfaces
/// \note   See CCANAPI for a description of the overridden methods
/// \{
class CPeakCAN : public CCANAPI {
public:
    // constructor / destructor
    CPeakCAN();
    ~CPeakCAN();

    // CCANAPI overrides
    static CANAPI_Return_t ProbeChannel(int32_t channel, CANAPI_OpMode_t opMode, const void *param, EChannelState &state);
    static CANAPI_Return_t ProbeChannel(int32_t channel, CANAPI_OpMode_t opMode, EChannelState &state);

    CANAPI_Return_t InitializeChannel(int32_t channel, CANAPI_OpMode_t opMode, const void *param = NULL);
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
};
/// \}
```
See header file `PeakCAN.h` for a description of the provided methods.

## Build Targets

Important note: _To build any of the following build targets run the script_ `build_no.sh` _to generate a pseudo build number._
```
uv-pc013mac:~ eris$ cd ~/Projects/CAN/Drivers/PeakCAN
uv-pc013mac:PeakCAN eris$ ./build_no.sh
```
Repeat this step after each `git commit`, `git pull`, `git clone`, etc.

Then go back to the root folder and compile the whole _bleep_ by typing the usual commands:
```
uv-pc013mac:~ eris$ cd ~/Projects/CAN/Drivers/PeakCAN
uv-pc013mac:PeakCAN eris$ make clean
uv-pc013mac:PeakCAN eris$ make all
uv-pc013mac:PeakCAN eris$ sudo make install
```
_(The version number of the libraries can be adapted by editing the `Makefile`s in the subfolders and changing the variable `VERSION` accordingly.  Don´t forget to set the version number also in the source files.)_

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

- Apple´s macOS (x86_64)

### Development Environment

#### macOS Big Sur

- macOS Big Sur (11.4) on a MacBook Pro (2019)
- Apple clang version 12.0.5 (clang-1205.0.22.9)
- Xcode Version 12.5 (12E262)

#### macOS High Sierra

- macOS High Sierra (10.13.6) on a MacBook Pro (late 2011)
- Apple LLVM version 10.0.0 (clang-1000.11.45.5)
- Xcode Version 10.1 (10B61)

### CAN Hardware

- PCAN-USB - single channel, CAN 2.0 (Peak´s item no.: IPEH-002021, IPEH-002021)
- PCAN-USB FD - single channel, CAN 2.0 and CAN FD (Peak´s item no.: IPEH-004022)
- PCAN-USB Pro FD - dual channel, CAN 2.0 and CAN FD (Peak´s item no.: IPEH-004061)

### Required PCBUSB Library

- `libPCBUSB.x.y.dylib` - Version 0.10 or later _(Latest is Greatest!)_

## Known Bugs and Caveats

- For a list of known bugs and caveats see tab [Issues](https://github.com/mac-can/PCBUSB-Wrapper/issues) in the GitHub repo.
- For a list of known bugs and caveats in the underlying PCBUSB library read the documentation of the appropriated library version.
- PCAN-USB Pro FD devices are supported since version 0.10 of the PCBUSB library, _but only the first channel_ (CAN1).

## This and That

The PCBUSB library can be downloaded form the [MacCAN](https://mac-can.com/) website (binaries only). \
Please note the copyright and license agreements.

### Dual-License

This work is dual-licensed under the terms of the BSD 2-Clause "Simplified" License
and under the terms of the GNU General Public License v3.0 (or any later version).
You can choose between one of them if you use this work in whole or in part.

`SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later`

### Trademarks

Mac and macOS are trademarks of Apple Inc., registered in the U.S. and other countries. \
PCAN is a registered trademark of PEAK-System Technik GmbH, Darmstadt, Germany. \
All other company, product and service names mentioned herein are trademarks, registered trademarks or service marks of their respective owners.

### Hazard Note

_If you connect your CAN device to a real CAN network when using this library, you might damage your application._

### Contact

E-Mail: mailto://info@mac.can.com \
Internet: https://www.mac-can.com

##### *Enjoy!*
