//  SPDX-License-Identifier: GPL-2.0-or-later
//
//  CAN-over-Ethernet Server Simulator (RocketCAN)
//
//  Copyright (c) 2008 by Uwe Vogt, UV Software, Friedrichshafen
//  Copyright (c) 2025 by Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, see <https://www.gnu.org/licenses/>.
//
#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED

#include "CANAPI.h"

#if (OPTION_CAN_2_0_ONLY != 0)
#error Compilation with legacy CAN 2.0 frame format!
#else
#define CAN_FD_SUPPORTED    1  // don't touch that dial
#define CAN_TRACE_SUPPORTED 0  // write trace file (1=PCAN)
#endif
#define SERVER_INTERFACE  "Generic CAN Interfaces"
#define SERVER_COPYRIGHT  "2008,2025 by Uwe Vogt, UV Software, Berlin"
#if defined(_WIN32) || defined(_WIN64)
#define SERVER_PLATFORM   "Windows"
#elif defined(__linux__)
#define SERVER_PLATFORM   "Linux"
#elif defined(__APPLE__)
#define SERVER_PLATFORM   "Darwin"
#endif
#define SERVER_ALIASNAME  "PCB:"

#endif // DRIVER_H_INCLUDED
