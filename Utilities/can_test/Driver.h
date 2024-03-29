//  SPDX-License-Identifier: GPL-3.0-or-later
//
//  CAN Tester for generic Interfaces (CAN API V3)
//
//  Copyright (c) 2008-2010,2014-2023 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED
#include "build_no.h"
#define VERSION_MAJOR      0
#define VERSION_MINOR      2
#define VERSION_PATCH      7
#define VERSION_BUILD      BUILD_NO
#define VERSION_STRING     TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH) " (" TOSTRING(BUILD_NO) ")"
#if defined(_WIN64)
#define PLATFORM          "x64"
#elif defined(_WIN32)
#define PLATFORM          "x86"
#elif defined(__linux__)
#define PLATFORM          "Linux"
#elif defined(__APPLE__)
#define PLATFORM          "macOS"
#elif defined(__CYGWIN__)
#define PLATFORM          "Cygwin"
#else
#error Unsupported architecture
#endif
#if (OPTION_CAN_2_0_ONLY != 0)
#error Compilation with legacy CAN 2.0 frame format!
#else
#define CAN_FD_SUPPORTED   1  // don't touch that dial
#endif
#define TESTER_INTEFACE   "PEAK-System PCAN-USB Interfaces"
#define TESTER_COPYRIGHT  "2008-2010,2014-2023 by Uwe Vogt, UV Software, Berlin"

#include "PeakCAN.h"

typedef CPeakCAN  CCanDriver;

#endif // DRIVER_H_INCLUDED
