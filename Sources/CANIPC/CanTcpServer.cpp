//  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later
//
//  CAN Interface API, Version 3 (CAN-over-Ethernet Server)
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
#include "CanTcpServer.h"
#include "tcp_server.h"
#include "RocketCAN.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef CANERR_SYSTEM
#define CANERR_SYSTEM  (-10000)
#endif
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define SERVER_NULL()   do { m_pServer = NULL; } while (0)
#define SERVICE_NULL()  do { m_szService[0] = '0'; m_szService[1] = '\0'; } while (0)

// TODO: Check retun values with original RocketCAN code

CCanTcpServer::CCanTcpServer() {
    SERVER_NULL();
    SERVICE_NULL();
    m_evCallback = NULL;
    m_pParameter = NULL;
    m_nLogging = TCP_LOGGING_NONE;
    m_nFrameSize = sizeof(CANTCP_Message_t);
}

CCanTcpServer::~CCanTcpServer() {
    (void)Stop();
}

CANAPI_Return_t CCanTcpServer::Start(const char *service) {
    if (m_pServer != NULL) return CANERR_YETINIT;
    if ((m_pServer = tcp_server_start(service, m_nFrameSize, m_evCallback, m_pParameter, m_nLogging)) != NULL) {
        strncpy(m_szService, service, sizeof(m_szService) - 1);
        m_szService[sizeof(m_szService) - 1] = '\0';
        return CANERR_NOERROR;
    }
    SERVICE_NULL();
    return (CANERR_SYSTEM - errno);
}

CANAPI_Return_t CCanTcpServer::Stop(void) {
    CANAPI_Return_t retVal = CANERR_FATAL;
    if (m_pServer == NULL) return CANERR_NOTINIT;
    retVal = tcp_server_stop(m_pServer);
    SERVICE_NULL();
    SERVER_NULL();
    return (retVal == 0) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}

CANAPI_Return_t CCanTcpServer::Send(const void *data, size_t size) {
    CANAPI_Return_t retVal = CANERR_FATAL;
    // send data over the network
    if (m_pServer == NULL) return CANERR_NOTINIT;
    retVal = tcp_server_send(m_pServer, data, size);
    return (retVal == 0) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}

CANAPI_Return_t CCanTcpServer::Send(CANAPI_Message_t message, uint8_t status, uint8_t extra) {
    CANAPI_Return_t retVal = CANERR_FATAL;
    CANTCP_Message_t packet = {};
    // map CAN API V3 message to RocketCAN message
    rock_msg_from_can(&packet, &message);
    rock_msg_add_status(&packet, status);
    rock_msg_add_extra(&packet, extra);
    // send RocketCAN message over the network
    if (m_pServer == NULL) return CANERR_NOTINIT;
    retVal = tcp_server_send(m_pServer, (void*)&packet, sizeof(packet));
    return (retVal == 0) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}

CANAPI_Return_t CCanTcpServer::SendAbort(uint8_t status) {
    CANAPI_Return_t retVal = CANERR_FATAL;
    CANTCP_Message_t packet = {};
    // make RocketCANabort message
    rock_msg_abort(&packet);
    rock_msg_add_status(&packet, status);
    // send RocketCANabort message over the network
    if (m_pServer == NULL) return CANERR_NOTINIT;
    retVal = tcp_server_send(m_pServer, (void*)&packet, sizeof(packet));
    return (retVal == 0) ? CANERR_NOERROR : (CANERR_SYSTEM - errno);
}

void CCanTcpServer::CanToNet(const CANAPI_Message_t &can, CANTCP_Message_t &net) {
    // map CAN API V3 message to RocketCAN message
    rock_msg_from_can(&net, &can);
}

bool CCanTcpServer::NetToCan(const CANTCP_Message_t &net, CANAPI_Message_t &can) {
    // check RocketCAN message for validity
    if (!rock_msg_is_valid(&net)) {
        return false;
    }
    // map RocketCAN message to CAN API V3 message
    can_tcp_message_t tmp = net;
    rock_msg_to_can(&can, &tmp);
    return true;
}
