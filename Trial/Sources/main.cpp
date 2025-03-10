//
//  main.cpp
//  PCBUSB-Wrapper
//  Bart Simpson didn´t do it
//
#include "PeakCAN_Defines.h"
#include "PeakCAN_Defaults.h"
#include "PeakCAN.h"

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#if !defined(_WIN32) && !defined(_WIN64)
 #include <unistd.h>
#else
 #include <windows.h>
#endif
#include <inttypes.h>

//#define SECOND_CHANNEL
#ifdef __APPLE__
#define ISSUE_198   (1)
#else
#define ISSUE_198   (0)
#endif
#if (OPTION_PCAN_BIT_TIMING == 1)
#define BITRATE_1M(x)    PEAKCAN_BR_1M(x)
#define BITRATE_800K(x)  PEAKCAN_BR_800K(x)
#define BITRATE_500K(x)  PEAKCAN_BR_500K(x)
#define BITRATE_250K(x)  PEAKCAN_BR_250K(x)
#define BITRATE_125K(x)  PEAKCAN_BR_125K(x)
#define BITRATE_100K(x)  PEAKCAN_BR_100K(x)
#define BITRATE_50K(x)   PEAKCAN_BR_50K(x)
#define BITRATE_20K(x)   PEAKCAN_BR_20K(x)
#define BITRATE_10K(x)   PEAKCAN_BR_10K(x)
#define BITRATE_5K(x)    PEAKCAN_BR_5K(x)
#else
#define BITRATE_1M(x)    DEFAULT_CAN_BR_1M(x)
#define BITRATE_800K(x)  DEFAULT_CAN_BR_800K(x)
#define BITRATE_500K(x)  DEFAULT_CAN_BR_500K(x)
#define BITRATE_250K(x)  DEFAULT_CAN_BR_250K(x)
#define BITRATE_125K(x)  DEFAULT_CAN_BR_125K(x)
#define BITRATE_100K(x)  DEFAULT_CAN_BR_100K(x)
#define BITRATE_50K(x)   DEFAULT_CAN_BR_50K(x)
#define BITRATE_20K(x)   DEFAULT_CAN_BR_20K(x)
#define BITRATE_10K(x)   DEFAULT_CAN_BR_10K(x)
#define BITRATE_5K(x)    DEFAULT_CAN_BR_5K(x)
#endif
#if (OPTION_PCAN_BIT_TIMING == 1)
#define BITRATE_FD_1M(x)      PEAKCAN_FD_BR_1M(x)
#define BITRATE_FD_500K(x)    PEAKCAN_FD_BR_500K(x)
#define BITRATE_FD_250K(x)    PEAKCAN_FD_BR_250K(x)
#define BITRATE_FD_125K(x)    PEAKCAN_FD_BR_125K(x)
#define BITRATE_FD_1M8M(x)    PEAKCAN_FD_BR_1M8M(x)
#define BITRATE_FD_500K4M(x)  PEAKCAN_FD_BR_500K4M(x)
#define BITRATE_FD_250K2M(x)  PEAKCAN_FD_BR_250K2M(x)
#define BITRATE_FD_125K1M(x)  PEAKCAN_FD_BR_125K1M(x)
#else
#define BITRATE_FD_1M(x)      DEFAULT_CAN_FD_BR_1M(x)
#define BITRATE_FD_500K(x)    DEFAULT_CAN_FD_BR_500K(x)
#define BITRATE_FD_250K(x)    DEFAULT_CAN_FD_BR_250K(x)
#define BITRATE_FD_125K(x)    DEFAULT_CAN_FD_BR_125K(x)
#define BITRATE_FD_1M8M(x)    DEFAULT_CAN_FD_BR_1M8M(x)
#define BITRATE_FD_500K4M(x)  DEFAULT_CAN_FD_BR_500K4M(x)
#define BITRATE_FD_250K2M(x)  DEFAULT_CAN_FD_BR_250K2M(x)
#define BITRATE_FD_125K1M(x)  DEFAULT_CAN_FD_BR_125K1M(x)
#endif
#define OPTION_NO   (0)
#define OPTION_YES  (1)

#define OPTION_TIME_DRIVER  (0)
#define OPTION_TIME_ZERO    (1)
#define OPTION_TIME_ABS     (2)
#define OPTION_TIME_REL     (3)

#define CHANNEL   PCAN_USB1

typedef CPeakCAN  CCanDriver;

#if defined(_WIN32) || defined(_WIN64)
 static void usleep(unsigned int usec);
 /* useconds_t: to be compatible with macOS */
 typedef unsigned int  useconds_t;
#endif
static void sigterm(int signo);

static void verbose(const can_mode_t &mode, const can_bitrate_t &bitrate, const can_speed_t &speed);

static volatile int running = 1;

static CCanDriver myDriver = CCanDriver();
#ifdef SECOND_CHANNEL
 static CCanDriver mySecond = CCanDriver();
#endif

