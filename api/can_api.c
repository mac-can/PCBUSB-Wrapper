/*  -- $HeadURL$ --
 *
 *  project   :  CAN - Controller Area Network
 *
 *  purpose   :  CAN Interface API, Version 3 (PCAN-USB)
 *
 *  copyright :  (C) 2010,2012 by UV Software, Friedrichshafen
 *               (C) 2013-2019 by UV Software, Berlin
 *
 *  compiler  :  Apple LLVM version 10.0.0 (clang-1000.11.45.5)
 *
 *  export    :  (see header file)
 *
 *  includes  :  can_api.h (can_defs.h), PCBUSB.h
 *
 *  author    :  Uwe Vogt, UV Software
 *
 *  e-mail    :  uwe.vogt@uv-software.de
 *
 *
 *  -----------  description  --------------------------------------------
 */
/** @file        can_api.h
 *
 *  @brief       CAN API V3 for PEAK PCAN-USB Interfaces - API
 *
 *  @author      $Author$
 *
 *  @version     $Rev$
 *
 *  @addtogroup  can_api
 *  @{
 */

#ifdef _MSC_VER
#define VERSION_MAJOR     3
#define VERSION_MINOR     3
#define VERSION_REVISION  0
#else
#define VERSION_MAJOR     0
#define VERSION_MINOR     2
#define VERSION_REVISION  0
#endif
#define VERSION_STRING    TOSTRING(VERSION_MAJOR)"." TOSTRING(VERSION_MINOR)"."TOSTRING(VERSION_REVISION)
#if defined(_WIN64)
#define PLATFORM    "x64"
#elif defined(_WIN32)
#define PLATFORM    "x86"
#elif defined(__linux__)
#define PLATFORM    "Linux"
#elif defined(__APPLE__)
#define PLATFORM    "macOS"
#elif defined(__MINGW32__)
#define PLATFORM    "MinGW"
#else
#error Unsupported architecture
#endif
#include "can_vers.h"
#ifdef _DEBUG
    static char _id[] = "CAN API V3 for PEAK PCAN-Basic Interfaces, Version "VERSION_STRING"-"TOSTRING(BUILD_NO)" ("PLATFORM") _DEBUG";
#else
    static char _id[] = "CAN API V3 for PEAK PCAN-Basic Interfaces, Version "VERSION_STRING"-"TOSTRING(BUILD_NO)" ("PLATFORM")";
#endif

/*  -----------  includes  -------------------------------------------------
 */

#include "can_api.h"                    // Interface prototypes

#include <stdio.h>                      // Standard I/O routines
#include <string.h>                     // String manipulation functions
#ifdef _WIN32
#include <windows.h>                    // Master include file for Windows
#include "PCANBasic.h"                  // PEAK PCAN-Basic device(s)
#else
#include <unistd.h>                     // Posix: standard symbolic constants and types
#include <sys/select.h>                 // Posix: select types
#include "PCBUSB.h"                     // PEAK PCAN-USB interface(s)
#endif

/*  -----------  defines  ------------------------------------------------
 */

#ifdef _CANAPI_SHARED_LIBRARY
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT
#endif

/*  -----------  defines  ------------------------------------------------
 */

#ifndef PCAN_MAX_HANDLES
#define PCAN_MAX_HANDLES        (16)    // maximum number of open handles
#endif
#define INVALID_HANDLE          (-1)
#define IS_HANDLE_VALID(hnd)    ((0 <= (hnd)) && ((hnd) < PCAN_MAX_HANDLES))
#ifndef DLC2LEN
#define DLC2LEN(x)              dlc_table[x & 0xF]
#endif

/*  -----------  types  --------------------------------------------------
 */

typedef union {
    BYTE byte;                          // byte access
    struct {                            // bit access:
        BYTE mon : 1;                   //   monitor mode enable/disable
        BYTE : 3;                       //   (reserved)
        BYTE niso : 1;                  //   Non-ISO CAN FD enable
        BYTE : 1;                       //   (reserved)
        BYTE brse : 1;                  //   bit-rate switch enable
        BYTE fdoe : 1;                  //   CAN FD operation enable
    }   b;
}   can_mode_t;

typedef struct {
    TPCANHandle board;                  // board hardware channel handle
    BYTE  brd_type;                     // board type (none PnP hardware)
    DWORD brd_port;                     // board parameter: I/O port address
    WORD  brd_irq;                      // board parameter: interrupt number
    int   initialized;                  // flag for deferred initialization (ICA)
    can_mode_t mode;                    // operation mode of the CAN channel
    can_status_t status;                // 8-bit status register
    can_bitrate_t bitrate;              // bit-rate setting
#ifndef _WIN32
    int   fdes;                         // file descriptor (for blocking read)
#else
#endif
}   can_interface_t;


/*  -----------  prototypes  ---------------------------------------------
 */

static int pcan_error(TPCANStatus);     // PCAN specific errors


/*  -----------  variables  ----------------------------------------------
 */

EXPORT
can_board_t can_board[PCAN_BOARDS] =    // list of CAN Interface boards:
{
    {PCAN_USB1,                           "PCAN-USB1"},
    {PCAN_USB2,                           "PCAN-USB2"},
    {PCAN_USB3,                           "PCAN-USB3"},
    {PCAN_USB4,                           "PCAN-USB4"},
    {PCAN_USB5,                           "PCAN-USB5"},
    {PCAN_USB6,                           "PCAN-USB6"},
    {PCAN_USB7,                           "PCAN-USB7"},
    {PCAN_USB8,                           "PCAN-USB8"},
#ifndef __APPLE__
    {PCAN_USB9,                           "PCAN-USB9"},
    {PCAN_USB10,                          "PCAN-USB10"},
    {PCAN_USB11,                          "PCAN-USB11"},
    {PCAN_USB12,                          "PCAN-USB12"},
    {PCAN_USB13,                          "PCAN-USB13"},
    {PCAN_USB14,                          "PCAN-USB14"},
    {PCAN_USB15,                          "PCAN-USB15"},
    {PCAN_USB16,                          "PCAN-USB16"}
#endif
};
static const BYTE dlc_table[16] = {     // DLC to length
    0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64
};
static can_interface_t can[PCAN_MAX_HANDLES];   // interface handles
static char hardware[256] = "";         // hardware version of the CAN interface board
static char software[256] = "";         // software version of the CAN interface driver
static int  init = 0;                   // initialization flag


