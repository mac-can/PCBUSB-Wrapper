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

#define DEFAULT_OP_MODE   CANMODE_DEFAULT
#define DEFAULT_BAUDRATE  CANBTR_INDEX_250K

#define BAUDRATE_STR      0
#define BAUDRATE_CHR      1
#define BITRATE_STR       2
#define BITRATE_CHR       3
#define VERBOSE_STR       4
#define VERBOSE_CHR       5
#define OP_MODE_STR       6
#define OP_MODE_CHR       7
#define OP_ERR_STR        8
#define OP_ERRFRMS_STR    9
#define TRACEFILE_STR     10
#define TRACEFILE_CHR     11
#define LOGGING_STR       12
#define LOGGING_CHR       13
#define SECURITY_RISKS    14
#define LISTBITRATES_STR  15
#define LISTBOARDS_STR    16
#define LISTBOARDS_CHR    17
#define TESTBOARDS_STR    18
#define TESTBOARDS_CHR    19
#define PROTOCOL_STR      20
#define PROTOCOL_CHR      21
#define JSON_STR          22
#define JSON_CHR          23
#define HELP              24
#define QUESTION_MARK     25
#define ABOUT             26
#define CHARACTER_MJU     27
#define VERSION           28
#define MAX_OPTIONS       29

static char* option[MAX_OPTIONS] = {
    (char*)"BAUDRATE", (char*)"bd",
    (char*)"BITRATE", (char*)"br",
    (char*)"VERBOSE", (char*)"v",
    (char*)"MODE", (char*)"m",
    (char*)"ERR", (char*)"ERROR-FRAMES",
    (char*)"TRACE", (char*)"trc",
    (char*)"LOGGING", (char*)"log",
    (char*)"SECURITY-RISKS",
    (char*)"LIST-BITRATES",
    (char*)"LIST-BOARDS", (char*)"list",
    (char*)"TEST-BOARDS", (char*)"test",
    (char*)"PROTOCOL", (char*)"pr",
#if (OPTION_CANAPI_LIBRARY != 0)
    (char*)"PATH", (char*)"p",
#else
    (char*)"JSON-FILE", (char*)"json",
#endif
    (char*)"HELP", (char*)"?",
    (char*)"ABOUT", (char*)"\xB5",
    (char*)"VERSION"
};
static const char* c_szApplication = CAN_PORT_APPLICATION;
static const char* c_szCopyright = CAN_PORT_COPYRIGHT;
static const char* c_szWarranty = CAN_PORT_WARRANTY;
static const char* c_szLicense = CAN_PORT_LICENSE;
static const char* c_szBasename = CAN_PORT_PROGRAM;
static const char* c_szInterface = "(unknown)";
static const char* c_szAcceppted = "I ACCEPT";
static const char* c_szService = "0";

SOptions::SOptions() {
    // to have default bus speed from bit-timing index
    (void)CCanDriver::MapIndex2Bitrate(DEFAULT_BAUDRATE, m_Bitrate);
    (void)CCanDriver::MapBitrate2Speed(m_Bitrate, m_BusSpeed);
    // initialization
    m_szBasename = (char*)c_szBasename;
    m_szInterface = (char*)c_szInterface;
#if (OPTION_CANAPI_LIBRARY != 0)
    m_szSearchPath = (char*)NULL;
#else
    m_szJsonFilename = (char*)NULL;
#endif
#if (SERIAL_CAN_SUPPORTED != 0)
    m_u8Protocol = CANSIO_LAWICEL;
#endif
    m_OpMode.byte = DEFAULT_OP_MODE;
    m_Bitrate.index = DEFAULT_BAUDRATE;
    m_bHasDataPhase = false;
    m_bHasNoSamp = false;
#if (CAN_TRACE_SUPPORTED != 0)
    m_eTraceMode = SOptions::eTraceOff;
#endif
    m_szServerPort = (char*)c_szService;
    m_nLoggingLevel = 0;
    m_eSocketType = eIpcTcp;
    m_eDataFormat = eMtuRocketCan;
    m_fRisksAccepted = false;
    m_fListBitrates = false;
    m_fListBoards = false;
    m_fTestBoards = false;
    m_fVerbose = false;
    m_fExit = false;
}

