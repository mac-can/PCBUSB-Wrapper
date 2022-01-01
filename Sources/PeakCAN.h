//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (for PEAK PCAN-USB Interfaces)
//
//  Copyright (c) 2012-2022 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
//  All rights reserved.
//
//  This file is part of PCBUSB-Wrapper.
//
//  PCBUSB-Wrapper is dual-licensed under the BSD 2-Clause "Simplified" License
//  and under the GNU General Public License v3.0 (or any later version). You can
//  choose between one of them if you use PCBUSB-Wrapper in whole or in part.
//
//  BSD 2-Clause "Simplified" License:
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this
//     list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  PCBUSB-Wrapper IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF PCBUSB-Wrapper, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  GNU General Public License v3.0 or later:
//  PCBUSB-Wrapper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  PCBUSB-Wrapper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with PCBUSB-Wrapper.  If not, see <https://www.gnu.org/licenses/>.
//
#ifndef PEAKCAN_H_INCLUDED
#define PEAKCAN_H_INCLUDED

#include "PeakCAN_Defines.h"
#include "CANAPI.h"

/// \name   PeakCAN
/// \brief  PeakCAN dynamic library
/// \{
#define PEAKCAN_LIBRARY_ID  CANLIB_PCANBASIC
#define PEAKCAN_LIBRARY_NAME  CANDLL_PCANBASIC
#define PEAKCAN_LIBRARY_VENDOR  "UV Software, Berlin"
#define PEAKCAN_LIBRARY_LICENSE  "BSD-2-Clause OR GPL-3.0-or-later"
#define PEAKCAN_LIBRARY_COPYRIGHT  "Copyright (c) 2012-2022  Uwe Vogt, UV Software, Berlin"
#define PEAKCAN_LIBRARY_HAZARD_NOTE  "If you connect your CAN device to a real CAN network when using this library,\n" \
                                     "you might damage your application."
/// \}


/// \name   PeakCAN API
/// \brief  CAN API V3 driver for PEAK PCAN-USB interfaces
/// \note   See CCanApi for a description of the overridden methods
/// \{
class CANCPP CPeakCAN : public CCanApi {
private:
    CANAPI_Handle_t m_Handle;  ///< CAN interface handle
    CANAPI_OpMode_t m_OpMode;  ///< CAN operation mode
    CANAPI_Bitrate_t m_Bitrate;  ///< CAN bitrate settings
    struct {
        uint64_t u64TxMessages;  ///< number of transmitted CAN messages
        uint64_t u64RxMessages;  ///< number of received CAN messages
        uint64_t u64ErrorFrames;  ///< number of received status messages
    } m_Counter;
public:
    // constructor / destructor
    CPeakCAN();
    ~CPeakCAN();
    // CPeakCAN-specific error codes (CAN API V3 extension)
    enum EErrorCodes {
        // note: range 0...-99 is reserved by CAN API V3
        GeneralError = VendorSpecific, ///< mapped PCAN-Basic error codes
        RegisterTestFailed     = -201, ///< PCAN_ERROR_REGTEST: test of the CAN controller hardware registers failed (no hardware found)
        DriverNotLoaded        = -202, ///< PCAN_ERROR_NODRIVER: driver not loaded
        HardwareAlreadyInUse   = -203, ///< PCAN_ERROR_HWINUSE: hardware is in use by another Net
        ClientAlreadyConnected = -204, ///< PCAN_ERROR_NETINUSE: a client is already connected to the Net
        HardwareHandleIsWrong  = -220, ///< PCAN_ERROR_ILLHW: hardware handle is wrong
        NetworkHandleIsWrong   = -224, ///< PCAN_ERROR_ILLNET: net handle is wrong
        ClientHandleIsWrong    = -228, ///< PCAN_ERROR_ILLCLIENT: client handle is wrong
        ResourceNotCreated     = -232, ///< PCAN_ERROR_RESOURCE: resource (FIFO, Client, timeout) cannot be created
        InvalidParameterType   = -264, ///< PCAN_ERROR_ILLPARAMTYPE: invalid parameter
        InvalidParameterValue  = -265, ///< PCAN_ERROR_ILLPARAMVAL: invalid parameter value
        InvalidData            = -267, ///< PCAN_ERROR_ILLDATA: invalid data, function, or action
        InvalidOperation       = -268, ///< PCAN_ERROR_ILLOPERATION: invalid operation
        Caution                = -289, ///< PCAN_ERROR_CAUTION: an operation was successfully carried out, however, irregularities were registered
        UnkownError            = -299  ///< PCAN_ERROR_UNKNOWN: unknown error
    };
    // CCanApi overrides
    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param, EChannelState &state);
    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, EChannelState &state);

    CANAPI_Return_t InitializeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param = NULL);
    CANAPI_Return_t TeardownChannel();
    CANAPI_Return_t SignalChannel();

    CANAPI_Return_t StartController(CANAPI_Bitrate_t bitrate);
    CANAPI_Return_t ResetController();

    CANAPI_Return_t WriteMessage(CANAPI_Message_t message, uint16_t timeout = 0U);
    CANAPI_Return_t ReadMessage(CANAPI_Message_t &message, uint16_t timeout = CANREAD_INFINITE);

    CANAPI_Return_t GetStatus(CANAPI_Status_t &status);
    CANAPI_Return_t GetBusLoad(uint8_t &load);

    CANAPI_Return_t GetBitrate(CANAPI_Bitrate_t &bitrate);
    CANAPI_Return_t GetBusSpeed(CANAPI_BusSpeed_t &speed);

    CANAPI_Return_t GetProperty(uint16_t param, void *value, uint32_t nbyte);
    CANAPI_Return_t SetProperty(uint16_t param, const void *value, uint32_t nbyte);

    char *GetHardwareVersion();  // (for compatibility reasons)
    char *GetFirmwareVersion();  // (for compatibility reasons)
    static char *GetVersion();  // (for compatibility reasons)

    static CANAPI_Return_t MapIndex2Bitrate(int32_t index, CANAPI_Bitrate_t &bitrate);
    static CANAPI_Return_t MapString2Bitrate(const char *string, CANAPI_Bitrate_t &bitrate);
    static CANAPI_Return_t MapBitrate2String(CANAPI_Bitrate_t bitrate, char *string, size_t length);
    static CANAPI_Return_t MapBitrate2Speed(CANAPI_Bitrate_t bitrate, CANAPI_BusSpeed_t &speed);