/*  -----------  functions  ----------------------------------------------
 */

#ifdef _CANAPI_SHARED_LIBRARY
__attribute__((constructor))
static void _initializer()
{
    return;
}

__attribute__((destructor))
static void _finalizer()
{
    return;
}
#endif

EXPORT
int can_test(int board, unsigned char mode, const void *param, int *result)
{
    TPCANStatus rc;                     // return value
    DWORD condition;                    // channel condition
    DWORD features;                     // channel features
    int used = 0;                       // own used channel
    int i;

    if(!init) {                         // when not init before:
        for(i = 0; i < PCAN_MAX_HANDLES; i++) {
            can[i].board = PCAN_NONEBUS;
            can[i].brd_type = 0;
            can[i].brd_port = 0;
            can[i].brd_irq = 0;
            can[i].initialized = 0;
            can[i].mode.byte = CANMODE_DEFAULT;
            can[i].status.byte = CANSTAT_RESET;
            can[i].bitrate.index = -CANBDR_250;
        }
        init = 1;                       //   set initialization flag
    }
    if((rc = CAN_GetValue((WORD)board, PCAN_CHANNEL_CONDITION,
                          (void*)&condition, sizeof(condition))) != PCAN_ERROR_OK)
        return pcan_error(rc);
    for(i = 0; i < PCAN_MAX_HANDLES; i++) {
        if(can[i].board == board) {     //   me, myself and I!
            condition = PCAN_CHANNEL_OCCUPIED;
            used = 1;
            break;
        }
    }
    if(result) {                        // CAN board test:
        if((condition == PCAN_CHANNEL_AVAILABLE) || (condition == PCAN_CHANNEL_PCANVIEW))
            *result = CANBRD_PRESENT;     // CAN board present
        else if(condition == PCAN_CHANNEL_UNAVAILABLE)
            *result = CANBRD_NOT_PRESENT; // CAN board not present
        else if(condition == PCAN_CHANNEL_OCCUPIED)
            *result = CANBRD_OCCUPIED;    // CAN board present, but occupied
        else
            *result = CANBRD_NOT_TESTABLE;// guess borad is not testable
    }
    if(((condition == PCAN_CHANNEL_AVAILABLE) || (condition == PCAN_CHANNEL_PCANVIEW)) ||
       (/*(condition == PCAN_CHANNEL_OCCUPIED) ||*/ used)) {
        // FIXME: issue TC07_47_9w - returns PCAN_ERROR_INITIALIZE when channel used by another process
        if((rc = CAN_GetValue((WORD)board, PCAN_CHANNEL_FEATURES,
                              (void*)&features, sizeof(features))) != PCAN_ERROR_OK)
            return pcan_error(rc);
        if((mode & CANMODE_FDOE) && !(features & FEATURE_FD_CAPABLE))
            return CANERR_ILLPARA; // CAN FD operation requested, but not supported
        if((mode & CANMODE_BRSE) && !(mode & CANMODE_FDOE))
            return CANERR_ILLPARA; // bit-rate switching requested, but CAN FD not enabled
        // TODO: mode NISO?
        // TODO: mode MON?
    }
    (void)param;
    return CANERR_NOERROR;
}

EXPORT
int can_init(int board, unsigned char mode, const void *param)
{
    TPCANStatus rc;                     // return value
    DWORD cond;                         // channel condition
    int i;

    if(!init) {                         // when not init before:
        for(i = 0; i < PCAN_MAX_HANDLES; i++) {
            can[i].board = PCAN_NONEBUS;
            can[i].brd_type = 0;
            can[i].brd_port = 0;
            can[i].brd_irq = 0;
            can[i].initialized = 0;
            can[i].mode.byte = CANMODE_DEFAULT;
            can[i].status.byte = CANSTAT_RESET;
            can[i].bitrate.index = -CANBDR_250;
        }
        init = 1;                       //   set initialization flag
    }
	for(i = 0; i < PCAN_MAX_HANDLES; i++) {
		if(can[i].board == board)       // channel already in use
			return CANERR_YETINIT;
	}
    for(i = 0; i < PCAN_MAX_HANDLES; i++) {
        if(can[i].board == PCAN_NONEBUS)// get an unused handle, if any
            break;
    }
    if(!IS_HANDLE_VALID(i))             // no free handle found
        return CANERR_HANDLE;

    if((rc = CAN_GetValue((WORD)board, PCAN_CHANNEL_CONDITION, (void*)&cond, sizeof(cond))) != PCAN_ERROR_OK)
        return pcan_error(rc);
    if(cond == PCAN_CHANNEL_UNAVAILABLE)
        return PCAN_ERR_ILLBOARD;
    else if(cond == PCAN_CHANNEL_OCCUPIED)
        return CANERR_YETINIT;
    else if((cond != PCAN_CHANNEL_AVAILABLE) && (cond != PCAN_CHANNEL_PCANVIEW))
        return PCAN_ERR_UNKNOWN;

    can[i].board = board;               // handle of the CAN channel
    if(param) {                         // non-plug'n'play devices:
        can[i].brd_type = ((struct _pcan_param*)param)->type;
        can[i].brd_port = ((struct _pcan_param*)param)->port;
        can[i].brd_irq  = ((struct _pcan_param*)param)->irq;
    }
#ifndef _WIN32
    can[i].fdes = -1;                   // clear file descriptor
#endif
    can[i].initialized = 0;             // clear re-initialization flag
    can[i].mode.byte = mode;            // store selected operation mode
    can[i].status.byte = CANSTAT_RESET; // CAN controller not started yet

    return i;                           // return the handle
}

