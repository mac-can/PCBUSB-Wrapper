//  SPDX-License-Identifier: GPL-2.0-or-later
//
//  CAN-over-Ethernet Server Simulator (RocketCAN)
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
#include <getopt.h>
#include <libgen.h>
#include <inttypes.h>
#include <errno.h>

#if defined(__linux__)
#define PLATFORM  "Linux"
#elif defined(__APPLE__)
#define PLATFORM  "macOS"
#elif defined(__CYGWIN__)
#define PLATFORM  "Cygwin"
#else
#error Platform not supported
#endif
#ifdef _MSC_VER
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif
#define USE_BASENAME  1

static const char* c_szApplication = CAN_SIMU_APPLICATION;
static const char* c_szCopyright = CAN_SIMU_COPYRIGHT;
static const char* c_szWarranty = CAN_SIMU_WARRANTY;
static const char* c_szLicense = CAN_SIMU_LICENSE;
static const char* c_szBasename = CAN_SIMU_PROGRAM;
static const char* c_szServerPort = "0";
static const char* c_szAcceppted = "I ACCEPT";

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
    int opt;
    int64_t intarg;

    int optSecurityRisks = 0;
    int optLogginglevel = 0;

    // command-line options
    int show_version = 0;
    struct option long_options[] = {
        {"logging", required_argument, 0, 'g'},
        {"security-risks", required_argument, 0, 'G'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, &show_version, 1},
        {0, 0, 0, 0}
    };
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
    while ((opt = getopt_long(argc, (char * const *)argv, "h", long_options, NULL)) != -1) {
        switch (opt) {
        /* option '--logging=<level>' */
        case 'g':
            if (optLogginglevel++) {
                fprintf(err, "%s: duplicated option `--logging'\n", m_szBasename);
                return 1;
            }
            if (optarg == NULL) {
                fprintf(err, "%s: missing argument for option `--logging'\n", m_szBasename);
                return 1;
            }
            if (sscanf(optarg, "%" SCNi64, &intarg) != 1) {
                fprintf(err, "%s: illegal argument for option `--logging'\n", m_szBasename);
                return 1;
            }
            if ((intarg < 0) || (intarg > INT_MAX)) {
                fprintf(err, "%s: illegal argument for option `--logging'\n", m_szBasename);
                return 1;
            }
            m_nLoggingLevel = (int)intarg;
            break;
        /* option '--security-risks="I ACCEPT" */
        case 'G':
            if (optSecurityRisks++) {
                fprintf(err, "%s: duplicated option `--security-risks'\n", m_szBasename);
                return 1;
            }
            if (optarg == NULL) {
                fprintf(err, "%s: missing argument for option `--security-risks'\n", m_szBasename);
                return 1;
            } 
            m_fRisksAccepted = (strcasecmp(optarg, c_szAcceppted) == 0) ? true : false;
            break;
        /* option '--help' (-h) */
        case 'h':
            ShowHelp(out);
            m_fExit = true;
            return 1;
        case '?':
            if (!opterr)
                ShowUsage(out);
            m_fExit = true;
            return 1;
        default:
            if (show_version)
                ShowVersion(out);
            else
                ShowUsage(out);
            m_fExit = true;
            return 1;
        }
    }
    // (3) scan command-line for argument <port>
    // - check if one and only one <port> is given
    if (optind + 1 != argc) {
        if (optind != argc) {
            fprintf(err, "%s: too many arguments given\n", m_szBasename);
            return 1;
        } else if (!m_fExit) {
            fprintf(err, "%s: no port given\n", m_szBasename);
            return 1;
        } else {
            // no port given, but --version or --help
            return 0;
        }
    } else {
        m_szServerPort = (char*)argv[optind];
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
    fprintf(stream, "     --security-risks=\"I ACCEPT\"      accept security risks (skip interactive input)\n");
    fprintf(stream, "     --logging=<level>                set logging level (default=0)\n");
    fprintf(stream, " -h, --help                           display this help screen and exit\n");
    fprintf(stream, "     --version                        show version information and exit\n");
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
