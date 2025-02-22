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
#ifndef CAN_TCP_CLIENT_H_INCLUDED
#define CAN_TCP_CLIENT_H_INCLUDED

#include "CANAPI.h"
#include "tcp_can.h"
#include "tcp_common.h"

/// \name  Compiler Switches
/// \brief Options for conditional compilation.
/// \{
/// \note  Do not set define OPTION_CAN_2_0_ONLY to a non-zero value to compile
///        with CAN 2.0 frame format only. This frame format is not supported!
///
#ifndef OPTION_DISABLED
#define OPTION_DISABLED  0  ///< if a define is not defined, it is automatically set to 0
#endif
#if (OPTION_CAN_2_0_ONLY != OPTION_DISABLED)
#error Compilation with legacy CAN 2.0 frame format!
#endif
/// @}

/// \name   CAN TCP/IP Client
/// \brief  CAN-over-Ethernet Client with RocketCAN frame format.
/// \{
class CCanTcpClient {
private:
    size_t m_nFrameSize;  ///< Frame size (in bytes)
    int m_nSocket;  ///< Socket file descriptor
public:
    /// \brief  Constructor (default frame format is RocketCAN).
    ///
    CCanTcpClient();
    
    /// \brief  Destructor.
    ///
    ~CCanTcpClient();
    
    /// \brief  Get the frame size.
    ///
    /// \return Frame size (in bytes)
    ///
    size_t GetFrameSize() { return m_nFrameSize; }

    /// \brief  Check if the TCP/IP client is connected.
    ///
    /// \return true if the TCP/IP client is connected, or false otherwise
    ///
    bool IsConnected() { return (m_nSocket >= 0) ? true : false; }

    /// \brief  Connect to a listening TCP/IP server.
    ///
    /// \param  server  Server address ("<host>:<port>")
    ///
    /// \return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Connect(const char *server);
    
    /// \brief  Disconnect from TCP/IP server.
    ///
    /// \return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Disconnect();
    
    /// \brief  Receive a CAN message from the network.
    ///
    /// \param  message  CAN message (CAN API V3 format)
    /// \param  timeout  Timeout in milliseconds
    ///
    /// \return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Receive(CANAPI_Message_t &message, uint16_t timeout = CANWAIT_INFINITE);
    
    /// \brief  Send a CAN message over the network.
    ///
    /// \param  message      CAN message (CAN API V3 format)
    /// \param  inhibitTime  Inhibit time in milliseconds
    ///
    /// \return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Send(CANAPI_Message_t message, uint16_t inhibitTime = 0U);

    /// \brief  Receive data from the network.
    ///
    /// \param  data     Data buffer
    /// \param  size     Size of the data buffer
    /// \param  timeout  Timeout in milliseconds
    ///
    /// \return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Receive(void *data, size_t size, uint16_t timeout = CANWAIT_INFINITE);

    /// \brief  Send data over the network.
    ///
    /// \param  data         Data to be sent (in network byte order)
    /// \param  size         Size of the data (must match the frame size)
    /// \param  inhibitTime  Inhibit time in milliseconds
    ///
    /// \return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Send(const void *data, size_t size, uint16_t inhibitTime = 0U);

    /// \brief  Get local host address with port number.
    ///
    /// \param  port  Port number
    ///
    /// \return Local host address with port number
    ///
    static const char *localhost(const char *port) {
        static char server[100];
        if (port)
            snprintf(server, sizeof(server), TCP_IPv4_LOCALHOST ":%s", port);
        else
            snprintf(server, sizeof(server), TCP_IPv4_LOCALHOST ":0");
        return (const char *)server;
    }
public:
    /// \brief  Set the frame size.
    ///
    /// \warning  This method is intended for testing purposes only!
    ///
    /// \note   The client must not be connected.
    ///
    /// \param  size  Frame size (in bytes)
    ///
    /// \return true if the frame size has been set, or false on error
    ///
    bool SetFrameSize(size_t size) {
        if (m_nSocket >= 0) return false;
        m_nFrameSize = size;
        return true;
    }
};
/// \}

#endif  // CAN_TCP_CLIENT_H_INCLUDED