EXPORT
int can_exit(int handle)
{
    if(!init)                           // must be initialized
        return CANERR_NOTINIT;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return CANERR_HANDLE;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return CANERR_HANDLE;

    /*if(!can[handle].status.b.can_stopped) // release the CAN interface: */
    {
        CAN_Uninitialize(can[handle].board);
#ifndef _WIN32
        close(can[handle].fdes);
#endif
    }
    can[handle].status.byte |= CANSTAT_RESET;// CAN controller in INIT state
    can[handle].board = PCAN_NONEBUS;   // handle can be used again

    return CANERR_NOERROR;
}

EXPORT
int can_start(int handle, const can_bitrate_t *bitrate)
{
    TPCANBaudrate baudrate;             // btr0btr1 value
    char string[256];                   // bit-rate string
    DWORD value;                        // parameter value
    TPCANStatus rc;                     // return value

    if(!init)                           // must be initialized
        return CANERR_NOTINIT;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return CANERR_HANDLE;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return CANERR_HANDLE;
    if(bitrate == NULL)                 // check for null-pointer
        return CANERR_NULLPTR;
    if(!can[handle].status.b.can_stopped)// must be stopped!
        return CANERR_ONLINE;

    if(bitrate->index <= 0) {           // btr0btr1 from index
        switch(bitrate->index * (-1)) {
        case CANBDR_1000: baudrate = PCAN_BAUD_1M; break;
        case CANBDR_800: baudrate = PCAN_BAUD_800K; break;
        case CANBDR_500: baudrate = PCAN_BAUD_500K; break;
        case CANBDR_250: baudrate = PCAN_BAUD_250K; break;
        case CANBDR_125: baudrate = PCAN_BAUD_125K; break;
        case CANBDR_100: baudrate = PCAN_BAUD_100K; break;
        case CANBDR_50: baudrate = PCAN_BAUD_50K; break;
        case CANBDR_20: baudrate = PCAN_BAUD_20K; break;
        case CANBDR_10: baudrate = PCAN_BAUD_10K; break;
        default: return CANERR_BAUDRATE;
        }
    }
    else if(!can[handle].mode.b.fdoe) { // btr0btr1 for CAN 2.0
        if(bitrate->btr.frequency != 8000000) return CANERR_BAUDRATE;   // SJA1000 @ 8MHz
        if((bitrate->btr.nominal.brp < 1) || (64 < bitrate->btr.nominal.brp)) return CANERR_BAUDRATE;
        if((bitrate->btr.nominal.tseg1 < 1) || (16 < bitrate->btr.nominal.tseg1)) return CANERR_BAUDRATE;
        if((bitrate->btr.nominal.tseg2 < 1) || (8 < bitrate->btr.nominal.tseg2)) return CANERR_BAUDRATE;
        if((bitrate->btr.nominal.sjw < 1) || (4 < bitrate->btr.nominal.sjw)) return CANERR_BAUDRATE;
        if((bitrate->btr.nominal.sam > 1)) return CANERR_BAUDRATE;
        /* +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+ */
        /* |  SJW  |          BRP          |SAM|   TSEG2   |     TSEG1     | */
        /* +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+ */
        baudrate = ((((TPCANBaudrate)(bitrate->btr.nominal.sjw - 1) & 0x0003) << 14)  | \
                    (((TPCANBaudrate)(bitrate->btr.nominal.brp - 1) & 0x003F) << 8)   | \
                    (((TPCANBaudrate)(bitrate->btr.nominal.sam - 1) & 0x0001) << 7)   | \
                    (((TPCANBaudrate)(bitrate->btr.nominal.tseg2 - 1) & 0x0007) << 4) | \
                    (((TPCANBaudrate)(bitrate->btr.nominal.tseg1 - 1) & 0x000F) << 0));
    }
    else {                              // a string for CAN FD
        if(can[handle].mode.b.brse)     //   long frames and fast frames
            snprintf(string, 256, "f_clock=%li,nom_brp=%u,nom_tseg1=%u,nom_tseg2=%u,nom_sjw=%u,nom_sam=%u," \
                                  "data_brp=%u,data_tseg1=%u,data_tseg2=%u,data_sjw=%u",
                     bitrate->btr.frequency,
                     bitrate->btr.nominal.brp,
                     bitrate->btr.nominal.tseg1,
                     bitrate->btr.nominal.tseg2,
                     bitrate->btr.nominal.sjw,
                     bitrate->btr.nominal.sam,
                     bitrate->btr.data.brp,
                     bitrate->btr.data.tseg1,
                     bitrate->btr.data.tseg2,
                     bitrate->btr.data.sjw);
        else                            //   long frames or CAN 2.0
            snprintf(string, 256, "f_clock=%li,nom_brp=%u,nom_tseg1=%u,nom_tseg2=%u,nom_sjw=%u,nom_sam=%u",
                     bitrate->btr.frequency,
                     bitrate->btr.nominal.brp,
                     bitrate->btr.nominal.tseg1,
                     bitrate->btr.nominal.tseg2,
                     bitrate->btr.nominal.sjw,
                     bitrate->btr.nominal.sam);
    }
    if(can[handle].initialized) {       // re-initialize CAN controller
        if((rc = CAN_Reset(can[handle].board)) !=  PCAN_ERROR_OK)
            return pcan_error(rc);
        if((rc = CAN_Uninitialize(can[handle].board)) !=  PCAN_ERROR_OK)
            return pcan_error(rc);
        can[handle].initialized = 0;
    }
    if(can[handle].mode.b.mon) {        // set listen-only mode?
        value = PCAN_PARAMETER_ON;
        if((rc = CAN_SetValue(can[handle].board, PCAN_LISTEN_ONLY,
                              (void*)&value, sizeof(value))) != PCAN_ERROR_OK)
            return pcan_error(rc);
    }
    if(can[handle].mode.b.fdoe) {       // CAN FD operation mode?
        if((rc = CAN_InitializeFD(can[handle].board, string)) != PCAN_ERROR_OK)
            return pcan_error(rc);
    }
    else {                              // CAN 2.0 operation mode!
        if((rc = CAN_Initialize(can[handle].board, baudrate,
                                can[handle].brd_type, can[handle].brd_port,
                                can[handle].brd_irq)) != PCAN_ERROR_OK)
            return pcan_error(rc);
    }
#ifndef _WIN32
    if((rc = CAN_GetValue(can[handle].board, PCAN_RECEIVE_EVENT,
                         &can[handle].fdes, sizeof(int))) != PCAN_ERROR_OK) {
        CAN_Uninitialize(can[handle].board);
        return pcan_error(rc);
    }
#endif
    memcpy(&can[handle].bitrate, bitrate, sizeof(can_bitrate_t));
    can[handle].status.byte = 0x00;     // clear old status bits
    can[handle].status.b.can_stopped = 0;// CAN controller started
    can[handle].initialized = 1;

    return CANERR_NOERROR;
}

