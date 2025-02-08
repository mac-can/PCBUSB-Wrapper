//  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later
//
//  CAN Interface API, Version 3 (RocketCAN Client)
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
#include "CanIpcClient.h"
#include "ipc_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef CANERR_SYSTEM
#define CANERR_SYSTEM  (-10000)
#endif
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// TODO: Check retun values with original RocketCAN code

CCanIpcClient::CCanIpcClient(EIpcProtocol protocol) {
    m_nSocket = (-1);
    m_ePprotocol = protocol;
    m_eFrameFormat = eRocketCAN;
    m_nMtuSize = CANIPC_MTU_ROCKETCAN;
}

CCanIpcClient::~CCanIpcClient() {
    (void)Disconnect();
}

bool CCanIpcClient::SetFrameFormat(EFrameFormat format) {
    if (m_nSocket < 0) {
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

CANAPI_Return_t CCanIpcClient::Connect(const char *serverName) {
    m_nSocket = ipc_client_connect(serverName, (int)m_ePprotocol);
    return (m_nSocket >= 0) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}

CANAPI_Return_t CCanIpcClient::Disconnect(void) {
    if (m_nSocket >= 0) {
        if (ipc_client_close(m_nSocket) != 0) {
            return (CANERR_SYSTEM - errno);
        }
        m_nSocket = (-1);
    }
    return CANERR_NOERROR;
}

CANAPI_Return_t CCanIpcClient::Receive(CANAPI_Message_t &message, uint16_t timeout) {
    CANIPC_Message_t data = {};
    // receive RocketCAN message from network
    ssize_t nbyte = ipc_client_recv(m_nSocket, (void*)&data, sizeof(data), timeout);
    if (nbyte < 0) {
        return (errno == ENODATA) ? CANERR_RX_EMPTY : (CANERR_SYSTEM - errno);
    } else if (nbyte != (ssize_t)sizeof(data)) {
        return (CANERR_SYSTEM - EPROTO);
    }
    // convert RocketCAN message to host byte order
    CAN_IPC_MSG_NTOH(data);
    // map RocketCAN message to CAN API V3 message
    message.id = data.id;
    message.xtd = (data.flags &CANIPC_XTD_MASK) ? 1 : 0;
    message.rtr = (data.flags &CANIPC_RTR_MASK) ? 1 : 0;
    message.fdf = (data.flags &CANIPC_FDF_MASK) ? 1 : 0;
    message.brs = (data.flags &CANIPC_BRS_MASK) ? 1 : 0;
    message.esi = (data.flags &CANIPC_ESI_MASK) ? 1 : 0;
    message.sts = (data.flags &CANIPC_STS_MASK) ? 1 : 0;
    message.dlc = CCanApi::Len2Dlc(data.length);
    memcpy(message.data, data.data, MAX(data.length, CANIPC_MAX_LEN));
    message.timestamp.tv_sec = data.timestamp.tv_sec;
    message.timestamp.tv_nsec = data.timestamp.tv_nsec;
    // return all alright
    return CANERR_NOERROR;
}

CANAPI_Return_t CCanIpcClient::Send(CANAPI_Message_t message, uint16_t inhibitTime) {
    CANIPC_Message_t data = {};
    // map CAN API V3 message to RocketCAN message
    data.id = message.id;
    data.flags |= CANIPC_XTD_FLAG(message.xtd);
    data.flags |= CANIPC_RTR_FLAG(message.rtr);
    data.flags |= CANIPC_FDF_FLAG(message.fdf);
    data.flags |= CANIPC_BRS_FLAG(message.brs);
    data.flags |= CANIPC_ESI_FLAG(message.esi);
    data.length = CCanApi::Dlc2Len(message.dlc);
    memcpy(data.data, message.data, MAX(data.length, CANIPC_MAX_LEN));
    data.timestamp.tv_sec = message.timestamp.tv_sec;
    data.timestamp.tv_nsec = message.timestamp.tv_nsec;
    // convert RocketCAN message to network byte order
    CAN_IPC_MSG_HTON(data);
    // send RocketCAN message
    ssize_t nbyte = ipc_client_send(m_nSocket, (const void*)&data, sizeof(data));
    if (nbyte < 0) {
        return (CANERR_SYSTEM - errno);
    } else if (nbyte != (ssize_t)sizeof(data)) {
        return (CANERR_SYSTEM - EPROTO);
    }
    // TODO: implement inhibit time
    (void)inhibitTime;
    return CANERR_NOERROR;
}

CANAPI_Return_t CCanIpcClient::Receive(void *data, size_t size, uint16_t timeout) {
    ssize_t nbyte = ipc_client_recv(m_nSocket, data, size, timeout);
    if (nbyte < 0) {
        return (errno == ENODATA) ? CANERR_RX_EMPTY : (CANERR_SYSTEM - errno);
    } else if (nbyte != (ssize_t)size) {
        return (CANERR_SYSTEM - EPROTO);
    }
    return CANERR_NOERROR;
}

CANAPI_Return_t CCanIpcClient::Send(const void *data, size_t size, uint16_t inhibitTime) {
    ssize_t nbyte = ipc_client_send(m_nSocket, data, size);
    if (nbyte < 0) {
        return (CANERR_SYSTEM - errno);
    } else if (nbyte != (ssize_t)size) {
        return (CANERR_SYSTEM - EPROTO);
    }
    // TODO: implement inhibit time
    (void)inhibitTime;
    return CANERR_NOERROR;
}
    