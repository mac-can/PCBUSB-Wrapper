/*  -- $HeadURL$ --
 *
 *  project   :  CAN - Controller Area Network
 *
 *  purpose   :  CAN Interface API, Version 3 (PCAN-USB)
 *
 *  copyright :  (C) 2010,2012 by UV Software, Friedrichshafen
 *               (C) 2013-2020 by UV Software, Berlin
 *
 *  compiler  :  Apple clang version 11.0.0 (clang-1100.0.33.16)
 *
 *  export    :  int can_test(int32_t board, uint8_t mode, const void *param, int *result);
 *               int can_init(int32_t board, uint8_t mode, const void *param);
 *               int can_exit(int handle);
 *               int can_start(int handle, const can_bitrate_t *bitrate);
 *               int can_reset(int handle);
 *               int can_write(int handle, const can_msg_t *msg);
 *               int can_read(int handle, can_msg_t *msg, uint16_t timeout);
 *               int can_status(int handle, uint8_t *status);
 *               int can_busload(int handle, uint8_t *load, uint8_t *status);
 *               int can_bitrate(int handle, can_bitrate_t *bitrate, can_speed_t *speed);
 *               int can_property(int handle, uint16_t param, void *value, uint32_t nbytes);
 *               char *can_hardware(int handle);
 *               char *can_software(int handle);
 *               char *can_version();
 *
 *  includes  :  can_defs.h, CANAPI_Types.h (LGPL)
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
 *               For PEAK PCAN-USB Interfaces (libPCBUSB v0.9):
 *               - PCAN-USB Interface (channel 1 - 8)
 *               - PCAN-USB FD Interface (channel 1 - 8)
 *  @note        Up to 8 handles are supported by the API.
 *
 *  @author      $Author$
 *
 *  @version     $Rev$
 *
 *  @defgroup    can_api CAN Interface API, Version 3
 *  @{
 */
#ifndef CAN_API_H_INCLUDED
#define CAN_API_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*  -----------  includes  -----------------------------------------------
 */

#include "can_defs.h"                   /* CAN definitions and options */
#include "CANAPI_Types.h"               /* CAN API data types and defines */


/*  -----------  options  ------------------------------------------------
 */

#if (CAN_API_SPEC != 0x300)
    #error Requires version 3.0 of CANAPI_Types.h
#endif
#if (OPTION_CAN_2_0_ONLY != 0)
    #error Requires CAN FD message format
#endif
#if (OPTION_CANAPI_LIBRARY != 0)
    #error Requires function signatures as driver
#endif
#if (OPTION_CANAPI_DRIVER == 0)
    #define OPTION_CANAPI_DRIVER
#endif
#ifdef _CANAPI_EXPORTS
    #define CANAPI  __declspec(dllexport)
#else
  #ifndef _CANAPI_EXTERN
    #define CANAPI  __declspec(dllimport)
  #else
    #define CANAPI  extern
  #endif
#endif

/*  -----------  defines  ------------------------------------------------
 */

/** @name  Aliases
 *  @brief Alternative names
 *  @{ */
typedef int                             can_handle_t;
#define CANAPI_HANDLE                   (can_handle_t)(-1)
#define CANBRD_AVAILABLE                CANBRD_PRESENT
#define CANBRD_UNAVAILABLE              CANBRD_NOT_PRESENT
#define CANBRD_INTESTABLE               CANBRD_NOT_TESTABLE
#define CANEXIT_ALL                     CANKILL_ALL
#define CAN_MAX_EXT_ID                  CAN_MAX_XTD_ID
/** @} */

/** @name  Legacy Stuff
 *  @brief For compatibility reasons with CAN API V1 and V2
 *  @{ */
#define can_transmit(hnd, msg)          can_write(hnd, msg)
#define can_receive(hnd, msg)           can_read(hnd, msg, 0U)
#define can_msg_t                       can_message_t
/** @} */

/*  -----------  types  --------------------------------------------------
 */

/** @brief       CAN Interface Board:
 */
typedef struct can_board_t_ {
    int32_t type;                       /**< board type */
    char   *name;                       /**< board name */
} can_board_t;


/*  -----------  variables  ----------------------------------------------
 */

CANAPI can_board_t can_boards[];        /**< list of CAN interface boards */


/*  -----------  prototypes  ---------------------------------------------
 */

