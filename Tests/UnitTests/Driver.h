//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (for PEAK PCAN-USB Interfaces)
//
//  Copyright (c) 2021-2022  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
//  All rights reserved.
//
//  This file is part of PCBUSB-Wrapper.
//
//  PCBUSB-Wrapper is dual-licensed under the BSD 2-Clause "Simplified" License
//  and under the GNU General Public License v3.0 (or any later version). You can
//  choose between one of them if you use PCBUSB-Wrapper in whole or in part.
//
//  BSD 2-Clause "Simplified" License:
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this
//     list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  PCBUSB-Wrapper IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF PCBUSB-Wrapper, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  GNU General Public License v3.0 or later:
//  PCBUSB-Wrapper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  PCBUSB-Wrapper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with PCBUSB-Wrapper.  If not, see <https://www.gnu.org/licenses/>.
//
#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED

//  A Template for CAN API V3 Testing
//
//  (§1) include the header file of the CAN API V3 C++ class of the CAN driver
#include "PeakCAN.h"

//  (§2) define macro CDriverCAN with the class name of the CAN driver
#define CDriverCAN  CPeakCAN

//  (§3) define macro CAN_DEVICE1 and CAN_DEVICE2 with a valid CAN channel no.
#define CAN_DEVICE1  PCAN_USB1
#define CAN_DEVICE2  PCAN_USB2

//  ($4) define macros for unsupported features (at least the mandatory macros, cf. compiler warnings)
#define FEATURE_BITRATE_800K  FEATURE_SUPPORTED
#define FEATURE_BITRATE_SAM   FEATURE_SUPPORTED
#define FEATURE_BITRATE_SJA1000  FEATURE_UNSUPPORTED
#define FEATURE_BITRATE_FD_SAM   FEATURE_UNSUPPORTED
#define FEATURE_BITRATE_FD_SJA1000  FEATURE_UNSUPPORTED
#define FEATURE_WRITE_ACKNOWLEDGED  FEATURE_UNSUPPORTED
#define FEATURE_STATUS_BIT_QUE_OVR  FEATURE_UNSUPPORTED

//  (§5) define macros for CAN Classic bit-rate settings (at least BITRATE_1M, BITRATE_500K, BITRATE_250K, BITRATE_125K, BITRATE_100K, BITRATE_50K, BITRATE_20K, BITRATE_10K)
#define BITRATE_1M(x)    do {x.btr.frequency=8000000;x.btr.nominal.brp=1; x.btr.nominal.tseg1=5; x.btr.nominal.tseg2=2;x.btr.nominal.sjw=1;x.btr.nominal.sam=0;} while(0)
#define BITRATE_800K(x)  do {x.btr.frequency=8000000;x.btr.nominal.brp=1; x.btr.nominal.tseg1=7; x.btr.nominal.tseg2=2;x.btr.nominal.sjw=1;x.btr.nominal.sam=0;} while(0)
#define BITRATE_500K(x)  do {x.btr.frequency=8000000;x.btr.nominal.brp=1; x.btr.nominal.tseg1=13;x.btr.nominal.tseg2=2;x.btr.nominal.sjw=1;x.btr.nominal.sam=0;} while(0)
#define BITRATE_250K(x)  do {x.btr.frequency=8000000;x.btr.nominal.brp=2; x.btr.nominal.tseg1=13;x.btr.nominal.tseg2=2;x.btr.nominal.sjw=1;x.btr.nominal.sam=0;} while(0)
#define BITRATE_125K(x)  do {x.btr.frequency=8000000;x.btr.nominal.brp=4; x.btr.nominal.tseg1=13;x.btr.nominal.tseg2=2;x.btr.nominal.sjw=1;x.btr.nominal.sam=0;} while(0)
#define BITRATE_100K(x)  do {x.btr.frequency=8000000;x.btr.nominal.brp=4; x.btr.nominal.tseg1=16;x.btr.nominal.tseg2=3;x.btr.nominal.sjw=2;x.btr.nominal.sam=0;} while(0)
#define BITRATE_50K(x)   do {x.btr.frequency=8000000;x.btr.nominal.brp=8; x.btr.nominal.tseg1=16;x.btr.nominal.tseg2=3;x.btr.nominal.sjw=2;x.btr.nominal.sam=0;} while(0)
#define BITRATE_20K(x)   do {x.btr.frequency=8000000;x.btr.nominal.brp=20;x.btr.nominal.tseg1=16;x.btr.nominal.tseg2=3;x.btr.nominal.sjw=2;x.btr.nominal.sam=0;} while(0)
#define BITRATE_10K(x)   do {x.btr.frequency=8000000;x.btr.nominal.brp=40;x.btr.nominal.tseg1=16;x.btr.nominal.tseg2=3;x.btr.nominal.sjw=2;x.btr.nominal.sam=0;} while(0)
#define BITRATE_5K(x)    do {x.btr.frequency=8000000;x.btr.nominal.brp=64;x.btr.nominal.tseg1=16;x.btr.nominal.tseg2=8;x.btr.nominal.sjw=2;x.btr.nominal.sam=0;} while(0)

