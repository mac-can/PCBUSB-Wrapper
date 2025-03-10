//  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later
//
//  CAN Interface API, Version 3 (Testing)
//
//  Copyright (c) 2004-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//  All rights reserved.
//
//  This file is part of CAN API V3.
//
//  CAN API V3 is dual-licensed under the BSD 2-Clause "Simplified" License
//  and under the GNU General Public License v2.0 (or any later version).
//  You can choose between one of them if you use this file.
//
//  (1) BSD 2-Clause "Simplified" License
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this
//     list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  CAN API V3 IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF CAN API V3, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  (2) GNU General Public License v2.0 or later
//
//  CAN API V3 is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  CAN API V3 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with CAN API V3; if not, see <https://www.gnu.org/licenses/>.
//
#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include "Driver.h"
#if (OPTION_CANTCP_ENABLED != 0)
#include "Server.h"
#endif
#define NUM_DUTS  2                     // number of devices under test
#define DUT1   0                        // device under test #1
#define DUT2   1                        // device under test #2

class COptions {
private:
    const CANAPI_OpMode_t m_DefaultOpMode = { TEST_CANMODE };
    const CANAPI_Bitrate_t m_DefaultBitrate = { TEST_BTRINDEX };
    const char *szUnkown = "(unknown)";
    // two devices under test
    struct SDevice {
        CCanApi::SChannelInfo m_Info;   // channel no. and library id.
        CANAPI_OpMode_t m_OpMode;       // operation mode
        CANAPI_Bitrate_t m_Bitrate;     // bit-rate settings
        void *m_pParameter;             // non PnP parameter
    } m_Dut[NUM_DUTS];
#if (OPTION_CANAPI_LIBRARY != 0)
    // search path for configurations
    char *m_szSearchPath;               // search path for JSON files
#endif
    // test options
    int32_t m_s32TestFrames;            // number of CAN frames to be sent during tests
    int32_t m_s32SmokeTestFrames;       // number of CAN frames to be sent during smoketest
    bool m_fStartInteractive;           // start test execution only when the Anykey is pressed
    bool m_fCallSequences;              // execute test cases from suite 'CallSequences'
    bool m_fBitrateConverter;           // execute test cases from suite 'BitrateConverter'
    bool m_fCanClassic;                 // execute only test cases in CAN Classic mode
    bool m_f3rdDevice;                  // a third device with default bit-rate is connected
    bool m_fRtrDevice;                  // a device that can answer RTR frames is connected
    bool m_fRunQuick;                   // omit some long lasting test cases
    bool m_fShowHelp;                   // show help and exit
#if (OPTION_CANTCP_ENABLED != 0)
    // server options
    struct SServer {
        bool m_fEnable;                 // enable RocketCAN server
        char *m_szService;              // service name or port number
        int m_nLogging;                 // logging level
    } m_Server; 
#endif
public:
    // constructor / destructor
    COptions();
    virtual ~COptions() {};
    // getter/setter
#if (OPTION_CANAPI_LIBRARY != 0)
    char *GetSearchPath() {
        return m_szSearchPath;
    }
#endif
    int32_t GetLibraryId(int dut) {
        return ((0 <= dut) && (dut < NUM_DUTS)) ? m_Dut[dut].m_Info.m_nLibraryId : INVALID_HANDLE;
    }
    int32_t GetChannelNo(int dut) {
        return ((0 <= dut) && (dut < NUM_DUTS)) ? m_Dut[dut].m_Info.m_nChannelNo : INVALID_HANDLE;
    }
    CANAPI_OpMode_t GetOpMode(int dut) {
        return ((0 <= dut) && (dut < NUM_DUTS)) ? m_Dut[dut].m_OpMode : m_DefaultOpMode;
    }
    CANAPI_Bitrate_t GetBitrate(int dut) {
        return ((0 <= dut) && (dut < NUM_DUTS)) ? m_Dut[dut].m_Bitrate : m_DefaultBitrate;
    }
    void *GetParameter(int dut) {
        return ((0 <= dut) && (dut < NUM_DUTS)) ? m_Dut[dut].m_pParameter : NULL;
    }
    char *GetDeviceName(int dut) {
        return ((0 <= dut) && (dut < NUM_DUTS) && (m_Dut[dut].m_Info.m_nChannelNo != INVALID_HANDLE)) ? m_Dut[dut].m_Info.m_szDeviceName : (char*)szUnkown;
    }
    int32_t GetNumberOfTestFrames() {
        return (0 < m_s32TestFrames) ? m_s32TestFrames : 0;
    }
    int32_t GetNumberOfSmokeTestFrames() {
        return (0 < m_s32SmokeTestFrames) ? m_s32SmokeTestFrames : 0;
    }
    bool RunTestCallSequences() {
        return m_fCallSequences;
    }
    bool RunTestBitrateConverter() {
        return m_fBitrateConverter;
    }
    bool RunCanClassicOnly() {
        return m_fCanClassic;
    }
    bool RunQuick() {
        return m_fRunQuick;
    }
    bool Is3rdDevicePresent() {
        return m_f3rdDevice;
    }
    bool IsRtrDevicePresent() {
        return m_fRtrDevice;
    }
    bool StartInteractive() {
        return m_fStartInteractive;
    }
#if (OPTION_CANTCP_ENABLED != 0)
    bool IsCanServerEnabled() {
        return m_Server.m_fEnable;
    }
    const char *GetCanServerService() {
        return (const char*)m_Server.m_szService;
    }
#if (0)
    CCanServer::EFrameFormat GetCanServerFormat() {
        return m_Server.m_eFormat;
    }
    CCanServer::EIpcProtocol GetCanServerProtocol() {
        return m_Server.m_eProtocol;
    }
#endif
    int GetCanServerLoggingLevel() {
        return m_Server.m_nLogging;
    }
#endif
    // scan commandline arguments
    int ScanOptions(int argc, char* argv[], char* err = NULL, size_t len = 0);
    int ShowHelp();
private:
    bool GetChannelInfoByDeviceName(const char *name, CCanApi::SChannelInfo &info);
    bool GetChannelInfoFromDeviceList(int32_t library, int32_t channel, CCanApi::SChannelInfo &info);
};

extern COptions g_Options;          // global access to testing options

#endif // OPTIONS_H_INCLUDED

// $Id: Options.h 1456 2025-02-19 21:22:16Z sedna $  Copyright (c) UV Software, Berlin.