int SOptions::ScanCommanline(int argc, const char* argv[], FILE* err, FILE* out) {
    int optind;
    char* optarg;
    int64_t intarg;

    int argInterface = 0;
    int optBitrate = 0;
    int optVerbose = 0;
    int optMode = 0;
    int optErrorFrames = 0;
#if (CAN_TRACE_SUPPORTED != 0)
    int optTraceMode = 0;
#endif
    int optSecurityRisks = 0;
    int optLogginglevel = 0;
    int optListBitrates = 0;
    int optListBoards = 0;
    int optTestBoards = 0;
#if (SERIAL_CAN_SUPPORTED != 0)
    int optProtocol = 0;
#endif
#if (OPTION_CANAPI_LIBRARY != 0)
    int optPath = 0;
#else
    int optJson = 0;
#endif
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
        switch (optind) {
        /* option '--baudrate=<baudrate>' (-b) */
        case BAUDRATE_STR:
        case BAUDRATE_CHR:
            if ((optBitrate++)) {
                fprintf(err, "%s: duplicated option /BAUDRATE\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /BAUDRATE\n", m_szBasename);
                return 1;
            }
            if (sscanf_s(optarg, "%lli", &intarg) != 1) {
                fprintf(err, "%s: illegal argument for option /BAUDRATE\n", m_szBasename);
                return 1;
            }
            switch (intarg) {
            case 0: case 1000: case 1000000: m_Bitrate.index = (int32_t)CANBTR_INDEX_1M; break;
            case 1: case 800:  case 800000:  m_Bitrate.index = (int32_t)CANBTR_INDEX_800K; break;
            case 2: case 500:  case 500000:  m_Bitrate.index = (int32_t)CANBTR_INDEX_500K; break;
            case 3: case 250:  case 250000:  m_Bitrate.index = (int32_t)CANBTR_INDEX_250K; break;
            case 4: case 125:  case 125000:  m_Bitrate.index = (int32_t)CANBTR_INDEX_125K; break;
            case 5: case 100:  case 100000:  m_Bitrate.index = (int32_t)CANBTR_INDEX_100K; break;
            case 6: case 50:   case 50000:   m_Bitrate.index = (int32_t)CANBTR_INDEX_50K; break;
            case 7: case 20:   case 20000:   m_Bitrate.index = (int32_t)CANBTR_INDEX_20K; break;
            case 8: case 10:   case 10000:   m_Bitrate.index = (int32_t)CANBTR_INDEX_10K; break;
            default:                         m_Bitrate.index = (int32_t)-intarg; break;
            }
            CANAPI_Bitrate_t bitrate;  // in order not to overwrite the index
            if (CCanDriver::MapIndex2Bitrate(m_Bitrate.index, bitrate) != CCanApi::NoError) {
                fprintf(err, "%s: illegal argument for option /BAUDRATE\n", m_szBasename);
                return 1;
            }
            if (CCanDriver::MapBitrate2Speed(bitrate, m_BusSpeed) != CCanApi::NoError) {
                fprintf(err, "%s: illegal argument for option /BAUDRATE\n", m_szBasename);
                return 1;
            }
            break;
        /* option '--bitrate=<bit-rate>' as string */
        case BITRATE_STR:
        case BITRATE_CHR:
            if ((optBitrate++)) {
                fprintf(err, "%s: duplicated option /BITRATE\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /BITRATE\n", m_szBasename);
                return 1;
            }
            if (CCanDriver::MapString2Bitrate(optarg, m_Bitrate, m_bHasDataPhase, m_bHasNoSamp) != CCanApi::NoError) {
                fprintf(err, "%s: illegal argument for option /BITRATE\n", m_szBasename);
                return 1;
            }
            if (CCanDriver::MapBitrate2Speed(m_Bitrate, m_BusSpeed) != CCanApi::NoError) {
                fprintf(err, "%s: illegal argument for option /BITRATE\n", m_szBasename);
                return 1;
            }
            break;
        /* option '--verbose' (-v) */
        case VERBOSE_STR:
        case VERBOSE_CHR:
            if (optVerbose) {
                fprintf(err, "%s: duplicated option /VERBOSE\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) != NULL) {
                fprintf(err, "%s: illegal argument for option /VERBOSE\n", m_szBasename);
                return 1;
            }
            m_fVerbose = true;
            break;
#if (OPTION_CANAPI_LIBRARY != 0)
        /* option '--path=<pathname>' (-p) */
        case JSON_STR:
        case JSON_CHR:
            if ((optPath++)) {
                fprintf(err, "%s: duplicated option /PATH\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /PATH\n", m_szBasename);
                return 1;
            }
            m_szSearchPath = optarg;
            break;
#endif
#if (SERIAL_CAN_SUPPORTED != 0)
        /* option '--protocol=(Lawicel|CANable)' (-z) */
        case PROTOCOL_STR:
        case PROTOCOL_CHR:
            if ((optProtocol++)) {
                fprintf(err, "%s: duplicated option /PROTOCOL\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /PROTOCOL\n", m_szBasename);
                return 1;
            }
            if (!strcasecmp(optarg, "LAWICEL") || !strcasecmp(optarg, "default") || !strcasecmp(optarg, "SLCAN"))
                m_u8Protocol = CANSIO_LAWICEL;
            else if (!strcasecmp(optarg, "CANABLE"))
                m_u8Protocol = CANSIO_CANABLE;
            else {
                fprintf(err, "%s: illegal argument for option /PROTOCOL\n", m_szBasename);
                return 1;
            }
            break;
#endif
        /* option '--mode=(2.0|FDF[+BRS])' (-m) */
        case OP_MODE_STR:
        case OP_MODE_CHR:
            if ((optMode++)) {
                fprintf(err, "%s: duplicated option /MODE\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /MODE\n", m_szBasename);
                return 1;
            }
            if (!strcasecmp(optarg, "DEFAULT") || !strcasecmp(optarg, "CLASSIC") || !strcasecmp(optarg, "CLASSICAL") ||
                !strcasecmp(optarg, "CAN20") || !strcasecmp(optarg, "CAN2.0") || !strcasecmp(optarg, "2.0"))
                m_OpMode.byte |= CANMODE_DEFAULT;
#if (CAN_FD_SUPPORTED != 0)
            else if (!strcasecmp(optarg, "CANFD") || !strcasecmp(optarg, "FD") || !strcasecmp(optarg, "FDF"))
                m_OpMode.byte |= CANMODE_FDOE;
            else if (!strcasecmp(optarg, "CANFD+BRS") || !strcasecmp(optarg, "FDF+BRS") || !strcasecmp(optarg, "FD+BRS"))
                m_OpMode.byte |= CANMODE_FDOE | CANMODE_BRSE;
#endif
            else {
                fprintf(err, "%s: illegal argument for option /MODE\n", m_szBasename);
                return 1;
            }
            break;
        /* option '--error-frames' */
        case OP_ERR_STR:
            if ((optErrorFrames++)) {
                fprintf(err, "%s: duplicated option /ERR\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /ERR\n", m_szBasename);
                return 1;
            }
            if (!strcasecmp(optarg, "YES") || !strcasecmp(optarg, "Y") || !strcasecmp(optarg, "ON") || !strcasecmp(optarg, "1"))
                m_OpMode.byte |= CANMODE_ERR;
            else if (!strcasecmp(optarg, "NO") || !strcasecmp(optarg, "N") || !strcasecmp(optarg, "OFF") || !strcasecmp(optarg, "0"))
                m_OpMode.byte &= ~CANMODE_ERR;
            else {
                fprintf(err, "%s: illegal argument for option /ERR\n", m_szBasename);
                return 1;
            }
            break;
        case OP_ERRFRMS_STR:
            if ((optErrorFrames++)) {
                fprintf(err, "%s: duplicated option /ERROR-FRAMES\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) != NULL) {
                fprintf(err, "%s: illegal argument for option /ERROR-FRAMES\n", m_szBasename);
                return 1;
            }
            m_OpMode.byte |= CANMODE_ERR;
            break;
        /* option '--trace=(ON|OFF)' (-y) */
#if (CAN_TRACE_SUPPORTED != 0)
        case TRACEFILE_STR:
        case TRACEFILE_CHR:
            if (optTraceMode++) {
                fprintf(err, "%s: duplicated option /TRACE\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /TRACE\n", m_szBasename);
                return 1;
            }
#if (CAN_TRACE_SUPPORTED == 1)
            if (!strcasecmp(optarg, "OFF") || !strcasecmp(optarg, "NO") || !strcasecmp(optarg, "n") || !strcasecmp(optarg, "0"))
                m_eTraceMode = SOptions::eTraceOff;
            else if (!strcasecmp(optarg, "ON") || !strcasecmp(optarg, "YES") || !strcasecmp(optarg, "y") || !strcasecmp(optarg, "1"))
                m_eTraceMode = SOptions::eTraceVendor;
#else
            if (!strcasecmp(optarg, "BIN") || !strcasecmp(optarg, "BINARY") || !strcasecmp(optarg, "default"))
                m_eTraceMode = SOptions::eTraceBinary;
            else if (!strcasecmp(optarg, "CSV") || !strcasecmp(optarg, "logger") || !strcasecmp(optarg, "log"))
                m_eTraceMode = SOptions::eTraceLogger;
            else if (!strcasecmp(optarg, "TRC") || !strcasecmp(optarg, "vendor"))
                m_eTraceMode = SOptions::eTraceVendor;
#endif
            else {
                fprintf(err, "%s: illegal argument for option /TRACE\n", m_szBasename);
                return 1;
            }
            break;
#endif
        /* option '--logging=<level>' */
        case LOGGING_STR:
        case LOGGING_CHR:
            if (optLogginglevel++) {
                fprintf(err, "%s: duplicated option /LOGGING\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /LOGGING\n", m_szBasename);
                return 1;
            }
            if (sscanf(optarg, "%lli", &intarg) != 1) {
                fprintf(err, "%s: illegal argument for option /LOGGING'\n", m_szBasename);
                return 1;
            }
            if ((intarg < 0) || (intarg > INT_MAX)) {
                fprintf(err, "%s: illegal argument for option /LOGGING\n", m_szBasename);
                return 1;
            }
            m_nLoggingLevel = (int)intarg;
            break;
        /* option '--security-risks="I ACCEPT" */
        case SECURITY_RISKS:
            if (optSecurityRisks++) {
                fprintf(err, "%s: duplicated option /SECURITY-RISKS\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /SECURITY-RISKS\n", m_szBasename);
                return 1;
            }
            m_fRisksAccepted = (strcasecmp(optarg, c_szAcceppted) == 0) ? true : false;
            break;
        /* option '--list-bitrates[=(2.0|FDF[+BRS])]' */
        case LISTBITRATES_STR:
            if ((optListBitrates++)) {
                fprintf(err, "%s: duplicated option /LIST-BITRATES\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) != NULL) {
                if ((optMode++)) {
                    fprintf(err, "%s: option /MODE already set\n", m_szBasename);
                    return 1;
                }
                if (!strcasecmp(optarg, "DEFAULT") || !strcasecmp(optarg, "CLASSIC") || !strcasecmp(optarg, "CLASSICAL") ||
                    !strcasecmp(optarg, "CAN20") || !strcasecmp(optarg, "CAN2.0") || !strcasecmp(optarg, "2.0"))
                    m_OpMode.byte |= CANMODE_DEFAULT;
#if (CAN_FD_SUPPORTED != 0)
                else if (!strcasecmp(optarg, "CANFD") || !strcasecmp(optarg, "FD") || !strcasecmp(optarg, "FDF"))
                    m_OpMode.byte |= CANMODE_FDOE;
                else if (!strcasecmp(optarg, "CANFD+BRS") || !strcasecmp(optarg, "FDF+BRS") || !strcasecmp(optarg, "FD+BRS"))
                    m_OpMode.byte |= CANMODE_FDOE | CANMODE_BRSE;
#endif
                else {
                    fprintf(err, "%s: illegal argument for option /LIST-BITRATES\n", m_szBasename);
                    return 1;
                }
            }
            m_fListBitrates = true;
            m_fExit = true;
            break;
        /* option '--list-boards' (-L) */
        case LISTBOARDS_STR:
        case LISTBOARDS_CHR:
            if ((optListBoards++)) {
                fprintf(err, "%s: duplicated option /LIST-BOARDS\n", m_szBasename);
                return 1;
            }
#if (OPTION_CANAPI_LIBRARY != 0)
            if ((optarg = getOptionParameter()) != NULL) {
                if ((optPath++)) {
                    fprintf(err, "%s: option /PATH already set\n", m_szBasename);
                    return 1;
                }
                m_szSearchPath = optarg;  // option '--list-boards=<pathname>' (-L)
            }
#endif
            m_fListBoards = true;
            m_fExit = true;
            break;
        /* option '--test-boards' (-T) */
        case TESTBOARDS_STR:
        case TESTBOARDS_CHR:
            if ((optTestBoards++)) {
                fprintf(err, "%s: duplicated option /TEST-BOARDS\n", m_szBasename);
                return 1;
            }
#if (OPTION_CANAPI_LIBRARY != 0)
            if ((optarg = getOptionParameter()) != NULL) {
                if ((optPath++)) {
                    fprintf(err, "%s: option /PATH already set\n", m_szBasename);
                    return 1;
                }
                m_szSearchPath = optarg;  // option '--test-boards=<pathname>' (-L)
            }
#endif
            m_fTestBoards = true;
            m_fExit = true;
            break;
#if (OPTION_CANAPI_LIBRARY == 0)
        /* option '--json=<filename>' (-j) */
        case JSON_STR:
        case JSON_CHR:
            if ((optJson++)) {
                fprintf(err, "%s: duplicated option /JSON-FILE\n", m_szBasename);
                return 1;
            }
            if ((optarg = getOptionParameter()) == NULL) {
                fprintf(err, "%s: missing argument for option /JSON-FILE\n", m_szBasename);
                return 1;
            }
            m_szJsonFilename = optarg;
            m_fExit = true;
            break;
#endif
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
    // (3) scan command-line for argument <interface>
    for (int i = 1; i < argc; i++) {
        if (!isOption(argc, (char**)argv, MAX_OPTIONS, option, i)) {
            if ((argInterface++)) {
                fprintf(err, "%s: too many arguments\n", m_szBasename);
                return 1;
            }
            m_szInterface = (char*)argv[i];
        }
    }
    // - check if one and only one <interface> is given
    if (!argInterface && !m_fExit) {
        fprintf(err, "%s: no interface given\n", m_szBasename);
        return 1;
    }
    // - extract <port> from <interface>
    m_szServerPort = strchr(m_szInterface, '@');
    if (m_szServerPort && strlen(m_szServerPort) > 1)
       *m_szServerPort++ = '\0';
    else if (m_fExit)
        m_szServerPort = (char*)c_szService;
    else {
        fprintf(err, "%s: illegal argument for <interface>@<port> (`%s')\n", m_szBasename, m_szInterface);
        return 1;
    }
    // (4) check for illegal combinations
#if (CAN_FD_SUPPORTED != 0)
    /* - check bit-timing index (n/a for CAN FD) */
    if (m_OpMode.fdoe && (m_Bitrate.btr.frequency <= CANBTR_INDEX_1M) && !m_fExit) {
        fprintf(err, "%s: illegal combination of options /MODE and /BAUDRATE\n", m_szBasename);
        return 1;
    }
#endif
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
    if (!stream)
        return;
    fprintf(stream, "Usage: %s <interface>@<port> [<option>...]\n", m_szBasename);
    fprintf(stream, "Options:\n");
#if (OPTION_CANAPI_LIBRARY != 0)
    fprintf(stream, "  /Path:<pathname>                    search path for JSON configuration files\n");
#endif
#if (CAN_FD_SUPPORTED != 0)
    fprintf(stream, "  /Mode:(2.0|FDf[+BRS])               CAN operation mode: CAN CC or CAN FD mode\n");
#else
    fprintf(stream, "  /Mode:2.0                           CAN operation mode: CAN CC\n");
#endif
    fprintf(stream, "  /ERR:(No|Yes) | /ERROR-FRAMES       allow reception of error frames\n");
    fprintf(stream, "  /BauDrate:<baudrate>                CAN bit-timing in kbps (default=250), or\n");
    fprintf(stream, "  /BitRate:<bitrate>                  CAN bit-rate settings (as key/value list)\n");
    fprintf(stream, "  /Verbose                            show detailed bit-rate settings\n");
#if (CAN_TRACE_SUPPORTED != 0)
#if (CAN_TRACE_SUPPORTED == 1)
    fprintf(stream, "  /TRaCe:(ON|OFF)                     write a trace file (default=OFF)\n");
#else
    fprintf(stream, "  /TRaCe:(BIN|CSV|TRC)                write a trace file (default=OFF)\n");
#endif
#endif
#if (SERIAL_CAN_SUPPORTED != 0)
    fprintf(stream, "  /PRotocol:(Lawicel|CANable)         select SLCAN protocol (default=Lawicel)\n");
#endif
    fprintf(stream, "  /LOGging=<level>                    set logging level (default=0)\n");
    fprintf(stream, "  /SECURITY-RISKS=\"I ACCEPT\"          accept security risks (skip interactive input)\n");
#if (CAN_FD_SUPPORTED != 0)
    fprintf(stream, "  /LIST-BITRATES[:(2.0|FDf[+BRS])]    list standard bit-rate settings and exit\n");
#else
    fprintf(stream, "  /LIST-BITRATES[:2.0]                list standard bit-rate settings and exit\n");
#endif
#if (OPTION_CANAPI_LIBRARY != 0)
    fprintf(stream, "  /LIST-boards[:<pathname>]           list all supported CAN interfaces and exit\n");
    fprintf(stream, "  /TEST-boards[:<pathname>]           list all available CAN interfaces and exit\n");
#else
    fprintf(stream, "  /LIST-BOARDS | /LIST                list all supported CAN interfaces and exit\n");
    fprintf(stream, "  /TEST-BOARDS | /TEST                list all available CAN interfaces and exit\n");
    fprintf(stream, "  /JSON-file:<filename>               write configuration into JSON file and exit\n");
#endif
    fprintf(stream, "  /HELP | /?                          display this help screen and exit\n");
    fprintf(stream, "  /VERSION                            show version information and exit\n");
    if (args) {
        fprintf(stream, "Arguments:\n");
        fprintf(stream, "  <port>         CAN-over-Ethernet port\n");
        fprintf(stream, "  <interface>    CAN interface board (list all with /LIST)\n");
        fprintf(stream, "  <baudrate>     CAN baud rate index (default=3):\n");
        fprintf(stream, "                 0 = 1000 kbps\n");
        fprintf(stream, "                 1 = 800 kbps\n");
        fprintf(stream, "                 2 = 500 kbps\n");
        fprintf(stream, "                 3 = 250 kbps\n");
        fprintf(stream, "                 4 = 125 kbps\n");
        fprintf(stream, "                 5 = 100 kbps\n");
        fprintf(stream, "                 6 = 50 kbps\n");
        fprintf(stream, "                 7 = 20 kbps\n");
        fprintf(stream, "                 8 = 10 kbps\n");
        fprintf(stream, "  <bitrate>      comma-separated key/value list:\n");
        fprintf(stream, "                 f_clock=<value>      frequency in Hz or\n");
        fprintf(stream, "                 f_clock_mhz=<value>  frequency in MHz\n");
        fprintf(stream, "                 nom_brp=<value>      bit-rate prescaler (nominal)\n");
        fprintf(stream, "                 nom_tseg1=<value>    time segment 1 (nominal)\n");
        fprintf(stream, "                 nom_tseg2=<value>    time segment 2 (nominal)\n");
        fprintf(stream, "                 nom_sjw=<value>      sync. jump width (nominal)\n");
        fprintf(stream, "                 nom_sam=<value>      sampling (only SJA1000)\n");
        fprintf(stream, "                 data_brp=<value>     bit-rate prescaler (FD data)\n");
        fprintf(stream, "                 data_tseg1=<value>   time segment 1 (FD data)\n");
        fprintf(stream, "                 data_tseg2=<value>   time segment 2 (FD data)\n");
        fprintf(stream, "                 data_sjw=<value>     sync. jump width (FD data).\n");
    }
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