EXPORT
int can_reset(int handle)
{
    if(!init)                           // must be initialized
        return CANERR_NOTINIT;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return CANERR_HANDLE;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return CANERR_HANDLE;

    if(!can[handle].status.b.can_stopped) { // CAN started, then reset
        // TODO: implement a permanent corrective action (PCA)
    }
    can[handle].status.b.can_stopped = 1;   // CAN controller stopped

    return CANERR_NOERROR;
}

EXPORT
int can_write(int handle, const can_msg_t *msg)
{
    TPCANMsg can_msg;                   // the message (CAN 2.0)
    TPCANMsgFD can_msg_fd;              // the message (CAN FD)
    TPCANStatus rc;                     // return value

    if(!init)                           // must be initialized
        return CANERR_NOTINIT;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return CANERR_HANDLE;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return CANERR_HANDLE;
    if(msg == NULL)                     // check for null-pointer
        return CANERR_NULLPTR;
    if(can[handle].status.b.can_stopped)// must be running
        return CANERR_OFFLINE;

    if(!can[handle].mode.b.fdoe) {
        if(msg->dlc > CAN_MAX_LEN)      //   data length 0 .. 8
            return CANERR_ILLPARA;
        if(msg->ext)                    //   29-bit identifier
            can_msg.MSGTYPE = PCAN_MESSAGE_EXTENDED;
        else                            //   11-bit identifier
            can_msg.MSGTYPE = PCAN_MESSAGE_STANDARD;
        if(msg->rtr)                    //   request a message
            can_msg.MSGTYPE |= PCAN_MESSAGE_RTR;
        can_msg.ID = (DWORD)(msg->id);
        can_msg.LEN = (BYTE)(msg->dlc);
        memcpy(can_msg.DATA, msg->data, msg->dlc);

        rc = CAN_Write(can[handle].board, &can_msg);
    }
    else {
        if(msg->dlc > CANFD_MAX_LEN)    //   data length 0 .. 64!
            return CANERR_ILLPARA;
        if(msg->ext)                    //   29-bit identifier
            can_msg_fd.MSGTYPE = PCAN_MESSAGE_EXTENDED;
        else                            //   11-bit identifier
            can_msg_fd.MSGTYPE = PCAN_MESSAGE_STANDARD;
        if(msg->rtr)                    //   request a message
            can_msg_fd.MSGTYPE |= PCAN_MESSAGE_RTR;
        if(msg->fdf)                    //   CAN FD format
            can_msg_fd.MSGTYPE |= PCAN_MESSAGE_FD;
        if(msg->brs && can[handle].mode.b.brse) //   bit-rate switching
            can_msg_fd.MSGTYPE |= PCAN_MESSAGE_BRS;
        can_msg_fd.ID = (DWORD)(msg->id);
        can_msg_fd.DLC = (BYTE)(msg->dlc);
        memcpy(can_msg_fd.DATA, msg->data, DLC2LEN(msg->dlc));

        rc = CAN_WriteFD(can[handle].board, &can_msg_fd);
    }
    if(rc != PCAN_ERROR_OK) {
        if((rc & PCAN_ERROR_QXMTFULL)) {//   transmit queue full?
            can[handle].status.b.transmitter_busy = 1;
            return CANERR_TX_BUSY;      //     transmitter busy
        }
        else if((rc & PCAN_ERROR_XMTFULL)){//transmission pending?
            can[handle].status.b.transmitter_busy = 1;
            return CANERR_TX_BUSY;      //     transmitter busy
        }
        return pcan_error(rc);          //   PCAN specific error?
    }
    can[handle].status.b.transmitter_busy = 0;  // message transmitted

    return CANERR_NOERROR;
}