/** @brief       tests if the CAN interface (hardware and driver) given by
 *               the argument 'board' is present, and if the requested
 *               operation mode is supported by the CAN controller board.
 *
 *  @note        When a requested operation mode is not supported by the
 *               CAN controller, error CANERR_ILLPARA will be returned.
 *
 *  @param[in]   board   - type of the CAN controller board
 *  @param[in]   mode    - operation mode to be checked
 *  @param[in]   param   - pointer to board-specific parameters
 *  @param[out]  result  - result of the board test:
 *                             < 0 - board is not present,
 *                             = 0 - board is present,
 *                             > 0 - board is present, but in use
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_ILLPARA   - illegal parameter value
 *  @retval      CANERR_NOTSUPP   - function not supported
 *  @retval      others           - vendor-specific
 */
CANAPI int can_test(int32_t board, uint8_t mode, const void *param, int *result);


/** @brief       initializes the CAN interface (hardware and driver) by loading
 *               and starting the appropriate DLL for the specified CAN controller
 *               board given by the argument 'board'.
 *               The operation state of the CAN controller is set to 'stopped';
 *               no communication is possible in this state.
 *
 *  @param[in]   board   - type of the CAN controller board
 *  @param[in]   mode    - operation mode of the CAN controller
 *  @param[in]   param   - pointer to board-specific parameters
 *
 *  @returns     handle of the CAN interface if successful,
 *               or a negative value on error.
 *
 *  @retval      CANERR_YETINIT   - interface already in use
 *  @retval      CANERR_HANDLE    - no free handle found
 *  @retval      others           - vendor-specific
 */
CANAPI int can_init(int32_t board, uint8_t mode, const void *param);


/** @brief       stops any operation of the CAN interface and sets the operation
 *               state of the CAN controller to 'offline'.
 *
 *  @note        The handle is invalid after this operation and could be assigned
 *               to a different CAN controller board in a multy-board application.
 *
 *  @param[in]   handle  - handle of the CAN interface, or (-1) to shutdown all
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - interface not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      others           - vendor-specific
 */
CANAPI int can_exit(int handle);


/** @brief       initializes the operation mode and the bit-rate settings of the
 *               CAN interface and sets the operation state of the CAN controller
 *               to 'running'.
 *
 *  @note        All statistical counters (tx/rx/err) will be reset by this.
 *
 *  @param[in]   handle  - handle of the CAN interface
 *  @param[in]   bitrate - bit-rate as btr register or baud rate index
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - interface not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      CANERR_NULLPTR   - null-pointer assignment
 *  @retval      CANERR_BAUDRATE  - illegal bit-rate settings
 *  @retval      CANERR_ONLINE    - interface already started
 *  @retval      others           - vendor-specific
 */
CANAPI int can_start(int handle, const can_bitrate_t *bitrate);


/** @brief       stops any operation of the CAN interface and sets the operation
 *               state of the CAN controller to 'stopped'; no communication is
 *               possible in this state.
 *
 *  @param[in]   handle  - handle of the CAN interface
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - interface not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      CANERR_OFFLINE   - interface already stopped
 *  @retval      others           - vendor-specific
 */
CANAPI int can_reset(int handle);


/** @brief       transmits a message over the CAN bus. The CAN controller must be
 *               in operation state 'running'.
 *
 *  @param[in]   handle  - handle of the CAN interface
 *  @param[in]   message - pointer to the message to send
 *  @param[in]   timeout - time to wait for the transmission of a message:
 *                              0 means the function returns immediately,
 *                              65535 means blocking read, and any other
 *                              value means the time to wait im milliseconds
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - interface not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      CANERR_NULLPTR   - null-pointer assignment
 *  @retval      CANERR_ILLPARA   - illegal data length code
 *  @retval      CANERR_OFFLINE   - interface not started
 *  @retval      CANERR_TX_BUSY   - transmitter busy
 *  @retval      others           - vendor-specific
 */
CANAPI int can_write(int handle, const can_message_t *message, uint16_t timeout);


/** @brief       read one message from the message queue of the CAN interface, if
 *               any message was received. The CAN controller must be in operation
 *               state 'running'.
 *
 *  @param[in]   handle  - handle of the CAN interface
 *  @param[out]  message - pointer to a message buffer
 *  @param[in]   timeout - time to wait for the reception of a message:
 *                              0 means the function returns immediately,
 *                              65535 means blocking read, and any other
 *                              value means the time to wait im milliseconds
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - library not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      CANERR_NULLPTR   - null-pointer assignment
 *  @retval      CANERR_OFFLINE   - interface not started
 *  @retval      CANERR_RX_EMPTY  - message queue empty
 *  @retval      CANERR_ERR_FRAME - error frame received
 *  @retval      others           - vendor-specific
 */
