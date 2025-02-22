//  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later
//
//  CAN Interface API, Version 3 (CAN-over-Ethernet Client)
//
//  Copyright (c) 2008-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//  All rights reserved.
//
//  This file is part of CAN API V3.
//
//  CAN API V3 is dual-licensed under the BSD 2-Clause "Simplified" License
//  and under the GNU General Public License v2.0 (or any later version). You can
//  choose between one of them if you use CAN API V3 in whole or in part.
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
#include "CanTcpClient.h"
#include "tcp_client.h"
#include "RocketCAN.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#ifndef CANERR_SYSTEM
#define CANERR_SYSTEM  (-10000)
#endif
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// TODO: Check retun values with original RocketCAN code

CCanTcpClient::CCanTcpClient() {
    m_nSocket = (-1);
    m_nFrameSize = sizeof(CANTCP_Message_t);
}

CCanTcpClient::~CCanTcpClient() {
    (void)Disconnect();
}

CANAPI_Return_t CCanTcpClient::Connect(const char *serverName) {
    m_nSocket = tcp_client_connect(serverName);
    return (m_nSocket >= 0) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}

CANAPI_Return_t CCanTcpClient::Disconnect(void) {
    if (m_nSocket >= 0) {
        if (tcp_client_close(m_nSocket) != 0) {
            return (CANERR_SYSTEM - errno);
        }
        m_nSocket = (-1);
    }
    return CANERR_NOERROR;
}

CANAPI_Return_t CCanTcpClient::Receive(CANAPI_Message_t &message, uint16_t timeout) {
    CANTCP_Message_t packet = {};
    // receive RocketCAN message from network
    ssize_t nbyte = tcp_client_recv(m_nSocket, (void*)&packet, sizeof(packet), timeout);
    if (nbyte < 0) {
        return (errno == ENODATA) ? CANERR_RX_EMPTY : (CANERR_SYSTEM - errno);
    } else if (nbyte != (ssize_t)sizeof(packet)) {
        return (CANERR_SYSTEM - EPROTO);
    }
    // check RocketCAN message for validity
    if (!rock_msg_is_valid(&packet) && !rock_msg_is_abort(&packet)) {
        return (CANERR_SYSTEM - EPROTO);
    }
    // map RocketCAN message to CAN API V3 message
    rock_msg_to_can(&message, &packet);
    // all all alright
    return CANERR_NOERROR;
}

CANAPI_Return_t CCanTcpClient::Send(CANAPI_Message_t message, uint16_t inhibitTime) {
    CANTCP_Message_t packet = {};
    // no timestamp on CAN TX messages, take current time instead
    (void)clock_gettime(CLOCK_REALTIME, &message.timestamp);
    // map CAN API V3 message to RocketCAN message
    rock_msg_from_can(&packet, &message);
    // send RocketCAN message over the network
    ssize_t nbyte = tcp_client_send(m_nSocket, (const void*)&packet, sizeof(packet));
    if (nbyte < 0) {
        return (CANERR_SYSTEM - errno);
    } else if (nbyte != (ssize_t)sizeof(packet)) {
        return (CANERR_SYSTEM - EPROTO);
    }
    // TODO: implement inhibit time
    (void)inhibitTime;
    return CANERR_NOERROR;
}

CANAPI_Return_t CCanTcpClient::Receive(void *data, size_t size, uint16_t timeout) {
    ssize_t nbyte = tcp_client_recv(m_nSocket, data, size, timeout);
    if (nbyte < 0) {
        return (errno == ENODATA) ? CANERR_RX_EMPTY : (CANERR_SYSTEM - errno);
    } else if (nbyte != (ssize_t)size) {
        return (CANERR_SYSTEM - EPROTO);
    }
    return CANERR_NOERROR;
}

CANAPI_Return_t CCanTcpClient::Send(const void *data, size_t size, uint16_t inhibitTime) {
    ssize_t nbyte = tcp_client_send(m_nSocket, data, size);
    if (nbyte < 0) {
        return (CANERR_SYSTEM - errno);
    } else if (nbyte != (ssize_t)size) {
        return (CANERR_SYSTEM - EPROTO);
    }
    // TODO: implement inhibit time
    (void)inhibitTime;
    return CANERR_NOERROR;
}
    