int main(int argc, const char * argv[]) {
    CANAPI_OpMode_t opMode = {};
    opMode.byte = CANMODE_DEFAULT;
    CANAPI_Status_t status = {};
    status.byte = CANSTAT_RESET;
    CANAPI_Bitrate_t bitrate = {};
    bitrate.index = CANBTR_INDEX_250K;
    CANAPI_Message_t message = {};
    message.id = 0x000U;
    message.xtd = 0;
    message.rtr = 0;
    message.dlc = CAN_MAX_DLC;
    message.data[0] = 0x00U;
    message.data[1] = 0x00U;
    message.data[2] = 0x00U;
    message.data[3] = 0x00U;
    message.data[4] = 0x00U;
    message.data[5] = 0x00U;
    message.data[6] = 0x00U;
    message.data[7] = 0x00U;
    message.timestamp.tv_sec = 0;
    message.timestamp.tv_nsec = 0;
    CANAPI_Return_t retVal = 0;
    int32_t channel = (int32_t)CHANNEL;
    uint32_t accCode11 = CANACC_CODE_11BIT;
    uint32_t accMask11 = CANACC_MASK_11BIT;
    uint32_t accCode29 = CANACC_CODE_29BIT;
    uint32_t accMask29 = CANACC_MASK_29BIT;
    uint16_t rxTimeout = CANWAIT_INFINITE;
    uint16_t txTimeout = 0U;
    useconds_t txDelay = 0U;
    CCanApi::SChannelInfo info;
    CCanApi::EChannelState state;
//    int32_t clocks[CANPROP_MAX_BUFFER_SIZE/sizeof(int32_t)];
    char szVal[CANPROP_MAX_BUFFER_SIZE];
    uint16_t u16Val;
    uint32_t u32Val;
    uint8_t u8Val;
    int32_t i32Val;
    int frames = 0;
    int option_info = OPTION_NO;
    int option_stat = OPTION_NO;
    int option_test = OPTION_NO;
    int option_list = OPTION_NO;
//    int option_path = OPTION_NO;
    int option_exit = OPTION_NO;
    int option_echo = OPTION_YES;
    int option_stop = OPTION_NO;
    int option_check = ISSUE_198;
    int option_retry = OPTION_NO;
    int option_reply = OPTION_NO;
    int option_transmit = OPTION_NO;
    int option_extended = OPTION_NO;
//    int option_device_id = OPTION_NO;
//    int option_trace = OPTION_NO;
//    int option_log = OPTION_NO;
    int option_xor = OPTION_NO;
    uint64_t received = 0ULL;
    uint64_t expected = 0ULL;
    time_t now = 0L;

    for (int i = 1, opt = 0; i < argc; i++) {
        /* PCAN-USB channel */
        if (!strcmp(argv[i], "PCAN-USB1") || !strcmp(argv[i], "CH:0")) channel = (int32_t)PCAN_USB1;
        if (!strcmp(argv[i], "PCAN-USB2") || !strcmp(argv[i], "CH:1")) channel = (int32_t)PCAN_USB2;
        if (!strcmp(argv[i], "PCAN-USB3") || !strcmp(argv[i], "CH:2")) channel = (int32_t)PCAN_USB3;
        if (!strcmp(argv[i], "PCAN-USB4") || !strcmp(argv[i], "CH:3")) channel = (int32_t)PCAN_USB4;
        if (!strcmp(argv[i], "PCAN-USB5") || !strcmp(argv[i], "CH:4")) channel = (int32_t)PCAN_USB5;
        if (!strcmp(argv[i], "PCAN-USB6") || !strcmp(argv[i], "CH:5")) channel = (int32_t)PCAN_USB6;
        if (!strcmp(argv[i], "PCAN-USB7") || !strcmp(argv[i], "CH:6")) channel = (int32_t)PCAN_USB7;
        if (!strcmp(argv[i], "PCAN-USB8") || !strcmp(argv[i], "CH:7")) channel = (int32_t)PCAN_USB8;
        if (!strcmp(argv[i], "PCAN-USB9") || !strcmp(argv[i], "CH:8")) channel = (int32_t)PCAN_USB9;
        if (!strcmp(argv[i], "PCAN-USB10") || !strcmp(argv[i], "CH:9")) channel = (int32_t)PCAN_USB10;
        if (!strcmp(argv[i], "PCAN-USB11") || !strcmp(argv[i], "CH:10")) channel = (int32_t)PCAN_USB11;
        if (!strcmp(argv[i], "PCAN-USB12") || !strcmp(argv[i], "CH:11")) channel = (int32_t)PCAN_USB12;
        if (!strcmp(argv[i], "PCAN-USB13") || !strcmp(argv[i], "CH:12")) channel = (int32_t)PCAN_USB13;
        if (!strcmp(argv[i], "PCAN-USB14") || !strcmp(argv[i], "CH:13")) channel = (int32_t)PCAN_USB14;
        if (!strcmp(argv[i], "PCAN-USB15") || !strcmp(argv[i], "CH:14")) channel = (int32_t)PCAN_USB15;
        if (!strcmp(argv[i], "PCAN-USB16") || !strcmp(argv[i], "CH:15")) channel = (int32_t)PCAN_USB16;
        /* baud rate (CAN 2.0) */
        if (!strcmp(argv[i], "BD:0") || !strcmp(argv[i], "BD:1000")) bitrate.index = CANBTR_INDEX_1M;
        if (!strcmp(argv[i], "BD:1") || !strcmp(argv[i], "BD:800")) bitrate.index = CANBTR_INDEX_800K;
        if (!strcmp(argv[i], "BD:2") || !strcmp(argv[i], "BD:500")) bitrate.index = CANBTR_INDEX_500K;
        if (!strcmp(argv[i], "BD:3") || !strcmp(argv[i], "BD:250")) bitrate.index = CANBTR_INDEX_250K;
        if (!strcmp(argv[i], "BD:4") || !strcmp(argv[i], "BD:125")) bitrate.index = CANBTR_INDEX_125K;
        if (!strcmp(argv[i], "BD:5") || !strcmp(argv[i], "BD:100")) bitrate.index = CANBTR_INDEX_100K;
        if (!strcmp(argv[i], "BD:6") || !strcmp(argv[i], "BD:50")) bitrate.index = CANBTR_INDEX_50K;
        if (!strcmp(argv[i], "BD:7") || !strcmp(argv[i], "BD:20")) bitrate.index = CANBTR_INDEX_20K;
        if (!strcmp(argv[i], "BD:8") || !strcmp(argv[i], "BD:10")) bitrate.index = CANBTR_INDEX_10K;
        if (!strcmp(argv[i], "BD:1M")) BITRATE_1M(bitrate);
        if (!strcmp(argv[i], "BD:800K")) BITRATE_800K(bitrate);
        if (!strcmp(argv[i], "BD:500K")) BITRATE_500K(bitrate);
        if (!strcmp(argv[i], "BD:250K")) BITRATE_250K(bitrate);
        if (!strcmp(argv[i], "BD:125K")) BITRATE_125K(bitrate);
        if (!strcmp(argv[i], "BD:100K")) BITRATE_100K(bitrate);
        if (!strcmp(argv[i], "BD:50K")) BITRATE_50K(bitrate);
        if (!strcmp(argv[i], "BD:20K")) BITRATE_20K(bitrate);
        if (!strcmp(argv[i], "BD:10K")) BITRATE_10K(bitrate);
        if (!strcmp(argv[i], "BD:5K")) BITRATE_5K(bitrate);
        /* CAN FD operation */
        if (!strcmp(argv[i], "CANFD") || !strcmp(argv[i], "FD")) opMode.fdoe = 1;
        if (!strcmp(argv[i], "FDF")) opMode.fdoe = 1;
        if (!strcmp(argv[i], "BRS")) opMode.brse = 1;
        /* bit rate (CAN FD) */
        if (!strcmp(argv[i], "BR:125K1M")) BITRATE_FD_125K1M(bitrate);
        if (!strcmp(argv[i], "BR:250K2M")) BITRATE_FD_250K2M(bitrate);
        if (!strcmp(argv[i], "BR:500K4M")) BITRATE_FD_500K4M(bitrate);
        if (!strcmp(argv[i], "BR:1M8M")) BITRATE_FD_1M8M(bitrate);
        if (!strcmp(argv[i], "BR:125K")) BITRATE_FD_125K(bitrate);
        if (!strcmp(argv[i], "BR:250K")) BITRATE_FD_250K(bitrate);
        if (!strcmp(argv[i], "BR:500K")) BITRATE_FD_500K(bitrate);
        if (!strcmp(argv[i], "BR:1M")) BITRATE_FD_1M(bitrate);
        /* asynchronous IO */
        if (!strcmp(argv[i], "POLLING")) rxTimeout = 0U;
        if (!strcmp(argv[i], "BLOCKING")) rxTimeout = CANWAIT_INFINITE;
        if (!strncmp(argv[i], "R:", 2) && sscanf(argv[i], "R:%i", &opt) == 1) rxTimeout = (useconds_t)opt;
        /* transmit messages */
        if ((sscanf(argv[i], "%i", &opt) == 1) && (opt > 0)) option_transmit = opt;
        if (!strcmp(argv[i], "EXT") || !strcmp(argv[i], "EXTENDED")) option_extended = OPTION_YES;
//        if (!strncmp(argv[i], "T:", 2) && sscanf(argv[i], "T:%i", &opt) == 1) txTimeout = (useconds_t)opt;
        if (!strncmp(argv[i], "C:", 2) && sscanf(argv[i], "C:%i", &opt) == 1) txDelay = (useconds_t)opt * 1000U;
        if (!strncmp(argv[i], "U:", 2) && sscanf(argv[i], "U:%i", &opt) == 1) txDelay = (useconds_t)opt;
        /* receive messages */
        if (!strcmp(argv[i], "STOP")) option_stop = OPTION_YES;
#if (ISSUE_198 == 0)
        if (!strcmp(argv[i], "CHECK")) option_check = OPTION_YES;
#else
        if (!strcmp(argv[i], "IGNORE")) option_check = OPTION_NO;
#endif
        if (!strcmp(argv[i], "RETRY")) option_retry = OPTION_YES;
        if (!strcmp(argv[i], "REPLY")) option_reply = OPTION_YES;
        if (!strcmp(argv[i], "XOR:ON")) option_xor = OPTION_YES;
        if (!strcmp(argv[i], "SILENT")) option_echo = OPTION_NO;
        /* time-stamps */
//        if (!strcmp(argv[i], "ZERO")) option_time = OPTION_TIME_ZERO;
//        if (!strcmp(argv[i], "ABS") || !strcmp(argv[i], "ABSOLUTE")) option_time = OPTION_TIME_ABS;
//        if (!strcmp(argv[i], "REL") || !strcmp(argv[i], "RELATIVE")) option_time = OPTION_TIME_REL;
        /* logging and debugging */
//        if (!strcmp(argv[i], "TRACE")) option_trace = OPTION_YES;
//        if (!strcmp(argv[i], "LOG")) option_log = OPTION_YES;
        /* query some informations: hw, sw, etc. */
        if (!strcmp(argv[i], "INFO")) option_info = OPTION_YES;
        if (!strcmp(argv[i], "STAT")) option_stat = OPTION_YES;
        if (!strcmp(argv[i], "TEST")) option_test = OPTION_YES;
        if (!strcmp(argv[i], "LIST")) option_list = OPTION_YES;
//        if (!strcmp(argv[i], "PATH")) option_path = OPTION_YES;
        if (!strcmp(argv[i], "EXIT")) option_exit = OPTION_YES;
        /* additional operation modes (bit field) */
        if (!strcmp(argv[i], "SHARED")) opMode.shrd = 1;
        if (!strcmp(argv[i], "MONITOR")) opMode.mon = 1;
        if (!strcmp(argv[i], "MON:ON")) opMode.mon = 1;
        if (!strcmp(argv[i], "ERR:ON")) opMode.err = 1;
        if (!strcmp(argv[i], "XTD:OFF")) opMode.nxtd = 1;
        if (!strcmp(argv[i], "RTR:OFF")) opMode.nrtr = 1;
        /* acceptance filtering */
        if (!strncmp(argv[i], "CODE:", 5) && sscanf(argv[i], "CODE:%x", &opt) == 1) accCode11 = (uint32_t)opt;
        if (!strncmp(argv[i], "MASK:", 5) && sscanf(argv[i], "MASK:%x", &opt) == 1) accMask11 = (uint32_t)opt;
        if (!strncmp(argv[i], "XCODE:", 6) && sscanf(argv[i], "XCODE:%x", &opt) == 1) accCode29 = (uint32_t)opt;
        if (!strncmp(argv[i], "XMASK:", 6) && sscanf(argv[i], "XMASK:%x", &opt) == 1) accMask29 = (uint32_t)opt;
    }
    fprintf(stdout, ">>> %s\n", CCanDriver::GetVersion());
    if ((signal(SIGINT, sigterm) == SIG_ERR) ||
#if !defined(_WIN32) && !defined(_WIN64)
       (signal(SIGHUP, sigterm) == SIG_ERR) ||
#endif
       (signal(SIGTERM, sigterm) == SIG_ERR)) {
        perror("+++ error");
        return errno;
    }
    /* information from library */
    if (option_info) {
        retVal = myDriver.GetProperty(CANPROP_GET_SPEC, (void*)&u16Val, sizeof(uint16_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_SPEC): value = %u.%u\n", (uint8_t)(u16Val >> 8), (uint8_t)u16Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_SPEC) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_VERSION, (void*)&u16Val, sizeof(uint16_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_VERSION): value = %u.%u\n", (uint8_t)(u16Val >> 8), (uint8_t)u16Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_VERSION) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_PATCH_NO, (void*)&u8Val, sizeof(uint8_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_PATCH_NO): value = %u\n", u8Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_PATCH_NO) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_BUILD_NO, (void*)&u32Val, sizeof(uint32_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_BUILD_NO): value = %07" PRIx32 "\n", u32Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_BUILD_NO) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_LIBRARY_ID, (void*)&i32Val, sizeof(int32_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_LIBRARY_ID): value = %d\n", i32Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_LIBRARY_ID) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_LIBRARY_DLLNAME, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_LIBRARY_DLLNAME): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_LIBRARY_DLLNAME) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_LIBRARY_VENDOR, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_LIBRARY_VENDOR): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_LIBRARY_VENDOR) returned %i\n", retVal);
        if (option_exit && !option_list && !option_test)
            return 0;
    }
    /* device list */
    if (option_list) {
        int n = 0;
#if (1)
        bool result = CCanDriver::GetFirstChannel(info);
        while (result) {
            fprintf(stdout, ">>> CCanAPI::Get%sChannel(): %i = \'%s\' (%i = \'%s\')\n", !n ? "First" : "Next",
                    info.m_nChannelNo, info.m_szDeviceName, info.m_nLibraryId, info.m_szVendorName);
            result = CCanDriver::GetNextChannel(info);
            n++;
        }
#else
        retVal = myDriver.SetProperty(CANPROP_SET_FIRST_CHANNEL, (void*)NULL, 0U);
        while (retVal == CCanApi::NoError) {
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_SET_%s_CHANNEL): OK\n", !n ? "FIRST" : "NEXT");
            retVal = myDriver.GetProperty(CANPROP_GET_CHANNEL_NO, (void*)&i32Val, sizeof(int32_t));
            if (retVal == CCanApi::NoError)
                fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_CHANNEL_NO): value = %d\n", i32Val);
            else
                fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_CHANNEL_NO) returned %i\n", retVal);
            retVal = myDriver.GetProperty(CANPROP_GET_CHANNEL_NAME, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
            if (retVal == CCanApi::NoError)
                fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_CHANNEL_NAME): value = '%s'\n", szVal);
            else
                fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_CHANNEL_NAME) returned %i\n", retVal);
            retVal = myDriver.GetProperty(CANPROP_GET_CHANNEL_DLLNAME, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
            if (retVal == CCanApi::NoError)
                fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_CHANNEL_DLLNAME): value = '%s'\n", szVal);
            else
                fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_CHANNEL_DLLNAME) returned %i\n", retVal);
            retVal = myDriver.GetProperty(CANPROP_GET_CHANNEL_VENDOR_ID, (void*)&i32Val, sizeof(int32_t));
            if (retVal == CCanApi::NoError)
                fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_CHANNEL_VENDOR_ID): value = %d\n", i32Val);
            else
                fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_CHANNEL_VENDOR_ID) returned %i\n", retVal);
            retVal = myDriver.GetProperty(CANPROP_GET_CHANNEL_VENDOR_NAME, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
            if (retVal == CCanApi::NoError)
                fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_CHANNEL_VENDOR_NAME): value = '%s'\n", szVal);
            else
                fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_CHANNEL_VENDOR_NAME) returned %i\n", retVal);
            n++;
        }
