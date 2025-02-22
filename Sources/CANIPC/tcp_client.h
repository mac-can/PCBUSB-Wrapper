/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'tcp_client' - Stream Socket Client (TCP/IP)
 *
 *  This module is dual-licensed under the BSD 2-Clause "Simplified" License
 *  and under the GNU General Public License v2.0 (or any later version).
 *  You can choose between one of them if you use this module.
 *
 *  (1) BSD 2-Clause "Simplified" License
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  THIS MODULE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS MODULE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  (2) GNU General Public License v2.0 or later
 *
 *  This module is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This module is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this module; if not, see <https://www.gnu.org/licenses/>.
 */
/** @file        tcp_client.h
 *
 *  @brief       Stream Socket Client (TCP/IP).
 *
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 1452 $
 *
 *  @addtogroup  tcp
 *  @{
 */
#ifndef TCP_CLIENT_H_INCLUDED
#define TCP_CLIENT_H_INCLUDED

/*  -----------  includes  -----------------------------------------------
 */
#include "tcp_common.h"  /* common definitions for TCP/IP server and client */


/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */


/*  -----------  types  --------------------------------------------------
 */


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */
#ifdef __cplusplus
extern "C" {
#endif

/** @brief   Open a connection to the server.
 *
 *  @param   server     The server address ("<host>:<port>").
 *
 *  @return  The file descriptor of the client socket or -1 on error.
 */
extern int tcp_client_connect(const char *server);

/** @brief   Close the connection to the server.
 *
 *  @param   fildes  The file descriptor of the client socket.
 *
 *  @return  0 on success, -1 on error.
 */
extern int tcp_client_close(int fildes);

/** @brief   Send data to the server.
 *
 *  @param   fildes  The file descriptor of the client socket.
 *  @param   buffer  The data to be sent.
 *  @param   length  The length of the data to be sent.
 *
 *  @return  The number of bytes sent or -1 on error.
 */
extern ssize_t tcp_client_send(int fildes, const void *buffer, size_t length);

/** @brief   Receive data from the server.
 *
 *  @param   fildes   The file descriptor of the client socket.
 *  @param   buffer   The buffer to store the received data.
 *  @param   length   The length of the buffer.
 *  @param   timeout  The timeout in milliseconds:
 *                    0 means the function returns immediately,
 *                    65535 means blocking read, and any other
 *                    value means the time to wait im milliseconds
 *
 *  @return  The number of bytes received or -1 on error.
 */
extern ssize_t tcp_client_recv(int fildes, void *buffer, size_t length, unsigned short timeout);

#ifdef __cplusplus
}
#endif
#endif  /* TCP_CLIENT_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