private:
    CANAPI_Return_t MapBitrate2Sja1000(CANAPI_Bitrate_t bitrate, uint16_t &btr0btr1);
    CANAPI_Return_t MapSja10002Bitrate(uint16_t btr0btr1, CANAPI_Bitrate_t &bitrate);
public:
    static uint8_t Dlc2Len(uint8_t dlc) { return CCanApi::Dlc2Len(dlc); }
    static uint8_t Len2Dlc(uint8_t len) { return CCanApi::Len2Dlc(len); }
};
/// \}

/// \name   PeakCAN Property IDs
/// \brief  Properties that can be read (or written)
/// \{
#define PEAKCAN_PROPERTY_CANAPI              (CANPROP_GET_SPEC)
#define PEAKCAN_PROPERTY_VERSION             (CANPROP_GET_VERSION)
#define PEAKCAN_PROPERTY_PATCH_NO            (CANPROP_GET_PATCH_NO)
#define PEAKCAN_PROPERTY_BUILD_NO            (CANPROP_GET_BUILD_NO)
#define PEAKCAN_PROPERTY_LIBRARY_ID          (CANPROP_GET_LIBRARY_ID)
#define PEAKCAN_PROPERTY_LIBRARY_NAME        (CANPROP_GET_LIBRARY_DLLNAME)
#define PEAKCAN_PROPERTY_LIBRARY_VENDOR      (CANPROP_GET_LIBRARY_VENDOR)
#define PEAKCAN_PROPERTY_DEVICE_CHANNEL      (CANPROP_GET_DEVICE_CHANNEL)
#define PEAKCAN_PROPERTY_DEVICE_NAME         (CANPROP_GET_DEVICE_NAME)
#define PEAKCAN_PROPERTY_DEVICE_VENDOR       (CANPROP_GET_DEVICE_VENDOR)
#define PEAKCAN_PROPERTY_DEVICE_DRIVER       (CANPROP_GET_DEVICE_DLLNAME)
#define PEAKCAN_PROPERTY_OP_CAPABILITY       (CANPROP_GET_OP_CAPABILITY)
#define PEAKCAN_PROPERTY_OP_MODE             (CANPROP_GET_OP_MODE)
#define PEAKCAN_PROPERTY_BITRATE             (CANPROP_GET_BITRATE)
#define PEAKCAN_PROPERTY_SPEED               (CANPROP_GET_SPEED)
#define PEAKCAN_PROPERTY_STATUS              (CANPROP_GET_STATUS)
#define PEAKCAN_PROPERTY_BUSLOAD             (CANPROP_GET_BUSLOAD)
#define PEAKCAN_PROPERTY_NUM_CHANNELS        (CANPROP_GET_NUM_CHANNELS)
#define PEAKCAN_PROPERTY_CAN_CHANNEL         (CANPROP_GET_CAN_CHANNEL)
#define PEAKCAN_PROPERTY_CAN_CLOCKS          (CANPROP_GET_CAN_CLOCKS)
#define PEAKCAN_PROPERTY_TX_COUNTER          (CANPROP_GET_TX_COUNTER)
#define PEAKCAN_PROPERTY_RX_COUNTER          (CANPROP_GET_RX_COUNTER)
#define PEAKCAN_PROPERTY_ERR_COUNTER         (CANPROP_GET_ERR_COUNTER)
//#define PEAKCAN_PROPERTY_RCV_QUEUE_SIZE      (CANPROP_GET_RCV_QUEUE_SIZE)
//#define PEAKCAN_PROPERTY_RCV_QUEUE_HIGH      (CANPROP_GET_RCV_QUEUE_HIGH)
//#define PEAKCAN_PROPERTY_RCV_QUEUE_OVFL      (CANPROP_GET_RCV_QUEUE_OVFL)
#define PEAKCAN_PROPERTY_DEVICE_ID           (CANPROP_GET_VENDOR_PROP + PCAN_DEVICE_ID)
#define PEAKCAN_PROPERTY_API_VERSION         (CANPROP_GET_VENDOR_PROP + PCAN_API_VERSION)
#define PEAKCAN_PROPERTY_CHANNEL_VERSION     (CANPROP_GET_VENDOR_PROP + PCAN_CHANNEL_VERSION)
#define PEAKCAN_PROPERTY_HARDWARE_NAME       (CANPROP_GET_VENDOR_PROP + PCAN_HARDWARE_NAME)
//#define PEAKCAN_PROPERTY_SERIAL_NUMBER       (CANPROP_GET_VENDOR_PROP + PCAN_SERIAL_NUMBER)
//#define PEAKCAN_PROPERTY_CLOCK_DOMAIN        (CANPROP_GET_VENDOR_PROP + PCAN_CLOCK_DOMAIN)
/// \}
#endif // PEAKCAN_H_INCLUDED
