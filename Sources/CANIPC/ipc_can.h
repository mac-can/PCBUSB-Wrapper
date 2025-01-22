/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  CAN Interface API, Version 3 (CAN IPC Interface)
 *
 *  Copyright (c) 2004-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  This file is part of CAN API V3.
 *
 *  CAN API V3 is dual-licensed under the BSD 2-Clause "Simplified" License
 *  and under the GNU General Public License v2.0 (or any later version). You can
 *  choose between one of them if you use CAN API V3 in whole or in part.
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
 *  CAN API V3 IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF CAN API V3, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  (2) GNU General Public License v2.0 or later
 *
 *  CAN API V3 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CAN API V3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with CAN API V3; if not, see <https://www.gnu.org/licenses/>.
 */
/** @file        ipc_can.h
 *
 *  @brief       CAN IPC Message (RocketCAN Format)
 *
 *  @author      $Author$
 *
 *  @version     $Rev$
 *
 *  @addtogroup  ipc
 *  @{
 */
#ifndef CANIPC_MESSAGE_H_INCLUDED
#define CANIPC_MESSAGE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*  -----------  includes  -----------------------------------------------
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>


/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */

/** @name  RocketCAN Identifier
 *  @brief RocketCAN identifier range
 *  @note  Bit 31..29 are reserved for flags
 *  @{ */
#define CANIPC_STD_ID(x)  (uint32_t)((x) & 0x7FFU)  /**< mask: standard identifier */
#define CANIPC_XTD_ID(x)  (uint32_t)((x) & 0x1FFFFFFFU)  /**< mask: extended identifier */

#define CANIPC_ID_VALID(x)  (bool)(((x) & 0xE0000000U) == 0U)  /**< check: valid identifier */
#define CANIPC_ID_BIT31(x)  (bool)(((x) & 0x80000000U) != 0U)  /**< check: bit 31 set */
#define CANIPC_ID_BIT30(x)  (bool)(((x) & 0x40000000U) != 0U)  /**< check: bit 30 set */
#define CANIPC_ID_BIT29(x)  (bool)(((x) & 0x20000000U) != 0U)  /**< check: bit 29 set */
/** @} */

/** @name  RocketCAN Data Length
 *  @brief Maximum payload length (in [byte])
 *  @{ */
#define CANIPC_MAX_LEN  64  /**< max. payload length (CAN FD) */
/** @} */

/** @name  RocketCAN Message Flags
 *  @brief Message flags (8-bit, CAN API Vx compatible)
 *  @{ */
#define CANIPC_XTD_FLAG(x)  (uint8_t)((x) ? 0x01 : 0)  /**< flag: extended format */
#define CANIPC_RTR_FLAG(x)  (uint8_t)((x) ? 0x02 : 0)  /**< flag: remote frame */
#define CANIPC_FDF_FLAG(x)  (uint8_t)((x) ? 0x04 : 0)  /**< flag: CAN FD format */
#define CANIPC_BRS_FLAG(x)  (uint8_t)((x) ? 0x08 : 0)  /**< flag: bit-rate switching */
#define CANIPC_ESI_FLAG(x)  (uint8_t)((x) ? 0x10 : 0)  /**< flag: error state indicator */
#define CANIPC_STS_FLAG(x)  (uint8_t)((x) ? 0x80 : 0)  /**< flag: status message */

#define CANIPC_XTD_MASK  (uint8_t)0x01  /**< mask: extended format */
#define CANIPC_RTR_MASK  (uint8_t)0x02  /**< mask: remote frame */
#define CANIPC_FDF_MASK  (uint8_t)0x04  /**< mask: CAN FD format */
#define CANIPC_BRS_MASK  (uint8_t)0x08  /**< mask: bit-rate switching */
#define CANIPC_ESI_MASK  (uint8_t)0x10  /**< mask: error state indicator */
#define CANIPC_STS_MASK  (uint8_t)0x80  /**< mask: status message */
/** @} */

/*  -----------  types  --------------------------------------------------
 */

/** @brief       CAN Message (IPC Format):
 */
typedef struct can_ipc_message_t_ {
    uint32_t id;                        /**< CAN identifier (11-bit or 29-bit) */
    uint8_t  flags;                     /**< message flags (8-bit, CAN API Vx) */
    uint8_t  length;                    /**< data length (in [byte], not CAN DLC!) */
    uint16_t extra;                     /**< additional flags (16-bit, for internal use) */
    uint8_t  data[CANIPC_MAX_LEN];      /**< data (to hold CAN FD payload) */
    struct timespec timestamp;          /**< time-stamp { sec, nsec } */
} can_ipc_message_t __attribute__((aligned(8)));


/*  - - -  conversion between host and network byte order  - - - - - - - -
 */
#include <arpa/inet.h>

/** @brief  Convert CAN Message (IPC Format) between Host and Network Byte Order.
 * 
 *  @param  msg  IPC CAN message (host byte order on input, network byte order on output)
 */
#define CAN_IPC_MSG_HTON(msg) do { \
    (msg).id = htonl((msg).id); \
    (msg).extra = htons((msg).extra); \
    (msg).timestamp.tv_sec = htonl((msg).timestamp.tv_sec); \
    (msg).timestamp.tv_nsec = htonl((msg).timestamp.tv_nsec); \
} while (0)

/** @brief  Convert CAN Message (IPC Format) from Network to Host Byte Order.
 * 
 *  @param  msg  IPC CAN message (network byte order on input, host byte order on output)
 */
#define CAN_IPC_MSG_NTOH(msg) do { \
    (msg).id = ntohl((msg).id); \
    (msg).extra = ntohs((msg).extra); \
    (msg).timestamp.tv_sec = ntohl((msg).timestamp.tv_sec); \
    (msg).timestamp.tv_nsec = ntohl((msg).timestamp.tv_nsec); \
} while (0)

#ifdef __cplusplus
}
#endif
#endif /* CANIPC_MESSAGE_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
