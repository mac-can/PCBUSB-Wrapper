//  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later
//
//  CAN Interface API, Version 3 (RocketCAN Server)
//
//  Copyright (c) 2008-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
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
#include "CanIpcServer.h"
#include "ipc_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef CANERR_SYSTEM
#define CANERR_SYSTEM  (-10000)
#endif
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// TODO: Check retun values with original RocketCAN code

CCanIpcServer::CCanIpcServer(EIpcProtocol protocol) {
    m_nPort = 0;
    m_pServer = NULL;
    m_evCallback = NULL;
    m_pParameter = NULL;
    m_ePprotocol = protocol;
    m_eFrameFormat = eRocketCAN;
    m_nMtuSize = CANIPC_MTU_ROCKETCAN;
    m_nLogging = IPC_LOGGING_NONE;
}

CCanIpcServer::~CCanIpcServer() {
    (void)Stop();
}

bool CCanIpcServer::SetFrameFormat(EFrameFormat format) {
    if (m_pServer == NULL) {
        switch (format) {
            case eRocketCAN: m_nMtuSize = CANIPC_MTU_ROCKETCAN; break;
#if (OPTION_CANIPC_SOCKETCAN != 0) 
            case eSocketCAN: m_nMtuSize = CANIPC_MTU_SOCKETCAN; break;
            case eSocketCAN_FD: m_nMtuSize = CANIPC_MTU_SOCKETCAN_FD; break;
#endif
            default: return false;
        }
        m_eFrameFormat = format;
        return true;
    }
    return false;
}

CANAPI_Return_t CCanIpcServer::Start(uint16_t port) {
    if (m_pServer != NULL) return CANERR_YETINIT;
    m_pServer = ipc_server_start(port, (int)m_ePprotocol, m_nMtuSize, m_evCallback, m_pParameter, m_nLogging);
    m_nPort = (m_pServer != NULL) ? port : 0;
    return (m_pServer != NULL) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}

CANAPI_Return_t CCanIpcServer::Stop(void) {
    CANAPI_Return_t retVal = CANERR_FATAL;
    if (m_pServer == NULL) return CANERR_NOTINIT;
    retVal = ipc_server_stop(m_pServer);
    m_pServer = NULL;
    m_nPort = 0;
    return (retVal == 0) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}

CANAPI_Return_t CCanIpcServer::Send(CANAPI_Message_t message, uint16_t inhibitTime) {
    CANAPI_Return_t retVal = CANERR_FATAL;
    CANIPC_Message_t data = {};
    // map the CAN API V3 message to RocketCAN message
    data.id = message.id;
    data.flags = CANIPC_XTD_FLAG(message.xtd) |
                 CANIPC_RTR_FLAG(message.rtr) |
                 CANIPC_FDF_FLAG(message.fdf) |
                 CANIPC_BRS_FLAG(message.brs) |
                 CANIPC_ESI_FLAG(message.esi) |
                 CANIPC_STS_FLAG(message.sts);
    data.length = CCanApi::Dlc2Len(message.dlc);
    memcpy(data.data, message.data, MAX(message.dlc, CANFD_MAX_LEN));
    data.timestamp.tv_sec = message.timestamp.tv_sec;
    data.timestamp.tv_nsec = message.timestamp.tv_nsec;
    data.busload = 0;
    data.status = 0;
    // convert RocketCAN message to network byte order
    CAN_IPC_MSG_HTON(data);
    // send RocketCAN message over the network
    if (m_pServer == NULL) return CANERR_NOTINIT;
    retVal = ipc_server_send(m_pServer, (void*)&data, sizeof(data));
    // TODO: implement inhibit time
    (void)inhibitTime;
    return (retVal == 0) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}

CANAPI_Return_t CCanIpcServer::Send(const void *data, size_t size, uint16_t inhibitTime) {
    CANAPI_Return_t retVal = CANERR_FATAL;
    // send data over the network
    if (m_pServer == NULL) return CANERR_NOTINIT;
    retVal = ipc_server_send(m_pServer, data, size);
    // TODO: implement inhibit time
    (void)inhibitTime;
    return (retVal == 0) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}