//  (§6) define macros for workarounds in CAN Classic operation mode (e.g. TC01_3_ISSUE)
//#define TC0x_y_ISSUE_  WORKAROUND_ENABLED
//  (§6.1) old PCANBasic issues (see macros in 'Settings.h')
#define PCBUSB_INIT_DELAY_WORKAROUND  WORKAROUND_ENABLED
#define PCBUSB_QXMTFULL_WORKAROUND  WORKAROUND_ENABLED

//  (§7) define macro CAN_FD_SUPPORTED if CAN FD operation mode is supported
#define CAN_FD_SUPPORTED  FEATURE_SUPPORTED

//  (§8) define macros for CAN FD bit-rate settings (at least BITRATE_FD_1M8M, BITRATE_FD_500K4M, BITRATE_FD_250K2M, BITRATE_FD_125K1M, BITRATE_FD_1M, BITRATE_FD_500K, BITRATE_FD_250K, BITRATE_FD_125K)
#define BITRATE_FD_1M(x)      do {x.btr.frequency=80000000;x.btr.nominal.brp=2;x.btr.nominal.tseg1=31; x.btr.nominal.tseg2=8; x.btr.nominal.sjw=8; } while(0)
#define BITRATE_FD_500K(x)    do {x.btr.frequency=80000000;x.btr.nominal.brp=2;x.btr.nominal.tseg1=63; x.btr.nominal.tseg2=16;x.btr.nominal.sjw=16;} while(0)
#define BITRATE_FD_250K(x)    do {x.btr.frequency=80000000;x.btr.nominal.brp=2;x.btr.nominal.tseg1=127;x.btr.nominal.tseg2=32;x.btr.nominal.sjw=32;} while(0)
#define BITRATE_FD_125K(x)    do {x.btr.frequency=80000000;x.btr.nominal.brp=2;x.btr.nominal.tseg1=255;x.btr.nominal.tseg2=64;x.btr.nominal.sjw=64;} while(0)
#define BITRATE_FD_1M8M(x)    do {x.btr.frequency=80000000;x.btr.nominal.brp=2;x.btr.nominal.tseg1=31; x.btr.nominal.tseg2=8; x.btr.nominal.sjw=8;  x.btr.data.brp=2; x.btr.data.tseg1=3;  x.btr.data.tseg2=1; x.btr.data.sjw=1; } while(0)
#define BITRATE_FD_500K4M(x)  do {x.btr.frequency=80000000;x.btr.nominal.brp=2;x.btr.nominal.tseg1=63; x.btr.nominal.tseg2=16;x.btr.nominal.sjw=16; x.btr.data.brp=2; x.btr.data.tseg1=7;  x.btr.data.tseg2=2; x.btr.data.sjw=2; } while(0)
#define BITRATE_FD_250K2M(x)  do {x.btr.frequency=80000000;x.btr.nominal.brp=2;x.btr.nominal.tseg1=127;x.btr.nominal.tseg2=32;x.btr.nominal.sjw=32; x.btr.data.brp=2; x.btr.data.tseg1=15; x.btr.data.tseg2=4; x.btr.data.sjw=4; } while(0)
#define BITRATE_FD_125K1M(x)  do {x.btr.frequency=80000000;x.btr.nominal.brp=2;x.btr.nominal.tseg1=255;x.btr.nominal.tseg2=64;x.btr.nominal.sjw=64; x.btr.data.brp=2; x.btr.data.tseg1=31; x.btr.data.tseg2=8; x.btr.data.sjw=8; } while(0)

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
//  (§9) define macros for workarounds in CAN FD operation mode (e.g. TC01_3_ISSUE)
#define TC03_29_ISSUE_PCBUSB_BR_FD_IN_2_0  WORKAROUND_ENABLED  // only SJA1000 settings allowed in CAN 2.0 (due to PCAN-USB device)
//#define TC0x_y_ISSUE_  WORKAROUND_ENABLED

#endif // CAN_FD_SUPPORTED
#endif // DRIVER_H_INCLUDED

// $Id: Template.h 1073 2022-07-16 13:06:44Z makemake $  Copyright (c) UV Software, Berlin //
