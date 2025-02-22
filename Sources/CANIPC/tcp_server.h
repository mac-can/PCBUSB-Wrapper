/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'tcp_server' - Stream Socket Server (TCP/IP)
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
/** @file        tcp_server.h
 *
 *  @brief       Stream Socket Server (TCP/IP).
 *
 *  @note        Reception of client data is handled by a user-defined
 *               callback function in a thread.
 *
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 1452 $
 *
 *  @addtogroup  tcp
 *  @{
 */
#ifndef TCP_SERVER_H_INCLUDED
#define TCP_SERVER_H_INCLUDED

/*  -----------  includes  -----------------------------------------------
 */
#include "tcp_common.h"  /* common definitions for TCP/IP server and client */


/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */
/** @name    Logging options.
 *  @brief   Logging options for the TCP/IP server.
 *  @{ */
#define TCP_LOGGING_NONE  0  /**< no logging */
#define TCP_LOGGING_INFO  1  /**< log client connection */
#define TCP_LOGGING_DATA  2  /**< log data transfer */
#define TCP_LOGGING_ALL   3  /**< log all (data as binary) */
/** @} */

/*  -----------  types  --------------------------------------------------
 */
/** @brief   TCP/IP server descriptor.
 */
typedef struct tcp_server_desc *tcp_server_t;  /* opaque type (requires C99) */


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */
#ifdef __cplusplus
extern "C" {
#endif

/** @brief   Start the server on the specified port.
 *
 *  @param   service    Service name or port number.
 *  @param   data_size  Data size.
 *  @param   recv_cbk   Receive callback function.
 *  @param   recv_ref   Receive callback parameter.
 *  @param   logging    Logging options.
 *
 *  @return  TCP/IP server descriptor on success, or NULL on error.
 */
extern tcp_server_t tcp_server_start(const char *service, size_t data_size,
                                     tcp_event_cbk_t recv_cbk, void *recv_ref, int logging);

/** @brief   Stop the server.
 *
 *  @param   server  TCP/IP server descriptor.
 *
 *  @return  0 on success, or -1 on error.
 */
extern int tcp_server_stop(tcp_server_t server);

/** @brief   Send data to connected clients.
 *
 *  @param   server  TCP/IP server descriptor.
 *  @param   data    Data to be sent.
 *  @param   size    Size of the data.
 *
 *  @return  0 on success, or -1 on error.
 */
extern int tcp_server_send(tcp_server_t server, const void *data, size_t size);

#ifdef __cplusplus
}
#endif
#endif  /* TCP_SERVER_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
