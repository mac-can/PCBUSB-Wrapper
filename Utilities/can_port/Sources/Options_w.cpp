//  SPDX-License-Identifier: GPL-2.0-or-later
//
//  CAN-to-Ethernet Server for generic Interfaces (CAN API V3)
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
#include "Options.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
extern "C" {
#include "dosopt.h"
}
#if defined(_WIN64)
#define PLATFORM  "x64"
#elif defined(_WIN32)
#define PLATFORM  "x86"
#else
#error Platform not supported
#endif
#ifdef _MSC_VER
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif
#define USE_BASENAME  0

// TODO: insert coin here

#if (USE_BASENAME != 0)
/* see man basename(3) */
static char* basename(char* path) {
    static char exe[] = "agimus.exe";
    char* ptr = NULL;
    if (path)
        ptr = strrchr(path, '\\');
    if (ptr)
        ptr++;
    return (ptr ? ptr : exe);
}
#endif
