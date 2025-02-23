/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  CAN Interface API, Version 3 (CAN-over-Ethernet / RocketCAN)
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
/** @file        tcp_can.h
 *
 *  @brief       RocketCAN Message Format
 *
 *  @author      $Author: gonggong $
 *
 *  @version     $Rev: 1466 $
 *
 *  @defgroup    rocketcan RocketCAN - CAN-over-Ethernet
 *  @{
 */
#ifndef TCP_CAN_H_INCLUDED
#define TCP_CAN_H_INCLUDED

/*  -----------  includes  -----------------------------------------------
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <arpa/inet.h>
#else
#include <winsock.h>
#endif

/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */


/*  - - - - - -  CAN API V3 - RocketCAN  - - - - - - - - - - - - - - - - -
 */
/** @name  RocketCAN Identifier
 *  @brief RocketCAN identifier range
 *  @note  Bit 31..29 are reserved for flags
 *  @{ */
#define CANTCP_STD_ID(x)  (uint32_t)((x) & 0x7FFU)  /**< mask: standard identifier */
#define CANTCP_XTD_ID(x)  (uint32_t)((x) & 0x1FFFFFFFU)  /**< mask: extended identifier */

#define CANTCP_ID_VALID(x)  (bool)(((x) & 0xE0000000U) == 0U)  /**< check: valid identifier */
#define CANTCP_ID_BIT31(x)  (bool)(((x) & 0x80000000U) != 0U)  /**< check: bit 31 set */
#define CANTCP_ID_BIT30(x)  (bool)(((x) & 0x40000000U) != 0U)  /**< check: bit 30 set */
#define CANTCP_ID_BIT29(x)  (bool)(((x) & 0x20000000U) != 0U)  /**< check: bit 29 set */
/** @} */

/** @name  RocketCAN Data Length
 *  @brief Maximum payload length (in [byte])
 *  @{ */
#define CANTCP_MAX_LEN  64  /**< max. payload length (CAN FD) */
/** @} */

/** @name  RocketCAN Message Flags
 *  @brief Message flags (8-bit, CAN API Vx compatible)
 *  @{ */
#define CANTCP_XTD_FLAG(x)  (uint8_t)((x) ? 0x01 : 0)  /**< flag: extended format */
#define CANTCP_RTR_FLAG(x)  (uint8_t)((x) ? 0x02 : 0)  /**< flag: remote frame */
#define CANTCP_FDF_FLAG(x)  (uint8_t)((x) ? 0x04 : 0)  /**< flag: CAN FD format */
#define CANTCP_BRS_FLAG(x)  (uint8_t)((x) ? 0x08 : 0)  /**< flag: bit-rate switching */
#define CANTCP_ESI_FLAG(x)  (uint8_t)((x) ? 0x10 : 0)  /**< flag: error state indicator */
#define CANTCP_STS_FLAG(x)  (uint8_t)((x) ? 0x80 : 0)  /**< flag: status message */

#define CANTCP_XTD_MASK  (uint8_t)0x01  /**< mask: extended format */
#define CANTCP_RTR_MASK  (uint8_t)0x02  /**< mask: remote frame */
#define CANTCP_FDF_MASK  (uint8_t)0x04  /**< mask: CAN FD format */
#define CANTCP_BRS_MASK  (uint8_t)0x08  /**< mask: bit-rate switching */
#define CANTCP_ESI_MASK  (uint8_t)0x10  /**< mask: error state indicator */
#define CANTCP_STS_MASK  (uint8_t)0x80  /**< mask: status message */
/** @} */

/** @name  RocketCAN Status Register
 *  @brief Status register (CAN API Vx compatible)
 *  @{ */
#define CANTCP_STAT_RESET    (uint8_t)0x80  /**< CAN status: controller stopped */
#define CANTCP_STAT_BOFF     (uint8_t)0x40  /**< CAN status: busoff status */
#define CANTCP_STAT_EWRN     (uint8_t)0x20  /**< CAN status: error warning level */
#define CANTCP_STAT_BERR     (uint8_t)0x10  /**< CAN status: bus error (LEC) */
#define CANTCP_STAT_TX_BUSY  (uint8_t)0x08  /**< CAN status: transmitter busy */
#define CANTCP_STAT_RX_EMPTY (uint8_t)0x04  /**< CAN status: receiver empty */
#define CANTCP_STAT_MSG_LST  (uint8_t)0x02  /**< CAN status: message lost */
#define CANTCP_STAT_QUE_OVR  (uint8_t)0x01  /**< CAN status: event-queue overrun */
/** @} */
#define CANTCP_MAX_BUSLOAD  (uint16_t)10000 /**< max. bus load (100%) */

