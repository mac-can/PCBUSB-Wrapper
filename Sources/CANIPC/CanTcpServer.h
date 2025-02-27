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
#ifndef CAN_TCP_SERVER_H_INCLUDED
#define CAN_TCP_SERVER_H_INCLUDED

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
#ifndef NI_MAXSERV
#define NI_MAXSERV  32  ///< maximum length of a service name or port number
#endif
typedef struct tcp_server_desc *tcp_server_t;  ///< forwards declaration

/// \name   CAN TCP/IP Server
/// \brief  CAN-over-Ethernet Server with RocketCAN frame format.
/// \{
class CCanTcpServer {
public:
private:
    char m_szService[NI_MAXSERV];  ///< Service name or port number
    size_t m_nFrameSize;           ///< Frame size (in bytes)
    tcp_event_cbk_t m_evCallback;  ///< Event callback function
    void *m_pParameter;            ///< Event callback parameter
    tcp_server_t m_pServer;        ///< TCP/IP server descriptor
    int m_nLogging;                ///< Logging level (0 = none)
public:
    /// @brief  Constructor (default frame format is RocketCAN).
    ///
    CCanTcpServer();

    /// @brief  Destructor.
    ///
    ~CCanTcpServer();

    /// @brief  Set the event callback function.
    ///
    /// @note   The server must not be running.
    ///
    /// @param  callback   Event callback function
    /// @param  parameter  Event callback parameter
    ///
    /// @return true if the callback function has been set, or false on error
    ///
    bool SetCallback(tcp_event_cbk_t callback, void *parameter = NULL) {
        if (m_pServer != NULL) return false;
        m_evCallback = callback;
        m_pParameter = parameter;
        return true;
    }
    /// @brief  Set the logging level.
    ///
    /// @note   The server must not be running.
    ///
    /// @param  level  Logging level (0 = none)
    ///
    /// @return true if the logging level has been set, or false on error
    ///
    bool SetLoggingLevel(int level) {
        if (m_pServer != NULL) return false;
        m_nLogging = level;
        return true;
    }
    /// @brief  Get the service name or port number.
    ///
    /// @return Service name or port number
    ///
    const char *GetService() { return (const char *)m_szService; }

    /// @brief  Get the frame size.
    ///
    /// @return Frame size (in bytes)
    ///
    size_t GetFrameSize() { return m_nFrameSize; }

    /// @brief  Check if the TCP/IP server is running.
    ///
    /// @return true if the TCP/IP server is running, or false otherwise
    ///
    bool IsRunning() { return (m_pServer != NULL) ? true : false; }

    /// @brief  Start the TCP/IP server on the specified port and accept incoming connections.
    ///
    /// @param  service  Service name or port number
    ///
    /// @return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Start(const char *service);

    /// @brief  Send data over the network.
    ///
    /// @param  data  Data to be sent (in network byte order)
    /// @param  size  Size of the data (must match the frames size)
    ///
    /// @return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Send(const void *data, size_t size);

    /// @brief  Send a CAN message over the network (RocketCAN frame format).
    ///
    /// @param  message  CAN message (CAN API V3 format)
    /// @param  status   CAN status register (as 8-bit value)
	/// @param  extra	 Additional information (as 8-bit value)
    ///
    /// @return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Send(CANAPI_Message_t message, uint8_t status = 0x00U, uint8_t extra = 0U);

    /// @brief  Send an abort message over the network.
    ///
    /// @param  status  CAN status register (as 8-bit value)
    ///
    /// @return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t SendAbort(uint8_t status = 0x00U);

    /// @brief  Stop the TCP/IP server.
    ///
    /// @return 0 on success, or a negative value on error
    ///
    CANAPI_Return_t Stop();
public:
    /// @brief  Set the frame size.
    ///
    /// @warning  This method is intended for testing purposes only!
    ///
    /// @note   The server must not be running.
    ///
    /// @param  size  Frame size (in bytes)
    ///
    /// @return true if the frame size has been set, or false on error
    ///
    bool SetFrameSize(size_t size) {
        if (m_pServer != NULL) return false;
        m_nFrameSize = size;
        return true;
    }
public:
    /// @brief  Convert CAN API V3 message to RocketCAN message.
    ///
    /// @param  can  CAN API V3 message (host byte order)
    /// @param  net  RocketCAN message (network byte order)
    ///
    static void CanToNet(const CANAPI_Message_t &can, CANTCP_Message_t &net);

    /// @brief  Convert RocketCAN message to CAN API V3 message.
    ///
    /// @param  net  RocketCAN message (network byte order)
    /// @param  can  CAN API V3 message (host byte order)
    ///
    /// @return true if the RocketCAN message is valid, or false otherwise
    ///
    static bool NetToCan(const CANTCP_Message_t &net, CANAPI_Message_t &can);
};
/// \}

#endif  // CAN_TCP_SERVER_H_INCLUDED
