/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'ipc_common' - Inter-Process Communication (IPC)
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
 *  This module is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This module is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this module.  If not, see <https://www.gnu.org/licenses/>.
 */
/** @file        ipc_common.h
 *
 *  @brief       Inter-Process Communication (IPC) common definitions.
 *
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 1429 $
 *
 *  @defgroup    ipc Inter-Process Communication (IPC)
 *  @{
 */
#ifndef IPC_COMMON_H_INCLUDED
#define IPC_COMMON_H_INCLUDED

/*  -----------  includes  -----------------------------------------------
 */
#include <stdio.h>   /* for type 'size_t' */
#ifdef _MSC_VER
#include <BaseTsd.h>  /* for type 'ssize_t' */
typedef SSIZE_T ssize_t;
#endif

/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */
/** @name    Socket types.
 *  @brief   Socket types for the IPC connection.
 *  @{ */
#define IPC_SOCK_TCP   1  /**< stream socket (for TCP) */
#define IPC_SOCK_UDP   2  /**< datagram socket (for UDP) */
#define IPC_SOCK_SCTP  5  /**< sequenced packet stream (for SCTP) */
/** @} */
#define IPC_MAX_MTU_SIZE  1500  /**< maximum transmission unit (MTU) size */
#define IPC_WAIT_FOREVER  65535U  /**< infinite time-out (blocking operation) */
#define IPC_ADDR_LOCALHOST  "127.0.0.1"  /**< local host address (IPv4) */


/*  -----------  types  --------------------------------------------------
 */
/** @brief   IPC event callback function.
 *
 *  @param   data  The event data.
 *  @param   size  Size of the data.
 *  @param   para  An event reference.
 *
 *  @return  0 on success, or a negative value on error.
 */
typedef int (*ipc_event_cbk_t)(const void *, size_t, void *);


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif  /* IPC_COMMON_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
