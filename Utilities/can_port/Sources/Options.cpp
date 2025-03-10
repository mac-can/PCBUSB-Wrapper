//  SPDX-License-Identifier: GPL-2.0-or-later
//
//  CAN-over-Ethernet Server for generic Interfaces (CAN API V3)
//
//  Copyright (c) 2008,2012-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
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
#if defined(_WIN32) || defined(_WIN64)
//  for Windows OS (x86 and x64)
#include "Options_w.cpp"
#else
//  for POSIX OS (Linux, Darwin)
#include "Options_p.cpp"
#endif
