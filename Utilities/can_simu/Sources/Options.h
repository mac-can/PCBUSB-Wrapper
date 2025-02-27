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
#ifndef CAN_SIMU_OPTIONS_H_INCLUDED
#define CAN_SIMU_OPTIONS_H_INCLUDED

#include "Driver.h"
#include "Version.h"

#include <stdio.h>
#include <stdint.h>

#define CAN_SIMU_APPLICATION "CAN-over-Ethernet Server Simulator, Version " VERSION_STRING
#define CAN_SIMU_COPYRIGHT   "Copyright (c) " SERVER_COPYRIGHT
#if !defined(_WIN32) && !defined(_WIN64)
#define CAN_SIMU_WARRANTY    "This program comes with ABSOLUTELY NO WARRANTY!\n\n" \
                             "This is free software, and you are welcome to redistribute it\n" \
                             "under certain conditions; type `" CAN_SIMU_PROGRAM " --version' for details.";
#else
#define CAN_SIMU_WARRANTY    "This program comes with ABSOLUTELY NO WARRANTY!\n\n" \
                             "This is free software, and you are welcome to redistribute it\n" \
                             "under certain conditions; type '" CAN_SIMU_PROGRAM " /VERSION' for details.";
#endif
#define CAN_SIMU_LICENSE     "This program is free software; you can redistribute it and/or modify\n" \
                             "it under the terms of the GNU General Public License as published by\n" \
                             "the Free Software Foundation; either version 2 of the License, or\n" \
                             "(at your option) any later version.\n\n" \
                             "This program is distributed in the hope that it will be useful,\n" \
                             "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
                             "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" \
                             "GNU General Public License for more details.\n\n" \
                             "You should have received a copy of the GNU General Public License along\n" \
                             "with this program; if not, see <https://www.gnu.org/licenses/>."
#define CAN_SIMU_PROGRAM     "can_send"

#define CAN_SIMU_SECURITY    "This program will open a network socket for CAN-over-Ethernet communication.\n" \
                             "This may expose your computer to security vulnerabilities, unauthorized\n" \
                             "access, data interception, denial of service attacks, and resource\n" \
                             "exhaustion.\n\n" \
                             "It is strongly recommended to implement appropriate security measures\n" \
                             "to minimize these risks."
#define CAN_SIMU_QUESTION    "Enter 'Y' to accept these risks and continue, or any other key to exit: "
#define CAN_SIMU_ACCEPTED    "Security risks accepted. Continue with caution!"

struct SOptions {
    // attributes
    char* m_szBasename;
    char* m_szServerPort;
    enum EIpcSocketType {
        eIpcTcp = 1,  // SOCK_STREAM (TCP)
        eIpcUdp = 2,  // SOCK_DGRAM (UDP)
    } m_eSocketType;
    enum EIpcDataFormat {
        eMtuRocketCan,  // CAN API V3
        eMtuSocketCan,  // Linux Kernel CAN
    } m_eDataFormat;
    int m_nLoggingLevel;
    bool m_fRisksAccepted;
    bool m_fExit;
    // initializer
    SOptions();
    // operations
    int ScanCommanline(int argc, const char* argv[], FILE* err = stderr, FILE* out = stdout);
    void ShowGreetings(FILE* stream);
    void ShowFarewell(FILE* stream);
    void ShowVersion(FILE* stream);
    void ShowHelp(FILE* stream);
    void ShowUsage(FILE* stream, bool args = false);
};

#endif  // CAN_SIMU_OPTIONS_H_INCLUDED
