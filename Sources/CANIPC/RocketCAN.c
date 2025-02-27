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
/** @file        RocketCAN.c
 *
 *  @brief       CAN API V3 message to RocketCAN message and vice versa.
 *
 *  @author      $Author: quaoar $
 *
 *  @version     $Rev: 1469 $
 *
 *  @addtogroup  rocketcan
 *  @{
 */
#include "RocketCAN.h"
#include "crc_j1850.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <time.h>


/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))


/*  -----------  types  --------------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */
static uint8_t dlc2len(uint8_t dlc);
static uint8_t len2dlc(uint8_t len);


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  functions  ----------------------------------------------
 */
bool rock_msg_is_valid(const can_tcp_message_t *msg) {
    /* sanity check */
    if (msg == NULL) {
        return false;
    }
    /* check for correct checksum */
    if (msg->checksum != crc_j1850_calc((const uint8_t*)msg, 
            sizeof(can_tcp_message_t) - sizeof(msg->checksum), NULL)) {
        return false;
    }
    /* check for control character (ETX) */
    if (msg->ctrlchar != CANTCP_CTRLCHAR) {
        return false;
    }
    /* RocketCAN message is valid */
    return true;
}

void rock_msg_to_can(can_message_t *can, can_tcp_message_t *net) {
    /* sanity check */
    if (can == NULL || net == NULL) {
        return;
    }
    memset(can, 0, sizeof(can_message_t));
    /* convert RocketCAN message from network to host byte order */
    CANTCP_MSG_NTOH(*net);
    /* map RocketCAN message to CAN message (CAN API V3) */
    can->id = net->id;
    can->xtd = (net->flags & CANTCP_XTD_MASK) ? 1 : 0;
    can->rtr = (net->flags & CANTCP_RTR_MASK) ? 1 : 0;
    can->fdf = (net->flags & CANTCP_FDF_MASK) ? 1 : 0;
    can->brs = (net->flags & CANTCP_BRS_MASK) ? 1 : 0;
    can->esi = (net->flags & CANTCP_ESI_MASK) ? 1 : 0;
    can->sts = (net->flags & CANTCP_STS_MASK) ? 1 : 0;
    can->dlc = len2dlc(net->length);
    memcpy(can->data, net->data, MAX(CANFD_MAX_LEN, CANTCP_MAX_LEN));
}

void rock_msg_from_can(can_tcp_message_t *net, const can_message_t *can) {
    /* sanity check */
    if (net == NULL || can == NULL) {
        return;
    }
    memset(net, 0, sizeof(can_tcp_message_t));
    /* map CAN message (CAN API V3) to RocketCAN message */
    net->id = can->id;
    net->flags = CANTCP_XTD_FLAG(can->xtd) | CANTCP_RTR_FLAG(can->rtr) |
                 CANTCP_FDF_FLAG(can->fdf) | CANTCP_BRS_FLAG(can->brs) |
                 CANTCP_ESI_FLAG(can->esi) | CANTCP_STS_FLAG(can->sts);
    net->length = dlc2len(can->dlc);
	net->ts_sec = (uint64_t)can->timestamp.tv_sec;
	net->ts_nsec = (uint32_t)can->timestamp.tv_nsec;
    memcpy(net->data, can->data, MAX(CANTCP_MAX_LEN, CANFD_MAX_LEN));
    /* convert RocketCAN message from host to network byte order */
    CANTCP_MSG_HTON(*net);
    /* set control character (ETX) */
    net->ctrlchar = CANTCP_CTRLCHAR;
    /* calculate and store the CRC checksum */
    net->checksum = crc_j1850_calc((const uint8_t*)net, 
            sizeof(can_tcp_message_t) - sizeof(net->checksum), NULL);
}

void rock_msg_add_status(can_tcp_message_t *net, uint8_t status) {
    /* sanity check */
    if (net == NULL) {
        return;
    }
    /* add CAN status register to RocketCAN message */
    net->status = status;
    /* update the CRC checksum */
    net->checksum = crc_j1850_calc((const uint8_t*)net, 
            sizeof(can_tcp_message_t) - sizeof(net->checksum), NULL);
}

void rock_msg_add_extra(can_tcp_message_t *net, uint8_t extra) {
    /* sanity check */
    if (net == NULL) {
        return;
    }
    /* add extrabreit to RocketCAN message */
	net->extra = extra;
    /* update the CRC checksum */
    net->checksum = crc_j1850_calc((const uint8_t*)net, 
            sizeof(can_tcp_message_t) - sizeof(net->checksum), NULL);
}

void rock_msg_abort(can_tcp_message_t *net) {
    /* sanity check */
    if (net == NULL) {
        return;
    }
    /* create RocketCAN abort message */
    memset(net, 0, sizeof(can_tcp_message_t));
    net->id = 0x001U;
    net->flags = CANTCP_STS_FLAG(1U);
    net->length = 4U;
    net->status = CANSTAT_RESET;
    net->data[3] = CANSTAT_RESET;
	/* get current system time in UTC */
    struct timespec now;
#if !defined(_MSC_VER) && !defined(_WIN32) && !defined(_WIN64)
    if (clock_gettime(CLOCK_REALTIME, &now) == 0)
#else
	if (timespec_get(&now, TIME_UTC) == TIME_UTC)
#endif
    {
		net->ts_sec = (uint64_t)now.tv_sec;
		net->ts_nsec = (uint32_t)now.tv_nsec;
    }
    /* set control character (EOT) */
    net->ctrlchar = CANTCP_EOT_CHAR;
    /* convert RocketCAN message from host to network byte order */
    CANTCP_MSG_HTON(*net);
    /* calculate and store the CRC checksum */
    net->checksum = crc_j1850_calc((const uint8_t*)net, 
            sizeof(can_tcp_message_t) - sizeof(net->checksum), NULL);
}

bool rock_msg_is_abort(const can_tcp_message_t *msg) {
    /* sanity check */
    if (msg == NULL) {
        return false;
    }
    /* check for correct checksum */
    if (msg->checksum != crc_j1850_calc((const uint8_t*)msg, 
            sizeof(can_tcp_message_t) - sizeof(msg->checksum), NULL)) {
        return false;
    }
    /* check for control character (EOT) */
    if (msg->ctrlchar != CANTCP_EOT_CHAR) {
        return false;
    }
    /* abort message is valid */
    return true;
}

/*  -----------  local functions  ----------------------------------------
 */
static uint8_t dlc2len(uint8_t dlc) {
    static const uint8_t dlc_table[16] = {
        0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U
    };
    return dlc_table[(dlc < 16U) ? dlc : 15U];
}

static uint8_t len2dlc(uint8_t len) {
    if(len > 48U) return 0x0FU;
    if(len > 32U) return 0x0EU;
    if(len > 24U) return 0x0DU;
    if(len > 20U) return 0x0CU;
    if(len > 16U) return 0x0BU;
    if(len > 12U) return 0x0AU;
    if(len > 8U) return 0x09U;
    return len;
}

/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