EXPORT
int can_read(int handle, can_msg_t *msg, unsigned short timeout)
{
    TPCANMsg can_msg;                   // the message (CAN 2.0)
    TPCANTimestamp timestamp;           // time stamp (CAN 2.0)
    TPCANMsgFD can_msg_fd;              // the message (CAN FD)
    TPCANTimestampFD timestamp_fd;      // time stamp (CAN FD)
    unsigned long long msec;            // milliseconds
    TPCANStatus rc;                     // return value

    if(!init)                           // must be initialized
        return CANERR_NOTINIT;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return CANERR_HANDLE;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return CANERR_HANDLE;
    if(msg == NULL)                     // check for null-pointer
        return CANERR_NULLPTR;
    if(can[handle].status.b.can_stopped)// must be running
        return CANERR_OFFLINE;

    // blocking read
#ifndef _WIN32
    fd_set rdfs;
    FD_ZERO(&rdfs);
    FD_SET(can[handle].fdes, &rdfs);
    struct timeval tv;
    tv.tv_sec = (time_t)(timeout / 1000u);
    tv.tv_usec = (suseconds_t)(timeout % 1000u) * (suseconds_t)1000;
retry:
#endif
    if(!can[handle].mode.b.fdoe)
        rc = CAN_Read(can[handle].board, &can_msg, &timestamp);
    else
        rc = CAN_ReadFD(can[handle].board, &can_msg_fd, &timestamp_fd);
    if(rc == PCAN_ERROR_QRCVEMPTY) {
#ifndef _WIN32
        if(timeout == 65535u) {
            if(select(can[handle].fdes+1, &rdfs, NULL, NULL, NULL) > 0)
                goto retry;
        }
        else if(timeout != 0) {
            if(select(can[handle].fdes+1, &rdfs, NULL, NULL, &tv) > 0)
                goto retry;
        }
#endif
        can[handle].status.b.receiver_empty = 1;
        return CANERR_RX_EMPTY;         //   receiver empty!
    }
    if(rc != PCAN_ERROR_OK) {
        return pcan_error(rc);          //   something's wrong
    }
    if((can_msg.MSGTYPE == PCAN_MESSAGE_STATUS) ||
       (can_msg_fd.MSGTYPE == PCAN_MESSAGE_STATUS)) {
        can[handle].status.b.bus_off = (can_msg.DATA[3] & PCAN_ERROR_BUSOFF) != PCAN_ERROR_OK;
        can[handle].status.b.bus_error = (can_msg.DATA[3] & PCAN_ERROR_BUSLIGHT) != PCAN_ERROR_OK;
        can[handle].status.b.warning_level = (can_msg.DATA[3] & PCAN_ERROR_BUSHEAVY) != PCAN_ERROR_OK;
        can[handle].status.b.message_lost |= (can_msg.DATA[3] & PCAN_ERROR_OVERRUN) != PCAN_ERROR_OK;
        return CANERR_RX_EMPTY;         //   receiver empty
    }
    if(!can[handle].mode.b.fdoe) {      // CAN 2.0 message:
        msg->id = can_msg.ID;
        msg->ext = (can_msg.MSGTYPE & PCAN_MESSAGE_EXTENDED) ? 1 : 0;
        msg->rtr = (can_msg.MSGTYPE & PCAN_MESSAGE_RTR) ? 1 : 0;
        msg->fdf = 0;
        msg->brs = 0;
        msg->esi = 0;
        msg->dlc = can_msg.LEN;
        memcpy(msg->data, can_msg.DATA, CAN_MAX_LEN);
        msec = ((unsigned long long)timestamp.millis_overflow << 32) + (unsigned long long)timestamp.millis;
        msg->timestamp.sec = (long)(msec / 1000ull);
        msg->timestamp.usec = (((long)(msec % 1000ull)) * 1000L) + (long)timestamp.micros;
    }
    else {                              // CAN FD message:
        msg->id = can_msg_fd.ID;
        msg->ext = (can_msg_fd.MSGTYPE & PCAN_MESSAGE_EXTENDED) ? 1 : 0;
        msg->rtr = (can_msg_fd.MSGTYPE & PCAN_MESSAGE_RTR) ? 1 : 0;
        msg->fdf = (can_msg_fd.MSGTYPE & PCAN_MESSAGE_FD) ? 1 : 0;
        msg->brs = (can_msg_fd.MSGTYPE & PCAN_MESSAGE_BRS) ? 1 : 0;
        msg->esi = (can_msg_fd.MSGTYPE & PCAN_MESSAGE_ESI) ? 1 : 0;
        msg->dlc = can_msg_fd.DLC;
        memcpy(msg->data, can_msg_fd.DATA, CANFD_MAX_LEN);
        msg->timestamp.sec = (long)(timestamp_fd / 1000000ull);
        msg->timestamp.usec = (long)(timestamp_fd % 1000000ull);
    }
    can[handle].status.b.receiver_empty = 0;        // message read

    return CANERR_NOERROR;
}

EXPORT
int can_status(int handle, unsigned char *status)
{
    TPCANStatus rc;                     // return value

    if(!init)                           // must be initialized
        return CANERR_NOTINIT;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return CANERR_HANDLE;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return CANERR_HANDLE;

    if(!can[handle].status.b.can_stopped)   {   // must be running:
        if((rc = CAN_GetStatus(can[handle].board)) > 255) // FIXME: mask it out!
            return pcan_error(rc);
        can[handle].status.b.bus_off = (rc & PCAN_ERROR_BUSOFF) != PCAN_ERROR_OK;
        can[handle].status.b.bus_error = (rc & (PCAN_ERROR_BUSLIGHT | PCAN_ERROR_BUSPASSIVE)) != PCAN_ERROR_OK;
        can[handle].status.b.warning_level = (rc & (PCAN_ERROR_BUSHEAVY | PCAN_ERROR_BUSWARNING)) != PCAN_ERROR_OK;
        can[handle].status.b.message_lost |= (rc & (PCAN_ERROR_OVERRUN | PCAN_ERROR_QOVERRUN)) != PCAN_ERROR_OK;
        can[handle].status.b.transmitter_busy |= (rc & (PCAN_ERROR_XMTFULL | PCAN_ERROR_QXMTFULL)) != PCAN_ERROR_OK;
    }
    if(status)                          // status-register
      *status = can[handle].status.byte;

    return CANERR_NOERROR;
}