CANAPI int can_read(int handle, can_message_t *message, uint16_t timeout);


#if defined (_WIN32) || defined(_WIN64)
/** @brief       signals a waiting event object of the CAN interface. This can
 *               be used to terminate a blocking read operation in progress
 *               (e.g. by means of a Ctrl-C handler or similar).
 *
 *  @remark      This driver DLL uses an event object to realize blocking
 *               read by a call to WaitForSingleObject, but this event object
 *               is not terminated by Ctrl-C (SIGINT).
 *
 *  @note        SIGINT is not supported for any Win32 application. [MSVC Docs]
 *
 *  @param[in]   handle  - handle of the CAN interface, or (-1) to signal all
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - interface not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      CANERR_NOTSUPP   - function not supported
 *  @retval      others           - vendor-specific
 */
CANAPI int can_kill(int handle);
#endif


/** @brief       retrieves the status register of the CAN interface.
 *
 *  @param[in]   handle  - handle of the CAN interface.
 *  @param[out]  status  - 8-bit status register.
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - interface not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      others           - vendor-specific
 */
CANAPI int can_status(int handle, uint8_t *status);


/** @brief       retrieves the bus-load (in percent) of the CAN interface.
 *
 *  @param[in]   handle  - handle of the CAN interface
 *  @param[out]  load    - bus-load in [percent]
 *  @param[out]  status  - 8-bit status register
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - interface not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      others           - vendor-specific
 */
CANAPI int can_busload(int handle, uint8_t *load, uint8_t *status);


/** @brief       retrieves the bit-rate setting of the CAN interface. The
 *               CAN controller must be in operation state 'running'.
 *
 *  @param[in]   handle  - handle of the CAN interface
 *  @param[out]  bitrate - bit-rate setting
 *  @param[out]  speed   - transmission rate
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - interface not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      CANERR_OFFLINE   - interface not started
 *  @retval      CANERR_BAUDRATE  - invalid bit-rate settings
 *  @retval      CANERR_NOTSUPP   - function not supported
 *  @retval      others           - vendor-specific
 */
CANAPI int can_bitrate(int handle, can_bitrate_t *bitrate, can_speed_t *speed);


/** @brief       retrieves or modifies a property value of the CAN interface.
 *
 *  @note        To read or to write a property value of the CAN interface DLL,
 *               -1 can be given as handle.
 *
 *  @param[in]   handle   - handle of the CAN interface, or (-1)
 *  @param[in]   param    - property id to be read or to be written
 *  @param[out]  value    - pointer to a buffer for the value to be read
 *  @param[in]   value    - pointer to a buffer with the value to be written
 *  @param[in]   nbytes   - size of the given buffer in bytes
 *
 *  @returns     0 if successful, or a negative value on error.
 *
 *  @retval      CANERR_NOTINIT   - interface not initialized
 *  @retval      CANERR_HANDLE    - invalid interface handle
 *  @retval      CANERR_NULLPTR   - null-pointer assignment
 *  @retval      CANERR_ILLPARA   - illegal parameter, value or nbytes
 *  @retval      CANERR_...       - tbd.
 *  @retval      CANERR_NOTSUPP   - property or function not supported
 *  @retval      others           - vendor-specific
 */
CANAPI int can_property(int handle, uint16_t param, void *value, uint32_t nbytes);


/** @brief       retrieves the hardware version of the CAN controller
 *               board as a zero-terminated string.
 *
 *  @param[in]   handle  - handle of the CAN interface
 *
 *  @returns     pointer to a zero-terminated string, or NULL on error.
 */
CANAPI char *can_hardware(int handle);


/** @brief       retrieves the firmware version of the CAN controller
 *               board as a zero-terminated string.
 *
 *  @param[in]   handle  - handle of the CAN interface
 *
 *  @returns     pointer to a zero-terminated string, or NULL on error.
 */
CANAPI char *can_software(int handle);


/** @brief       retrieves version information of the CAN interface DLL
 *               as a zero-terminated string.
 *
 *  @returns     pointer to a zero-terminated string, or NULL on error.
 */
CANAPI char* can_version();


#ifdef __cplusplus
}
#endif
#endif /* CAN_API_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
