/*  -- $HeadURL$ --
 *
 *  project   :  CAN - Controller Area Network
 *
 *  purpose   :  CAN Interface API, Version 3 (PCAN-Basic)
 *
 *  copyright :  (C) 2010,2012 UV Software, Friedrichshafen
 *               (C) 2014,2017 UV Software, Berlin
 *
 *  compiler  :  Apple LLVM version 9.0.0 (clang-900.0.38)
 *
 *  export    :  (see below)
 *
 *  includes  :  (none)
 *
 *  author    :  Uwe Vogt, UV Software
 *
 *  e-mail    :  uwe.vogt@uv-software.de
 *
 *
 *  -----------  description  --------------------------------------------
 */
/** @file        can_defs.h
 *
 *  @brief       CAN API V3 for PEAK PCAN-Basic Interfaces - Defines
 *
 *               For PEAK PCAN-Basic Interfaces (libPCBUSB v0.8):
 *               - PCAN-USB Interface (channel 1 - 8)
 *               - PCAN-USB FD Interface (channel 1 - 8)
 *
 *  @author      $Author$
 *
 *  @version     $Rev$
 *
 *  @defgroup    can_defs Options and Definitions
 *  @{
 */

#ifndef __CAN_DEFS_H
#define __CAN_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */

#ifndef _CAN_DEFS                       // PEAK PCAN-Basic interfaces:
 #define PCAN_USB1              0x51U   //   PCAN-USB interface, channel 1
 #define PCAN_USB2              0x52U   //   PCAN-USB interface, channel 2
 #define PCAN_USB3              0x53U   //   PCAN-USB interface, channel 3
 #define PCAN_USB4              0x54U   //   PCAN-USB interface, channel 4
 #define PCAN_USB5              0x55U   //   PCAN-USB interface, channel 5
 #define PCAN_USB6              0x56U   //   PCAN-USB interface, channel 6
 #define PCAN_USB7              0x57U   //   PCAN-USB interface, channel 7
 #define PCAN_USB8              0x58U   //   PCAN-USB interface, channel 8
#ifndef __APPLE__
 #define PCAN_USB9              0x509U  //   PCAN-USB interface, channel 9
 #define PCAN_USB10             0x50AU  //   PCAN-USB interface, channel 10
 #define PCAN_USB11             0x50BU  //   PCAN-USB interface, channel 11
 #define PCAN_USB12             0x50CU  //   PCAN-USB interface, channel 12
 #define PCAN_USB13             0x50DU  //   PCAN-USB interface, channel 13
 #define PCAN_USB14             0x50EU  //   PCAN-USB interface, channel 14
 #define PCAN_USB15             0x50FU  //   PCAN-USB interface, channel 15
 #define PCAN_USB16             0x510U  //   PCAN-USB interface, channel 16
 #define PCAN_BOARDS               (16) //   number of PCAN Interface boards

 #define PCAN_MAX_HANDLES          (16) //   maximum number of interface handles
#else
 #define PCAN_BOARDS               (8)  //   number of PCAN Interface boards

 #define PCAN_MAX_HANDLES          (8)  //   maximum number of interface handles
#endif
 #define PCAN_ERR_REGTEST          -201 //   test of the CAN controller hardware registers failed (no hardware found)
 #define PCAN_ERR_NODRIVER         -202 //   driver not loaded
 #define PCAN_ERR_HWINUSE          -203 //   hardware is in use by another Net
 #define PCAN_ERR_NETINUSE         -204 //   a client is already connected to the Net
 #define PCAN_ERR_ILLHW            -220 //   hardware handle is wrong
 #define PCAN_ERR_ILLNET           -224 //   net handle is wrong
 #define PCAN_ERR_ILLCLIENT        -228 //   client handle is wrong
 #define PCAN_ERR_RESOURCE         -232 //   resource (FIFO, Client, timeout) cannot be created
 #define PCAN_ERR_ILLPARAMTYPE     -264 //   invalid parameter
 #define PCAN_ERR_ILLPARAMVAL      -265 //   invalid parameter value
 #define PCAN_ERR_ILLDATA          -267 //   invalid data, function, or action
 #define PCAN_ERR_ILLOPERATION     -268 //   invalid operation [Value was changed from 0x80000 to 0x8000000]
 #define PCAN_ERR_CAUTION          -289 //   an operation was successfully carried out, however, irregularities were registered
 #define PCAN_ERR_ILLBOARD         -290 //   unknown board type, or not supported

 #define PCAN_ERR_OFFSET           -200 //   offset for PCANBasic-specific errors
 #define PCAN_ERR_UNKNOWN          -299 //   unknown error

 #define PCAN_LIB_ID                200 //   library ID (CAN/COP API V1 compatible) 
 #define PCAN_LIB_BASIC            "libPCBUSB.dylib"

 struct _pcan_param                     //   installation parameter:
 {
    unsigned char  type;                //     operation mode (non-plug'n'play devices)
    unsigned long  port;                //     I/O port address (parallel device)
    unsigned short irq;                 //     interrupt number (parallel device)
 };
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __CAN_DEFS_H */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