EXPORT
int can_busload(int handle, unsigned char *load, unsigned char *status)
{
    if(!init)                           // must be initialized
        return CANERR_NOTINIT;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return CANERR_HANDLE;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return CANERR_HANDLE;

    if(!can[handle].status.b.can_stopped) { // must be running:
        if(load)
            *load = 0;                  //   TODO: bus-load [percent]
    }
    return can_status(handle, status);  // status-register
}

EXPORT
int can_bitrate(int handle, can_bitrate_t *bitrate, can_speed_t *speed)
{
    if(!init)                           // must be initialized
        return CANERR_NOTINIT;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return CANERR_HANDLE;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return CANERR_HANDLE;

    if(!can[handle].status.b.can_stopped) { // must be running:
        if(bitrate)
            memcpy(bitrate, &can[handle].bitrate, sizeof(can_bitrate_t));
        if (speed)
            memset(speed, 0, sizeof(can_speed_t)); // TODO: calculate this!
    }
    return can_status(handle, NULL);    // current status
}

EXPORT
int can_interface(int handle, int *board, unsigned char *mode, void *param)
{

    if(!init)                           // must be initialized
        return CANERR_NOTINIT;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return CANERR_HANDLE;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return CANERR_HANDLE;

    if(board)                           // handle of the CAN channel
        *board = can[handle].board;
    if(mode)                            // current opperation mode
        *mode  = can[handle].mode.byte;
    if(param) {                         // non-plug'n'play devices:
        ((struct _pcan_param*)param)->type = can[handle].brd_type;
        ((struct _pcan_param*)param)->port = can[handle].brd_port;
        ((struct _pcan_param*)param)->irq  = can[handle].brd_irq;
    }
    return CANERR_NOERROR;
}

EXPORT
char *can_hardware(int handle)
{
    char  str[256], *ptr;               // info string
    DWORD dev = 0x0000UL;               // device number

    if(!init)                           // must be initialized
        return NULL;
    if(!IS_HANDLE_VALID(handle))        // must be a valid handle
        return NULL;
    if(can[handle].board == PCAN_NONEBUS) // must be an opened handle
        return NULL;

#ifndef PCAN_EXT_HARDWARE_VERSION
    if(CAN_GetValue(can[handle].board, PCAN_CHANNEL_VERSION, (void*)str, 256) != PCAN_ERROR_OK)
        return NULL;
    if((ptr = strchr(str, '\n')) != NULL)
       *ptr = '\0';
    if((((can[handle].board & 0x00F0) >> 4) == PCAN_USB) ||
       (((can[handle].board & 0x0F00) >> 8) == PCAN_USB))
    {
        if(CAN_GetValue(can[handle].board, PCAN_DEVICE_NUMBER, (void*)&dev, 4) != PCAN_ERROR_OK)
            return NULL;
        snprintf(hardware, 256, "%s (Device %02lXh)", str, dev);
    }
    else
        strncpy(hardware, str, 256);
#else
    if(CAN_GetValue(can[handle].board, PCAN_EXT_HARDWARE_VERSION, (void*)str, 256) != PCAN_ERROR_OK)
        return NULL;
    (void)dev;
    (void)ptr;
    strncpy(hardware, str, 256);
#endif
    return (char*)hardware;     // hardware version
}

EXPORT
char *can_software(int handle)
{
    char  str[256] = "PCAN-Basic API "; // info string

    if(!init)                           // must be initialized
        return NULL;
    (void)handle;                       // handle not needed here

#ifndef PCAN_EXT_SOFTWARE_VERSION
    if(CAN_GetValue(PCAN_NONEBUS, PCAN_API_VERSION, (void*)&str[15], 256-15) != PCAN_ERROR_OK)
        return NULL;
#else
    if(CAN_GetValue(PCAN_NONEBUS, PCAN_EXT_SOFTWARE_VERSION, (void*)str, 256) != PCAN_ERROR_OK)
        return NULL;
#endif
    strncpy(software, str, 256);

    return (char*)software;             // software version
}

EXPORT
int can_library(unsigned short *version, unsigned char *revision, unsigned long *build)
{
    if(version)
        *version = ((unsigned short)VERSION_MAJOR << 8) | ((unsigned short)VERSION_MINOR & 0x00FFu);
    if(revision)
        *revision = (unsigned char)VERSION_REVISION;
    if(build)
        *build = (unsigned long)BUILD_NO;

    return PCAN_LIB_ID;                 // library ID
}

/*  -----------  local functions  ----------------------------------------
 */

