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
/** @file        RocketCAN.h
 *
 *  @brief       CAN API V3 message to RocketCAN message and vice versa.
 *
 *  @note        RocketCAN messages are transmitted in network byte order. 
 *
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 1453 $
 *
 *  @addtogroup  rocketcan
 *  @{
 */
#ifndef ROCKETCAN_H_INCLUDED
#define ROCKETCAN_H_INCLUDED

/*  -----------  includes  -----------------------------------------------
 */
#include "CANAPI_Types.h"  /* CAN API V3 message and return types */
#include "tcp_can.h"       /* CAN-over-Ethernet message: RocketCAN */


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

/** @brief  Check if RocketCAN message is valid.
 * 
 *          The function checks for the correct CRC checksum
 *          and the correct control character (ETX).
 *
 *  @remark The server can also send an abort message with the
 *          control character EOT in case of an error. On the
 *          client side, the function 'rock_msg_is_abort' checks
 *          for the control character EOT instead of ETX.
 *
 *  @param  msg  RocketCAN message (network byte order)
 *
 *  @return  true if message is valid, false otherwise
 */
extern bool rock_msg_is_valid(const can_tcp_message_t *msg);

/** @brief  Convert RocketCAN message to CAN API V3 message.
 *
 *  @note   The RocketCAN message must be valid and in network byte order.
 * 
 *  @note   The RocketCAN message is afterwards in host byte order.
 *
 *  @param  can  CAN API V3 message (host byte order)
 *  @param  net  RocketCAN message (network byte order)
 */
extern void rock_msg_to_can(can_message_t *can, can_tcp_message_t *net);

/** @brief  Convert CAN API V3 message to RocketCAN message.
 * 
 *          The function sets the control character (ETX)
 *          and calculates and sets the CRC checksum.
 *
 *  @param  net  RocketCAN message (network byte order)
 *  @param  can  CAN API V3 message (host byte order)
 */
extern void rock_msg_from_can(can_tcp_message_t *net, const can_message_t *can);

/** @brief  Add CAN status register to RocketCAN message.
 *
 *          The function also updates the CRC checksum.
 *
 *  @param  net     RocketCAN message (network byte order)
 *  @param  status  CAN status register (8-bit value)
 */
extern void rock_msg_add_status(can_tcp_message_t *net, uint8_t status);

/** @brief  Add CAN bus load to RocketCAN message.
 *
 *          The function also updates the CRC checksum.
 *
 *  @param  net      RocketCAN message (network byte order)
 *  @param  busload  CAN bus load (0 .. 10'000 ==> 0 .. 255)
 */
extern void rock_msg_add_busload(can_tcp_message_t *net, uint16_t busload);

/** @brief  Create RocketCAN abort message to signal to all clients that
 *          the server is shutting down.
 *
 *          RocketCAN abort message:
 *          - CAN identifier = 0x001
 *          - flags = 0x80 (STS)
 *          - length = 4
 *          - status = 0x80 (offline)
 *          - data = 0x00 0x00 0x00 0x80
 *          - time-stamp = current time
 *          - control character = EOT (end of transmission)
 *
 *  @param  net  RocketCAN message (network byte order)
 */
extern void rock_msg_abort(can_tcp_message_t *net);

/** @brief  Check if RocketCAN message is an abort message.
 *
 *          The function checks for the correct CRC checksum
 *          and for the control character EOT.
 *
 *  @param  msg  RocketCAN message (network byte order)
 *
 *  @return  true if message is an abort message, false otherwise
 */
extern bool rock_msg_is_abort(const can_tcp_message_t *msg);

#ifdef __cplusplus
}
#endif
#endif  /* ROCKETCAN_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
