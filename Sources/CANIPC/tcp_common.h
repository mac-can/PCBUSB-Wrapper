/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'tcp_common' - Stream Socket Communication (TCP/IP)
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
/** @file        tcp_common.h
 *
 *  @brief       Common definitions for Stream Socket communication (TCP/IP).
 *
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 1452 $
 *
 *  @defgroup    tcp Stream Socket Communication (TCP/IP)
 *  @{
 */
#ifndef TCP_COMMON_H_INCLUDED
#define TCP_COMMON_H_INCLUDED

/*  -----------  includes  -----------------------------------------------
 */
#include <stdio.h>   /* for type 'size_t' */
#ifdef _MSC_VER
#include <BaseTsd.h>  /* for type 'ssize_t' */
typedef SSIZE_T ssize_t;
#endif

/*  -----------  options  ------------------------------------------------
 */

/** @name  Compiler Switches
 *  @brief Options for conditional compilation.
 *  @{ */
/** @note  Set define OPTION_TCPIP_TCPDELAY to a non-zero value to compile
 *         without socket option TCP_NODELAY. (e.g. in the build environment).
 *         The socket option TCP_NODELAY disables the Nagle algorithm, which
 *         is used to reduce the number of small packets sent over the network.
 *         *) Option TCP_NODELAY is set by default for use with RocketCAN.
 */
/** @note  Set define OPTION_TCPIP_BACKLOG to a suitalbe value to set the
 *         maximun number of pending connections in the listening queue (e.g.
 *         in the build environment). The default value is 5.
 *         *) This value is a common default in many network applications and
 *         operating systems, as it provides a reasonable balance between allowing
 *         multiple simultaneous connection attempts and limiting resource usage.
 */
#ifndef OPTION_DISABLED
#define OPTION_DISABLED  0  /**< if a define is not defined, it is automatically set to 0 */
#endif
/** @} */

/*  -----------  defines  ------------------------------------------------
 */
#define TCP_MTU_SIZE  1500  /**< maximum transmission unit (MTU) size */
#define TCP_MSS_SIZE  1460  /**< maximum segment size (MSS) for TCP */
#define TCP_BUF_SIZE  TCP_MSS_SIZE  /**< data buffer size */
#define TCP_WAIT_FOREVER  65535U  /**< infinite time-out (blocking operation) */
#define TCP_IPv4_LOCALHOST  "127.0.0.1"  /**< local host address (IPv4) */
#define TCP_IPv6_LOCALHOST  "::1"  /**< local host address (IPv6) */


/*  -----------  types  --------------------------------------------------
 */
/** @brief   TCP/IP receive event callback function.
 *
 *  @param   data  The event data.
 *  @param   size  Size of the data.
 *  @param   para  An event reference.
 *
 *  @return  0 on success, or a negative value on error.
 */
typedef int (*tcp_event_cbk_t)(const void *, size_t, void *);


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
#endif  /* TCP_COMMON_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