static int pcan_error(TPCANStatus status)
{
    if((status & PCAN_ERROR_XMTFULL)      == PCAN_ERROR_XMTFULL)       return CANERR_TX_BUSY;
    if((status & PCAN_ERROR_OVERRUN)      == PCAN_ERROR_OVERRUN)       return CANERR_MSG_LST;
    if((status & PCAN_ERROR_BUSOFF)       == PCAN_ERROR_BUSOFF)        return CANERR_BOFF;
    if((status & PCAN_ERROR_BUSPASSIVE)   == PCAN_ERROR_BUSPASSIVE)    return CANERR_EWRN;
    if((status & PCAN_ERROR_BUSHEAVY)     == PCAN_ERROR_BUSHEAVY)      return CANERR_BERR;
    if((status & PCAN_ERROR_BUSLIGHT)     == PCAN_ERROR_BUSLIGHT)      return CANERR_BERR;
    if((status & PCAN_ERROR_QRCVEMPTY)    == PCAN_ERROR_QRCVEMPTY)     return CANERR_RX_EMPTY;
    if((status & PCAN_ERROR_QOVERRUN)     == PCAN_ERROR_QOVERRUN)      return CANERR_MSG_LST;
    if((status & PCAN_ERROR_QXMTFULL)     == PCAN_ERROR_QXMTFULL)      return CANERR_TX_BUSY;
    if((status & PCAN_ERROR_REGTEST)      == PCAN_ERROR_REGTEST)       return PCAN_ERR_REGTEST;
    if((status & PCAN_ERROR_NODRIVER)     == PCAN_ERROR_NODRIVER)      return PCAN_ERR_NODRIVER;
    if((status & PCAN_ERROR_HWINUSE)      == PCAN_ERROR_HWINUSE)       return PCAN_ERR_HWINUSE;
    if((status & PCAN_ERROR_NETINUSE)     == PCAN_ERROR_NETINUSE)      return PCAN_ERR_NETINUSE;
    if((status & PCAN_ERROR_ILLHW)        == PCAN_ERROR_ILLHW)         return PCAN_ERR_ILLHW;
    if((status & PCAN_ERROR_ILLNET)       == PCAN_ERROR_ILLNET)        return PCAN_ERR_ILLNET;
    if((status & PCAN_ERROR_ILLCLIENT)    == PCAN_ERROR_ILLCLIENT)     return PCAN_ERR_ILLCLIENT;
    if((status & PCAN_ERROR_RESOURCE)     == PCAN_ERROR_RESOURCE)      return PCAN_ERR_RESOURCE;
    if((status & PCAN_ERROR_ILLPARAMTYPE) == PCAN_ERROR_ILLPARAMTYPE)  return PCAN_ERR_ILLPARAMTYPE;
    if((status & PCAN_ERROR_ILLPARAMVAL)  == PCAN_ERROR_ILLPARAMVAL)   return PCAN_ERR_ILLPARAMVAL;
    if((status & PCAN_ERROR_ILLDATA)      == PCAN_ERROR_ILLDATA)       return PCAN_ERR_ILLDATA;
    if((status & PCAN_ERROR_CAUTION)      == PCAN_ERROR_CAUTION)       return PCAN_ERR_CAUTION;
    if((status & PCAN_ERROR_INITIALIZE)   == PCAN_ERROR_INITIALIZE)    return CANERR_NOTINIT;
    if((status & PCAN_ERROR_ILLOPERATION) == PCAN_ERROR_ILLOPERATION)  return PCAN_ERR_ILLOPERATION;

    return PCAN_ERR_UNKNOWN;
}

/*  -----------  revision control  ---------------------------------------
 */

EXPORT
char* can_version()
{
    return (char*)_id;
}

/*  ===========  run-time loader  ========================================
 */

#ifdef _PCBUSB_SHARED_LIBRARY
#include <dlfcn.h>

typedef TPCANStatus (*CAN_Initialize_t)(TPCANHandle Channel, TPCANBaudrate Btr0Btr1, TPCANType HwType, DWORD IOPort, WORD Interrupt);
typedef TPCANStatus (*CAN_Uninitialize_t)(TPCANHandle Channel);
typedef TPCANStatus (*CAN_Reset_t)(TPCANHandle Channel);
typedef TPCANStatus (*CAN_GetStatus_t)(TPCANHandle Channel);
typedef TPCANStatus (*CAN_Read_t)(TPCANHandle Channel, TPCANMsg* MessageBuffer, TPCANTimestamp* TimestampBuffer);
typedef TPCANStatus (*CAN_Write_t)(TPCANHandle Channel, TPCANMsg* MessageBuffer);
typedef TPCANStatus (*CAN_FilterMessages_t)(TPCANHandle Channel, DWORD FromID, DWORD ToID, TPCANMode Mode);
typedef TPCANStatus (*CAN_GetValue_t)(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength);
typedef TPCANStatus (*CAN_SetValue_t)(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength);
typedef TPCANStatus (*CAN_GetErrorText_t)(TPCANStatus Error, WORD Language, char* Buffer);
typedef TPCANStatus (*CAN_InitializeFD_t)(TPCANHandle Channel, TPCANBitrateFD BitrateFD);
typedef TPCANStatus (*CAN_ReadFD_t)(TPCANHandle Channel, TPCANMsgFD* MessageBuffer, TPCANTimestampFD* TimestampBuffer);
typedef TPCANStatus (*CAN_WriteFD_t)(TPCANHandle Channel, TPCANMsgFD* MessageBuffer);

static CAN_Initialize_t _CAN_Initialize = NULL;
static CAN_Uninitialize_t _CAN_Uninitialize = NULL;
static CAN_Reset_t _CAN_Reset = NULL;
static CAN_GetStatus_t _CAN_GetStatus = NULL;
static CAN_Read_t _CAN_Read = NULL;
static CAN_Write_t _CAN_Write = NULL;
static CAN_FilterMessages_t _CAN_FilterMessages = NULL;
static CAN_GetValue_t _CAN_GetValue = NULL;
static CAN_SetValue_t _CAN_SetValue = NULL;
static CAN_GetErrorText_t _CAN_GetErrorText = NULL;
static CAN_InitializeFD_t _CAN_InitializeFD = NULL;
static CAN_ReadFD_t _CAN_ReadFD = NULL;
static CAN_WriteFD_t _CAN_WriteFD = NULL;

static void *hLibrary = NULL;

