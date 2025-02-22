/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'crc_j1850' - CRC Calculation (SAE-J1850 CRC8 Standard)
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
/** @file        crc_j1850.c
 *
 *  @brief       CRC Calculation according to SAE-J1850 CRC8 Standard.
 *
 *               Polynomial: x^8 + x^4 + x^3 + x^2 + 1 (0x1D)
 *
 *               Whereby:<br>
 *               - initial value: 0xFF<br>
 *               - final XOR: 0xFF<br>
 *               - CRC width: 8 bits<br>
 *               - input data reflected:  No<br> 
 *               - result data reflected:  No
 *
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 846 $
 *
 *  @addtogroup  crc
 *  @{
 */
#ifndef CRC_J1850_H_INCLUDED
#define CRC_J1850_H_INCLUDED

/*  -----------  includes  -----------------------------------------------
 */
#include "crc.h"


/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */


/*  -----------  types  --------------------------------------------------
 */


/*  -----------  functions  ----------------------------------------------
 */
#ifdef __cplusplus
extern "C" {
#endif

/** @brief  Initialize the CRC8 calculation.
 *
 *  @return  The initial value for the CRC8 calculation.
 */
extern crc8_t crc_j1850_init(void); 

/** @brief  Calculate the CRC8 value.
 *
 *  @param  data    Pointer to the data to be processed.
 *  @param  length  Number of bytes to be processed.
 *  @param  crc     Current CRC8 value (in), updated CRC8 value (out).
 *                  NULL to start with the initial value.
 *
 *  @return  The calculated CRC8 value, including the final XOR value.
 */
extern crc8_t crc_j1850_calc(const void *data, size_t length, crc8_t *crc);

#ifdef __cplusplus
}
#endif
#endif /* CRC_J1850_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
