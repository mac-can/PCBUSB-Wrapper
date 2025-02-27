//  SPDX-License-Identifier: GPL-2.0-or-later
//
//  CAN Sender for generic Interfaces (CAN API V3)
//
//  Copyright (c) 2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
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

#define HELP              32
#define QUESTION_MARK     33
#define ABOUT             34
#define CHARACTER_MJU     35
#define VERSION           36
#define MAX_OPTIONS       37

static char* option[MAX_OPTIONS] = {
    (char*)"HELP", (char*)"?",
    (char*)"ABOUT", (char*)"\xB5",
    (char*)"VERSION"
};
static const char* c_szApplication = CAN_SIMU_APPLICATION;
static const char* c_szCopyright = CAN_SIMU_COPYRIGHT;
static const char* c_szWarranty = CAN_SIMU_WARRANTY;
static const char* c_szLicense = CAN_SIMU_LICENSE;
static const char* c_szBasename = CAN_SIMU_PROGRAM;
static const char* c_szServerPort = "0";
static const char* c_szAcceppted = "I ACCEPT";

#if (USE_BASENAME != 0)
static char* basename(char* path);
#endif

SOptions::SOptions() {
    // initialization
    m_szBasename = (char*)c_szBasename;
    m_szServerPort = (char*)c_szServerPort;
    m_nLoggingLevel = 0;
    m_eSocketType = eIpcTcp;
    m_eDataFormat = eMtuRocketCan;
    m_fRisksAccepted = false;
    m_fExit = false;
}

int SOptions::ScanCommanline(int argc, const char* argv[], FILE* err, FILE* out) {
    int optind;
    char* optarg;
    int64_t intarg;

    int optSecurityRisks = 0;
    int optLogginglevel = 0;
    // (0) sanity check
    if ((argc <= 0) || (argv == NULL))
        return (-1);
    if ((err == NULL) || (out == NULL))
        return (-1);
    // (1) get basename from command-line
#if (USE_BASENAME != 0)
    m_szBasename = basename((char*)argv[0]);
#endif
    // (2) scan command-line for options
    while ((optind = getOption(argc, (char**)argv, MAX_OPTIONS, option)) != EOF) {
        /* option '--help' (-h) */
        case HELP:
        case QUESTION_MARK:
            ShowHelp(out);;
            return 1;
        case ABOUT:
        case VERSION:
        case CHARACTER_MJU:
            ShowVersion(out);;
            return 1;
        default:
            ShowUsage(out);;
            return 1;
        }
    }
    // (3) scan command-line for argument <port>
    for (int i = 1; i < argc; i++) {
        if (!isOption(argc, (char**)argv, MAX_OPTIONS, option, i)) {
            if ((argInterface++)) {
                fprintf(err, "%s: too many arguments\n", m_szBasename);
                return 1;
            }
            m_szServerPort = (char*)argv[i];
        }
    }
    // - check if one and only one <port> is given
    if (!argInterface && !m_fExit) {
        fprintf(err, "%s: no port given\n", m_szBasename);
        return 1;
    }
    return 0;
}

void SOptions::ShowGreetings(FILE* stream) {
    if (!stream)
        return;
    fprintf(stream, "%s\n%s\n\n%s\n\n", c_szApplication, c_szCopyright, c_szWarranty);
}

void SOptions::ShowFarewell(FILE* stream) {
    if (!stream)
        return;
    fprintf(stream, "%s\n", c_szCopyright);
}

void SOptions::ShowVersion(FILE* stream) {
    if (!stream)
        return;
    fprintf(stream, "%s\n%s\n\n%s\n\n", c_szApplication, c_szCopyright, c_szLicense);
    fprintf(stream, "Written by Uwe Vogt, UV Software, Berlin <https://www.uv-software.com/>\n");
}

void SOptions::ShowUsage(FILE* stream, bool args) {
    if(!stream)
        return;
    fprintf(stream, "Usage: %s <port> [<option>...]\n", m_szBasename);
    fprintf(stream, "Options:\n");
    fprintf(stream, "  /HELP | /?                          display this help screen and exit\n");
    fprintf(stream, "  /VERSION                            show version information and exit\n");
    if (args) {
        fprintf(stream, "Arguments:\n");
        fprintf(stream, "  <port>                          CAN-over-Ethernet port\n");
    }
    fprintf(stream, "Syntax:\n");
    fprintf(stream, " <can_frame>:\n");
    fprintf(stream, "  <can_id>#{data}                     for CAN CC data frames\n");
    fprintf(stream, "  <can_id>#R{len}                     for CAN CC remote frames\n");
    //fprintf(stream, "  <can_id>#{data}_{dlc}               for CAN CC data frames with 9..F DLC\n");
    //fprintf(stream, "  <can_id>#R{len}_{dlc}               for CAN CC remote frames with 9..F DLC\n");
    fprintf(stream, "  <can_id>##<flags>{data}             for CAN FD data frames (up to 64 bytes)\n");
    fprintf(stream, " <can_id>:\n");
    fprintf(stream, "  3  ASCII hex-chars (0 .. F) for Standard frame format (SFF) or\n");
    fprintf(stream, "  8  ASCII hex-chars (0 .. F) for eXtended frame format (EFF)\n");
    fprintf(stream, " {data}:\n");
    fprintf(stream, "  0 .. 8   ASCII hex-values in CAN CC mode (optionally separated by '.') or\n");
    fprintf(stream, "  0 .. 64  ASCII hex-values in CAN FD mode (optionally separated by '.')\n");
    fprintf(stream, " {len}:\n");
    fprintf(stream, "  an optional 0 .. 8 value as RTR frames can contain a valid DLC field\n");
    //fprintf(stream, " _{dlc}:\n");
    //fprintf(stream, "  an optional 9..F data length code value when payload length is 8\n");
    fprintf(stream, " <flags>:\n");
    fprintf(stream, "  one ASCII hex-char (0 .. F) which defines CAN FD flags:\n");
    fprintf(stream, "    4 = FDF                           for CAN FD frame format\n");
    fprintf(stream, "    5 = FDF and BRS                   for CAN FD with Bit Rate Switch\n");
    fprintf(stream, "    6 = FDF and ESI                   for CAN FD with Error State Indicator\n");
    fprintf(stream, "    7 = FDF, BRS and ESI              all together now\n");
    fprintf(stream, "Hazard note:\n");
    fprintf(stream, "  If you open a port for socket communication while using this program, it may\n");
    fprintf(stream, "  expose your computer to security vulnerabilities, unauthorized access, data\n");
    fprintf(stream, "  interception, denial of service attacks, and resource exhaustion.\n");
    fprintf(stream, "  It is strongly recommended to implement appropriate security measures to\n");
    fprintf(stream, "  mitigate these risks.\n");
    fprintf(stream, "  If you connect your CAN device to a real CAN network when using this program,\n");
    fprintf(stream, "  you might damage your application.\n");
}

void SOptions::ShowHelp(FILE* stream) {
    ShowGreetings(stream);
    ShowUsage(stream);
}

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