static int LoadLibrary(void)
{
    if(!hLibrary) {
        hLibrary = dlopen(PCAN_LIB_BASIC, RTLD_LAZY);
        if(!hLibrary)
            return -1;
        if((_CAN_Initialize = (CAN_Initialize_t)dlsym(hLibrary, "CAN_Initialize")) == NULL)
            goto err;
        if((_CAN_Uninitialize = (CAN_Uninitialize_t)dlsym(hLibrary, "CAN_Uninitialize")) == NULL)
            goto err;
        if((_CAN_Reset = (CAN_Reset_t)dlsym(hLibrary, "CAN_Reset")) == NULL)
            goto err;
        if((_CAN_GetStatus = (CAN_GetStatus_t)dlsym(hLibrary, "CAN_GetStatus")) == NULL)
            goto err;
        if((_CAN_Read = (CAN_Read_t)dlsym(hLibrary, "CAN_Read")) == NULL)
            goto err;
        if((_CAN_Write = (CAN_Write_t)dlsym(hLibrary, "CAN_Write")) == NULL)
            goto err;
        if((_CAN_FilterMessages = (CAN_FilterMessages_t)dlsym(hLibrary, "CAN_FilterMessages")) == NULL)
            goto err;
        if((_CAN_GetValue = (CAN_GetValue_t)dlsym(hLibrary, "CAN_GetValue")) == NULL)
            goto err;
        if((_CAN_SetValue = (CAN_SetValue_t)dlsym(hLibrary, "CAN_SetValue")) == NULL)
            goto err;
        if((_CAN_GetErrorText = (CAN_GetErrorText_t)dlsym(hLibrary, "CAN_GetErrorText")) == NULL)
            goto err;
        if((_CAN_InitializeFD = (CAN_InitializeFD_t)dlsym(hLibrary, "CAN_InitializeFD")) == NULL)
            goto err;
        if((_CAN_ReadFD = (CAN_ReadFD_t)dlsym(hLibrary, "CAN_ReadFD")) == NULL)
            goto err;
        if((_CAN_WriteFD = (CAN_WriteFD_t)dlsym(hLibrary, "CAN_WriteFD")) == NULL)
            goto err;
    }
    return 0;
err:
    _CAN_Initialize = NULL;
    _CAN_Uninitialize = NULL;
    _CAN_Reset = NULL;
    _CAN_GetStatus = NULL;
    _CAN_Read = NULL;
    _CAN_Write = NULL;
    _CAN_FilterMessages = NULL;
    _CAN_GetValue = NULL;
    _CAN_SetValue = NULL;
    _CAN_GetErrorText = NULL;
    _CAN_InitializeFD = NULL;
    _CAN_ReadFD = NULL;
    _CAN_WriteFD = NULL;
    dlclose(hLibrary);
    return -1;
}

TPCANStatus CAN_Initialize(TPCANHandle Channel, TPCANBaudrate Btr0Btr1, TPCANType HwType, DWORD IOPort, WORD Interrupt)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_Initialize)
        return _CAN_Initialize(Channel, Btr0Btr1, HwType, IOPort, Interrupt);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_Uninitialize(TPCANHandle Channel)
{
    //if(LoadLibrary() != 0)
    //    return PCAN_ERROR_NODRIVER;
    if(_CAN_Uninitialize)
        return _CAN_Uninitialize(Channel);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_Reset(TPCANHandle Channel)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_Reset)
        return _CAN_Reset(Channel);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_GetStatus(TPCANHandle Channel)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_GetStatus)
        return _CAN_GetStatus(Channel);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_Read(TPCANHandle Channel, TPCANMsg* MessageBuffer, TPCANTimestamp* TimestampBuffer)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_Read)
        return _CAN_Read(Channel, MessageBuffer, TimestampBuffer);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_Write(TPCANHandle Channel, TPCANMsg* MessageBuffer)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_Write)
        return _CAN_Write(Channel, MessageBuffer);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_FilterMessages(TPCANHandle Channel, DWORD FromID, DWORD ToID, TPCANMode Mode)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_FilterMessages)
        return _CAN_FilterMessages(Channel, FromID, ToID, Mode);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_GetValue(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_GetValue)
        return _CAN_GetValue(Channel, Parameter, Buffer, BufferLength);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_SetValue(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_SetValue)
        return _CAN_SetValue(Channel, Parameter, Buffer, BufferLength);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_GetErrorText(TPCANStatus Error, WORD Language, char* Buffer)
{
    if(LoadLibrary() != 0) {
        if(Buffer)
            strcpy(Buffer, "PCBUSB library could not be loaded");
        return PCAN_ERROR_NODRIVER;
    }
    if(_CAN_GetErrorText)
        return _CAN_GetErrorText(Error, Language, Buffer);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_InitializeFD(TPCANHandle Channel, TPCANBitrateFD BitrateFD)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_InitializeFD)
        return _CAN_InitializeFD(Channel, BitrateFD);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_ReadFD(TPCANHandle Channel, TPCANMsgFD* MessageBuffer, TPCANTimestampFD* TimestampBuffer)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_ReadFD)
        return _CAN_ReadFD(Channel, MessageBuffer, TimestampBuffer);
    else
        return PCAN_ERROR_UNKNOWN;
}

TPCANStatus CAN_WriteFD(TPCANHandle Channel, TPCANMsgFD* MessageBuffer)
{
    if(LoadLibrary() != 0)
        return PCAN_ERROR_NODRIVER;
    if(_CAN_WriteFD)
        return _CAN_WriteFD(Channel, MessageBuffer);
    else
        return PCAN_ERROR_UNKNOWN;
}
#endif //_PCBUSB_SHARED_LIBRARY
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