#endif
        if (option_exit && !option_test)
            return 0;
    }
    /* channel tester */
    if (option_test) {
#if (1)
        bool result = CCanDriver::GetFirstChannel(info);
        while (result) {
            retVal = CCanDriver::ProbeChannel(info.m_nChannelNo, opMode, state);
            fprintf(stdout, ">>> CCanAPI::ProbeChannel(%i): %s = %s",
                             info.m_nChannelNo, info.m_szDeviceName,
                            (state == CCanApi::ChannelOccupied) ? "occupied" :
                            (state == CCanApi::ChannelAvailable) ? "available" :
                            (state == CCanApi::ChannelNotAvailable) ? "not available" : "not testable");
            fprintf(stdout, "%s", (retVal == CCanApi::IllegalParameter) ? " (warning: Op.-Mode not supported)\n" : "\n");
            result = CCanDriver::GetNextChannel(info);
        }
#else
        retVal = myDriver.SetProperty(CANPROP_SET_FIRST_CHANNEL, (void*)NULL, 0U);
        while (retVal == CCanApi::NoError) {
            retVal = myDriver.GetProperty(CANPROP_GET_CHANNEL_NO, (void*)&i32Val, sizeof(int32_t));
            if (retVal == CCanApi::NoError) {
                retVal = CCanDriver::ProbeChannel(i32Val, opMode, state);
                fprintf(stdout, ">>> CCanApi::ProbeChannel(%i): state = %s", i32Val,
                                (state == CCanApi::ChannelOccupied) ? "occupied" :
                                (state == CCanApi::ChannelAvailable) ? "available" :
                                (state == CCanApi::ChannelNotAvailable) ? "not available" : "not testable");
                fprintf(stdout, "%s", (retVal == CCanApi::IllegalParameter) ? " (warning: Op.-Mode not supported)\n" : "\n");
            }
            retVal = myDriver.SetProperty(CANPROP_SET_NEXT_CHANNEL, (void*)NULL, 0U);
        }
#endif
        if (option_exit)
            return 0;
    }
    /* initialization */
    retVal = myDriver.InitializeChannel(channel, opMode);
    if (retVal != CCanApi::NoError) {
        fprintf(stderr, "+++ error: myDriver.InitializeChannel(%i) returned %i\n", channel, retVal);
        goto end;
    }
    else if (myDriver.GetStatus(status) == CCanApi::NoError) {
        fprintf(stdout, ">>> myDriver.InitializeChannel(%i): status = 0x%02X\n", channel, status.byte);
    }
    /* channel status */
    if (option_test) {
        retVal = myDriver.ProbeChannel(channel, opMode, state);
        fprintf(stdout, ">>> myDriver.ProbeChannel(%i): state = %s", channel,
                        (state == CCanApi::ChannelOccupied) ? "now occupied" :
                        (state == CCanApi::ChannelAvailable) ? "available" :
                        (state == CCanApi::ChannelNotAvailable) ? "not available" : "not testable");
        fprintf(stdout, "%s", (retVal == CCanApi::IllegalParameter) ? " (warning: Op.-Mode not supported)\n" : "\n");
    }
    /* information from driver */
    if (option_info) {
        retVal = myDriver.GetProperty(CANPROP_GET_NUM_CHANNELS, (void*)&u8Val, sizeof(uint8_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_NUM_CHANNELS): value = %d\n", u8Val);
        //else [optional property]
        //    fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_NUM_CHANNELS) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_CAN_CHANNEL, (void*)&u8Val, sizeof(uint8_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_CAN_CHANNEL): value = %u\n", u8Val);
        //else [optional property]
        //    fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_CAN_CHANNEL) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_DEVICE_TYPE, (void*)&i32Val, sizeof(int32_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_DEVICE_TYPE): value = %d\n", i32Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_DEVICE_TYPE) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_DEVICE_NAME, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_DEVICE_NAME): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_DEVICE_NAME) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_DEVICE_VENDOR, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_DEVICE_VENDOR): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_DEVICE_VENDOR) returned %i\n", retVal);
        retVal = myDriver.GetProperty(CANPROP_GET_DEVICE_DLLNAME, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_DEVICE_DLLNAME): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_DEVICE_DLLNAME) returned %i\n", retVal);
        /* vendor-specific properties */
        retVal = myDriver.GetProperty(PEAKCAN_PROPERTY_DEVICE_ID, (void*)&u32Val, sizeof(uint32_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(PEAKCAN_PROPERTY_DEVICE_ID): value = 0x%x\n", u32Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(PEAKCAN_PROPERTY_DEVICE_ID) returned %i\n", retVal);
        retVal = myDriver.GetProperty(PEAKCAN_PROPERTY_API_VERSION, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(PEAKCAN_PROPERTY_API_VERSION): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(PEAKCAN_PROPERTY_API_VERSION) returned %i\n", retVal);
        retVal = myDriver.GetProperty(PEAKCAN_PROPERTY_CHANNEL_VERSION, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(PEAKCAN_PROPERTY_CHANNEL_VERSION): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(PEAKCAN_PROPERTY_CHANNEL_VERSION) returned %i\n", retVal);
        retVal = myDriver.GetProperty(PEAKCAN_PROPERTY_HARDWARE_NAME, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(PEAKCAN_PROPERTY_HARDWARE_NAME): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(PEAKCAN_PROPERTY_HARDWARE_NAME) returned %i\n", retVal);
#if (0)
            retVal = myDriver.GetProperty(PEAKCAN_PROPERTY_DEVICE_GUID, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(PEAKCAN_PROPERTY_DEVICE_GUID): value = '%s'\n", szVal);
        //else [optional property]
        //    fprintf(stderr, "+++ error: myDriver.GetProperty(PEAKCAN_PROPERTY_DEVICE_GUID) returned %i\n", retVal);
        retVal = myDriver.GetProperty(PEAKCAN_PROPERTY_SERIAL_NUMBER, (void*)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(PEAKCAN_PROPERTY_SERIAL_NUMBER): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(PEAKCAN_PROPERTY_SERIAL_NUMBER) returned %i\n", retVal);
#endif
        retVal = myDriver.GetProperty(PEAKCAN_PROPERTY_CONTROLLER_NUMBER, (void*)&u32Val, sizeof(uint32_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(PEAKCAN_PROPERTY_CONTROLLER_NUMBER): value = %u\n", u32Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(PEAKCAN_PROPERTY_CONTROLLER_NUMBER) returned %i\n", retVal);
#if (0)
        retVal = myDriver.GetProperty(PEAKCAN_PROPERTY_CAN_CLOCK_DOMAINS, (void*)clocks, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CCanApi::NoError) {
            fprintf(stdout, ">>> myDriver.GetProperty(PEAKCAN_PROPERTY_CAN_CLOCK_DOMAINS): array =");
            for (int i = 0; (clocks[i] != EOF) && (i < (int)(CANPROP_MAX_BUFFER_SIZE/sizeof(int32_t))); i++)
                fprintf(stdout, "%s%.1f", i ? ", " : " [", (float)clocks[i] / (float)1000000);
            fprintf(stdout, "]\n");
        }
        //else [optional property]
        //    fprintf(stderr, "+++ error: myDriver.GetProperty(PEAKCAN_PROPERTY_CAN_CLOCK_DOMAINS) returned %i\n", retVal);
#endif
        retVal = myDriver.GetProperty(CANPROP_GET_CAN_CLOCK, (void*)&i32Val, sizeof(int32_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_CAN_CLOCK): value = %d\n", i32Val);
        //else [optional property]
        //    fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_CAN_CLOCK) returned %i\n", retVal);
        /* device capabilities */
        retVal = myDriver.GetProperty(CANPROP_GET_OP_CAPABILITY, (void*)&u8Val, sizeof(uint8_t));
        if (retVal == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_OP_CAPABILITY): value = 0x%02X\n", u8Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(CANPROP_GET_OP_CAPABILITY) returned %i\n", retVal);
        if (myDriver.GetProperty(CANPROP_GET_OP_MODE, (void*)&u8Val, sizeof(uint8_t)) == CCanApi::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_OP_MODE): value = 0x%02X\n", u8Val);
    }
    /* acceptance filtering */
    if ((accCode11 != CANACC_CODE_11BIT) || (accMask11 != CANACC_MASK_11BIT)) {
        retVal = myDriver.SetFilter11Bit(accCode11, accMask11);
        if (retVal != CCanApi::NoError) {
            fprintf(stderr, "+++ error: myDriver.SetFilter11Bit returned %i\n", retVal);
            goto teardown;
        }
    }
    if ((accCode29 != CANACC_CODE_29BIT) || (accMask29 != CANACC_MASK_29BIT)) {
        retVal = myDriver.SetFilter29Bit(accCode29, accMask29);
        if (retVal != CCanApi::NoError) {
            fprintf(stderr, "+++ error: myDriver.SetFilter29Bit returned %i\n", retVal);
            goto teardown;
        }
    }
    /* start communication */
    retVal = myDriver.StartController(bitrate);
    if (retVal != CCanApi::NoError) {
        fprintf(stderr, "+++ error: myDriver.StartController returned %i\n", retVal);
        goto teardown;
    }
    else if (myDriver.GetStatus(status) == CCanApi::NoError) {
        fprintf(stdout, ">>> myDriver.StartController: status = 0x%02X\n", status.byte);
    }
    if (option_info) {
        CANAPI_BusSpeed_t speed;
        if ((myDriver.GetBitrate(bitrate) == CCanApi::NoError) &&
            (myDriver.GetBusSpeed(speed) == CCanApi::NoError))
            verbose(opMode, bitrate, speed);

        uint32_t code, mask;
        if ((myDriver.GetFilter11Bit(code, mask) == CCanApi::NoError) &&
            ((code != CANACC_CODE_11BIT) || (mask != CANACC_MASK_11BIT)))
            fprintf(stdout, "    Filter11: code = 0x%03X, mask = 0x%03X\n", code, mask);
        if ((myDriver.GetFilter29Bit(code, mask) == CCanApi::NoError) &&
            ((code != CANACC_CODE_29BIT) || (mask != CANACC_MASK_29BIT)))
            fprintf(stdout, "    Filter29: code = 0x%08X, mask = 0x%08X\n", code, mask);
    }
#ifdef SECOND_CHANNEL
    retVal = mySecond.InitializeChannel(channel+1U, opMode);
    if (retVal != CCanApi::NoError) {
        fprintf(stderr, "+++ error: mySecond.InitializeChannel(%i) returned %i\n", channel+1U, retVal);
        goto teardown;
    }
    else if (myDriver.GetStatus(status) == CCanApi::NoError) {
        fprintf(stdout, ">>> mySecond.InitializeChannel(%i): status = 0x%02X\n", channel+1U, status.byte);
    }
    retVal = mySecond.StartController(bitrate);
    if (retVal != CCanApi::NoError) {
        fprintf(stderr, "+++ error: mySecond.StartController returned %i\n", retVal);
        goto teardown;
    }
    else if (myDriver.GetStatus(status) == CCanApi::NoError) {
        fprintf(stdout, ">>> mySecond.StartController: status = 0x%02X\n", status.byte);
    }
#endif
    /* transmit messages */
    if (option_transmit) {
#ifdef __linux__
       if (!option_retry)
           fprintf(stdout, "Attention: The program will throw errors if the transmit queue is full.\n"
                           "           Use program option RETRY to avoid this.\n");
#else
//        if ((txTimeout == 0U) && !option_retry)
//            fprintf(stdout, "Attention: The program will be aborted when the transmitter is busy.\n"
//                            "           Use program option RETRY or T:<timeout> to avoid this.\n");
#endif
        fprintf(stdout, "Press Ctrl+C to abort..."); fflush(stdout);
        frames = 0;
        now = time(NULL);
        while (running && (option_transmit > frames)) {
#if (OPTION_CAN_2_0_ONLY != 0)
            message.dlc = CAN_MAX_DLC;
#else
            if (!opMode.fdoe) {
                message.fdf = 0;
                message.brs = 0;
                message.dlc = CAN_MAX_DLC;
            } else {
                message.fdf = opMode.fdoe;
                message.brs = opMode.brse;
                message.dlc = CANFD_MAX_DLC;
            }
#endif
            message.xtd = option_extended ? 1 : 0;
            message.id = (uint32_t)frames & (message.xtd ? CAN_MAX_XTD_ID : CAN_MAX_STD_ID);
            message.data[0] = (uint8_t)(((uint64_t)frames & 0x00000000000000FF) >> 0);
            message.data[1] = (uint8_t)(((uint64_t)frames & 0x000000000000FF00) >> 8);
            message.data[2] = (uint8_t)(((uint64_t)frames & 0x0000000000FF0000) >> 16);
            message.data[3] = (uint8_t)(((uint64_t)frames & 0x00000000FF000000) >> 24);
            message.data[4] = (uint8_t)(((uint64_t)frames & 0x000000FF00000000) >> 32);
            message.data[5] = (uint8_t)(((uint64_t)frames & 0x0000FF0000000000) >> 40);
            message.data[6] = (uint8_t)(((uint64_t)frames & 0x00FF000000000000) >> 48);
            message.data[7] = (uint8_t)(((uint64_t)frames & 0xFF00000000000000) >> 56);
retry_write:
            retVal = myDriver.WriteMessage(message, txTimeout);
            if ((retVal == CCanApi::TransmitterBusy) && option_retry)
                goto retry_write;
            else if (retVal != CCanApi::NoError) {
                fprintf(stderr, "\n+++ error: myDriver.WriteMessage returned %i\n", retVal);
                goto teardown;
            }
            if (txDelay)
                usleep(txDelay);
            frames++;
        }
        fprintf(stdout, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        if (myDriver.GetStatus(status) == CCanApi::NoError) {
            fprintf(stdout, ">>> myDriver.WriteMessage: status = 0x%02X\n", status.byte);
        }
        fprintf(stdout, "    %i %s message(s) sent (took %.1lfs)\n", frames, option_extended ? "extended" : "standard",
                difftime(time(NULL), now));
        if (option_exit)
            goto teardown;
    }
    /* receiving message */
    fprintf(stdout, "Press Ctrl+C to abort...\n");
    frames = 0;
    while (running) {
        if ((retVal = myDriver.ReadMessage(message, rxTimeout)) == CCanApi::NoError) {
            if (option_echo) {
                fprintf(stdout, ">>> %i\t", frames++);
                fprintf(stdout, "%7li.%04li\t", (long)message.timestamp.tv_sec, message.timestamp.tv_nsec / 100000);
#if (OPTION_CAN_2_0_ONLY != 0)
                fprintf(stdout, "%03x\t%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ', message.dlc);
#else
                if (!opMode.fdoe)
                    fprintf(stdout, "%03x\t%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ', message.dlc);
                else
                    fprintf(stdout, "%03x\t%c%c%c%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ',
                            message.fdf ? 'F' : ' ', message.brs ? 'B' : ' ', message.esi ? 'E' :' ', CCanApi::Dlc2Len(message.dlc));
#endif
                for (uint8_t i = 0; i < CCanApi::Dlc2Len(message.dlc); i++)
                    fprintf(stdout, " %02x", message.data[i]);
                if (message.sts) {
                    fprintf(stdout, " <<< status frame");
                    if (myDriver.GetStatus(status) == CCanApi::NoError) {
                        fprintf(stdout, " (0x%02X)", status.byte);
                    }
                }
                fprintf(stdout, "\n");
            } else {
                if (!(frames++ % 2048)) {
                    fprintf(stdout, ".");
                    fflush(stdout);
                }
            }
            if (option_check && !message.sts) {
                received = 0;
                if (message.dlc > 0) received |= (uint64_t)message.data[0] << 0;
                if (message.dlc > 1) received |= (uint64_t)message.data[1] << 8;
                if (message.dlc > 2) received |= (uint64_t)message.data[2] << 16;
                if (message.dlc > 3) received |= (uint64_t)message.data[3] << 24;
                if (message.dlc > 4) received |= (uint64_t)message.data[4] << 32;
                if (message.dlc > 5) received |= (uint64_t)message.data[5] << 40;
                if (message.dlc > 6) received |= (uint64_t)message.data[6] << 48;
                if (message.dlc > 7) received |= (uint64_t)message.data[7] << 56;
                if (received != expected) {
                    fprintf(stderr, "+++ error: received data is not equal to expected data (%" PRIu64 " : %" PRIu64 ")\n", received, expected);
                    if (expected > received)
                        fprintf(stderr, "           issue #198: old messages read again (offset -%" PRIu64 ")\a\n", expected - received);
                    if (option_stop)
                        goto teardown;
                }
                expected = received + 1;
            }
            if (option_reply) {
                if (option_xor)
                    message.id = (message.id ^ UINT32_MAX) & (message.xtd ? CAN_MAX_XTD_ID : CAN_MAX_STD_ID);
                else
                    message.id = message.xtd ? ((message.id + 0x10000000U) & CAN_MAX_XTD_ID) : ((message.id + 0x400U) & CAN_MAX_STD_ID);
                for (uint8_t i = 0; i < CCanApi::Dlc2Len(message.dlc); i++)
                    message.data[i] = message.data[i] ^ 0xFFU;
retry_reply:
                retVal = myDriver.WriteMessage(message, txTimeout);
                if ((retVal == CCanApi::TransmitterBusy) && option_retry)
                    goto retry_reply;
                else if (retVal != CCanApi::NoError) {
                    fprintf(stderr, "+++ error: myDriver.WriteMessage returned %i\n", retVal);
                    goto teardown;
                }
            }
        }
        else if (retVal != CCanApi::ReceiverEmpty) {
            fprintf(stdout, ">>> myDriver.ReadMessage returned %i\n", retVal);
            goto teardown;
        }
#ifdef SECOND_CHANNEL
        if ((retVal = mySecond.ReadMessage(message, 0U)) == CCanApi::NoError) {
            if (option_echo) {
                fprintf(stdout, ">2> %i\t", frames++);
                fprintf(stdout, "%7li.%04li\t", (long)message.timestamp.tv_sec, message.timestamp.tv_nsec / 100000);
                if (!opMode.fdoe)
                    fprintf(stdout, "%03x\t%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ', message.dlc);
                else
                    fprintf(stdout, "%03x\t%c%c%c%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ',
                        message.fdf ? 'F' : ' ', message.brs ? 'B' : ' ', message.esi ? 'E' : ' ', CCanApi::Dlc2Len(message.dlc));
                for (uint8_t i = 0; i < CCanApi::Dlc2Len(message.dlc); i++)
                    fprintf(stdout, " %02x", message.data[i]);
                if (message.sts)
                    fprintf(stdout, " <<< status frame");
                else if (option_reply) {
                    retVal = myDriver.WriteMessage(message, txTimeout);
                    if (retVal != CCanApi::NoError) {
                        fprintf(stderr, "+++ error: mySecond.WriteMessage returned %i\n", retVal);
                        goto teardown;
                    }
                }
                fprintf(stdout, "\n");
            }
            else {
                if (!(frames++ % 2048)) {
                    fprintf(stdout, ".");
                    fflush(stdout);
                }
            }
        }
        else if (retVal != CCanApi::ReceiverEmpty) {
            fprintf(stderr, "+++ error: mySecond.ReadMessage returned %i\n", retVal);
            goto teardown;
        }
#endif
    }
    if (myDriver.GetStatus(status) == CCanApi::NoError) {
        fprintf(stdout, "\n>>> myDriver.ReadMessage: status = 0x%02X\n", status.byte);
    }
    /* some statistics */
    if (option_stat || option_info) {
        uint64_t u64TxCnt, u64RxCnt, u64ErrCnt;
        if ((myDriver.GetProperty(CANPROP_GET_TX_COUNTER, (void*)&u64TxCnt, sizeof(uint64_t)) == CCanApi::NoError) &&
            (myDriver.GetProperty(CANPROP_GET_RX_COUNTER, (void*)&u64RxCnt, sizeof(uint64_t)) == CCanApi::NoError) &&
            (myDriver.GetProperty(CANPROP_GET_ERR_COUNTER, (void*)&u64ErrCnt, sizeof(uint64_t)) == CCanApi::NoError))
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_*_COUNTER): TX = %" PRIu64 " RX = %" PRIu64 " ERR = %" PRIu64 "\n", u64TxCnt, u64RxCnt, u64ErrCnt);
    }
    if (option_stat) {
        uint32_t u32QueSize, u32QueHigh; uint64_t u64QueOvfl;
        if ((myDriver.GetProperty(CANPROP_GET_RCV_QUEUE_SIZE, (void*)&u32QueSize, sizeof(uint32_t)) == CCanApi::NoError) &&
            (myDriver.GetProperty(CANPROP_GET_RCV_QUEUE_HIGH, (void*)&u32QueHigh, sizeof(uint32_t)) == CCanApi::NoError) &&
            (myDriver.GetProperty(CANPROP_GET_RCV_QUEUE_OVFL, (void*)&u64QueOvfl, sizeof(uint64_t)) == CCanApi::NoError))
            fprintf(stdout, ">>> myDriver.GetProperty(CANPROP_GET_QUEUE_*): SIZE = %" PRIu32 " HIGH = %" PRIu32 " OVFL = %" PRIu64 "\n", u32QueSize, u32QueHigh, u64QueOvfl);
    }
    /* version information */
    if (option_info) {
        char *hardware = myDriver.GetHardwareVersion();
        if (hardware)
            fprintf(stdout, ">>> myDriver.GetHardwareVersion: '%s'\n", hardware);
        char *firmware = myDriver.GetFirmwareVersion();
        if (firmware)
            fprintf(stdout, ">>> myDriver.GetFirmwareVersion: '%s'\n", firmware);
    }
teardown:
    /* shutdown */
#ifdef SECOND_CHANNEL
    retVal = mySecond.ResetController();
    if (retVal != CCanApi::NoError)
        fprintf(stderr, "+++ error: mySecond.ResetController returned %i\n", retVal);
    retVal = mySecond.TeardownChannel();
    if (retVal != CCanApi::NoError)
        fprintf(stderr, "+++ error: mySecond.TeardownChannel returned %i\n", retVal);
#endif
    retVal = myDriver.TeardownChannel();
    if (retVal != CCanApi::NoError) {
        fprintf(stderr, "+++ error: myDriver.TeardownChannel returned %i\n", retVal);
        goto end;
    }
    else if (myDriver.GetStatus(status) == CCanApi::NoError) {
        fprintf(stdout, ">>> myDriver.TeardownChannel: status = 0x%02X\n", status.byte);
    }
    else {
        fprintf(stdout, "@@@ Resistance is futile!\n");
    }
end:
    fprintf(stdout, "Cheers!\n");
    return retVal;
}

static void verbose(const can_mode_t &mode, const can_bitrate_t &bitrate, const can_speed_t &speed)
{
#if (OPTION_CAN_2_0_ONLY == 0)
    fprintf(stdout, "    Op.-Mode: 0x%02X (fdoe=%u,brse=%u,niso=%u,shrd=%u,nxtd=%u,nrtr=%u,err=%u,mon=%u)\n",
            mode.byte, mode.fdoe, mode.brse, mode.niso, mode.shrd, mode.nxtd, mode.nrtr, mode.err, mode.mon);
#else
    fprintf(stdout, "    Op.-Mode: 0x%02X (shrd=%u,nxtd=%u,nrtr=%u,err=%u,mon=%u)\n",
            mode.byte, mode.shrd, mode.nxtd, mode.nrtr, mode.err, mode.mon);
#endif
    if (bitrate.btr.frequency > 0) {
        fprintf(stdout, "    Baudrate: %.0fkbps@%.1f%%",
            speed.nominal.speed / 1000., speed.nominal.samplepoint * 100.);
#if (OPTION_CAN_2_0_ONLY == 0)
        if (/*speed.data.brse*/mode.fdoe && mode.brse)
            fprintf(stdout, ":%.0fkbps@%.1f%%",
                speed.data.speed / 1000., speed.data.samplepoint * 100.);
#endif
        fprintf(stdout, " (f_clock=%i,nom_brp=%u,nom_tseg1=%u,nom_tseg2=%u,nom_sjw=%u,nom_sam=%u",
            bitrate.btr.frequency,
            bitrate.btr.nominal.brp,
            bitrate.btr.nominal.tseg1,
            bitrate.btr.nominal.tseg2,
            bitrate.btr.nominal.sjw,
            bitrate.btr.nominal.sam);
#if (OPTION_CAN_2_0_ONLY == 0)
        if (mode.fdoe && mode.brse)
            fprintf(stdout, ",data_brp=%u,data_tseg1=%u,data_tseg2=%u,data_sjw=%u",
                bitrate.btr.data.brp,
                bitrate.btr.data.tseg1,
                bitrate.btr.data.tseg2,
                bitrate.btr.data.sjw);
#endif
        fprintf(stdout, ")\n");
    }
    else {
        fprintf(stdout, "    Baudrate: %skbps (CiA index %i)\n",
            bitrate.index == CANBDR_1000 ? "1000" :
            bitrate.index == -CANBDR_800 ? "800" :
            bitrate.index == -CANBDR_500 ? "500" :
            bitrate.index == -CANBDR_250 ? "250" :
            bitrate.index == -CANBDR_125 ? "125" :
            bitrate.index == -CANBDR_100 ? "100" :
            bitrate.index == -CANBDR_50 ? "50" :
            bitrate.index == -CANBDR_20 ? "20" :
            bitrate.index == -CANBDR_10 ? "10" : "?", -bitrate.index);
    }
}

#if defined(_WIN32) || defined(_WIN64)
 /* usleep(3) - Linux man page
  *
  * Notes
  * The type useconds_t is an unsigned integer type capable of holding integers in the range [0,1000000].
  * Programs will be more portable if they never mention this type explicitly. Use
  *
  *    #include <unistd.h>
  *    ...
  *        unsigned int usecs;
  *    ...
  *        usleep(usecs);
  */
 static void usleep(unsigned int usec) {
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * (LONGLONG)usec); // Convert to 100 nanosecond interval, negative value indicates relative time
    if (usec >= 100) {
        if ((timer = CreateWaitableTimer(NULL, TRUE, NULL)) != NULL) {
            SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
            WaitForSingleObject(timer, INFINITE);
            CloseHandle(timer);
        }
    }
    else {
        Sleep(0);
    }
 }
#endif

static void sigterm(int signo) {
    //fprintf(stderr, "%s: got signal %d\n", __FILE__, signo);
    (void)myDriver.SignalChannel();
#ifdef SECOND_CHANNEL
    (void)mySecond.SignalChannel();
#endif
    running = 0;
    (void)signo;
}
