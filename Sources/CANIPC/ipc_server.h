/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'ipc_server' - Inter-Process Communication (IPC) server
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
/** @file        ipc_server.h
 *
 *  @brief       Inter-Process Communication (IPC) server.
 *
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 1429 $
 *
 *  @defgroup    ipc Inter-Process Communication (IPC)
 *  @{
 */
#ifndef IPC_SERVER_H_INCLUDED
#define IPC_SERVER_H_INCLUDED

/*  -----------  includes  -----------------------------------------------
 */
#include "ipc_common.h"  /* common definitions for IPC server and client */


/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */
/** @name    Logging options.
 *  @brief   Logging options for the IPC server.
 *  @{ */
#define IPC_LOGGING_NONE  0  /**< no logging */
#define IPC_LOGGING_INFO  1  /**< log client connection */
#define IPC_LOGGING_DATA  2  /**< log data transfer */
#define IPC_LOGGING_ALL   3  /**< log all (data as binary) */
/** @} */

/*  -----------  types  --------------------------------------------------
 */
/** @brief   IPC server descriptor.
 */
typedef struct ipc_server_desc *ipc_server_t;  /* opaque type (requires C99) */


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */
#ifdef __cplusplus
extern "C" {
#endif

/** @brief   Start the server on the specified port.
 *
 *  @param   port       Port number.
 *  @param   sock_type  Socket type (SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET).
 *  @param   mtu_size   Maximum transmission unit (MTU) size.
 *  @param   recv_cbk   Receive callback function.
 *  @param   recv_ref   Receive callback parameter.
 *  @param   logging    Logging options.
 *
 *  @return  IPC server descriptor on success, or NULL on error.
 */
extern ipc_server_t ipc_server_start(unsigned short port, int sock_tyoe, size_t mtu_size,
                                     ipc_event_cbk_t recv_cbk, void *recv_ref, int logging);

/** @brief   Stop the server.
 *
 *  @param   server  IPC server descriptor.
 *
 *  @return  0 on success, or -1 on error.
 */
extern int ipc_server_stop(ipc_server_t server);

/** @brief   Send data to the client.
 *
 *  @param   server  IPC server descriptor.
 *  @param   data    Data to be sent.
 *  @param   size    Size of the data.
 *
 *  @return  0 on success, or -1 on error.
 */
extern int ipc_server_send(ipc_server_t server, const void *data, size_t size);

#ifdef __cplusplus
}
#endif
#endif  /* IPC_SERVER_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