/** @name  RocketCAN Control Character
 *  @brief Control character (8-bit, ASCII code)
 *
 *  @note  ETX = end of text (default value, mandatory for client and server)
 *  @note  EOT = end of transmission (server will cancel connection, e.g. after the CAN device is lost)
 *  @note  ETB = end of transmission block (in a sequence of messages, optional for client and server)
 *  @{ */
#define CANTCP_ETX_CHAR  0x03  /**< end of text */
#define CANTCP_EOT_CHAR  0x04  /**< end of transmission */
#define CANTCP_ETB_CHAR  0x17  /**< end of transmission block */  // TODO: Not used yet!
#define CANTCP_CTRLCHAR  CANTCP_ETX_CHAR  /**< default control character */
/** @} */

/*  -----------  types  --------------------------------------------------
 */
#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif
 /** @brief       CAN Message (RocketCAN Format):
  */
typedef struct can_tcp_message_t_ {
    uint32_t id;                        /**< CAN identifier (11-bit or 29-bit) */
    uint8_t  flags;                     /**< message flags (8-bit, CAN API Vx) */
    uint8_t  length;                    /**< data length (in [byte], not CAN DLC!) */
    uint8_t  status;                    /**< status register (8-bit, CAN API Vx) */
    uint8_t  extra;                     /**< unspecific bit-field (8-bit) */  // TODO: Not used yet!
    uint8_t  data[CANTCP_MAX_LEN];      /**< data (to hold CAN FD payload) */
    struct timespec timestamp;          /**< time-stamp { sec, nsec } */
    uint8_t  reserved[4];               /**< reserved (4-byte) */
    uint16_t busload;                   /**< bus load (0 .. 10'000 = 0 .. 100%) */
    uint8_t  ctrlchar;                  /**< control character (ETX or EOT) */
    uint8_t  checksum;                  /**< J1850 checksum (8-bit) */
}
#if defined(__GNUC__) || defined(__clang__)
__attribute__((packed))
#endif
can_tcp_message_t;
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

typedef can_tcp_message_t CANTCP_Message_t;  /**< alias for RocketCAN Message */


/*  - - -  conversion between host and network byte order  - - - - - - - -
 */
#if !defined(__APPLE__)
#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
    #include <intrin.h>
    #define htonll(x) _byteswap_uint64(x)
    #define ntohll(x) _byteswap_uint64(x)
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
    #define ntohll(x) (((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define htonll(x) (x)
    #define ntohll(x) (x)
#else
    #error "Unknown endianness!"
#endif
#endif
#define CANTCP_CAN_ID_HTON(id)  (id) = htonl((id))
#define CANTCP_CAN_ID_NTOH(id)  (id) = ntohl((id))
#define CANTCP_TIMESTAMP_HTON(ts) do { \
    (ts).tv_sec = htonll((uint64_t)(ts).tv_sec); \
    (ts).tv_nsec = htonll((uint64_t)(ts).tv_nsec); \
} while (0)
#define CANTCP_TIMESTAMP_NTOH(ts) do { \
    (ts).tv_sec = ntohll((uint64_t)(ts).tv_sec); \
    (ts).tv_nsec = ntohll((uint64_t)(ts).tv_nsec); \
} while (0)
#define CANTCP_BUSLOAD_HTON(bl)  (bl) = htons((bl))
#define CANTCP_BUSLOAD_NTOH(bl)  (bl) = ntohs((bl))

/** @brief  Convert RocketCAN Message from Host to Network Byte Order.
 *
 *  @param  msg  RocketCAN message (host byte order on input, network byte order on output)
 */
#define CANTCP_MSG_HTON(msg) do { \
    CANTCP_CAN_ID_HTON((msg).id); \
    CANTCP_TIMESTAMP_HTON((msg).timestamp); \
    CANTCP_BUSLOAD_HTON((msg).busload); \
} while (0)

/** @brief  Convert RocketCAN Message from Network to Host Byte Order.
 *
 *  @param  msg  RocketCAN message (network byte order on input, host byte order on output)
 */
#define CANTCP_MSG_NTOH(msg) do { \
    CANTCP_CAN_ID_NTOH((msg).id); \
    CANTCP_TIMESTAMP_NTOH((msg).timestamp); \
    CANTCP_BUSLOAD_NTOH((msg).busload); \
} while (0)

#endif /* TCP_CAN_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
