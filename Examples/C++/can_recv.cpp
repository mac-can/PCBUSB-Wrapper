#include <iostream>
#include <signal.h>
#include <errno.h>

#include "PCAN.h"

#ifndef CHANNEL
#define CHANNEL  0x51
#endif
#ifndef BAUDRATE
#define BAUDRATE  CANBTR_INDEX_250K
#endif

static void sigterm(int signo);
static volatile int running = 1;

int main(int argc, const char * argv[]) {
    CPCAN myDriver = CPCAN();
    CANAPI_OpMode_t opMode = {
        .byte = CANMODE_DEFAULT
    };
    CANAPI_Bitrate_t bitrate = {
        .index = BAUDRATE
    };
    CANAPI_Message_t message;
    CANAPI_Return_t retVal = 0;
    int frames = 0;

    std::cout << CPCAN::GetVersion() << std::endl;
    if((signal(SIGINT, sigterm) == SIG_ERR) ||
#if !defined(_WIN32) && !defined(_WIN64)
       (signal(SIGHUP, sigterm) == SIG_ERR) ||
#endif
       (signal(SIGTERM, sigterm) == SIG_ERR)) {
        perror("+++ error");
        return errno;
    }
    if ((retVal = myDriver.InitializeChannel(CHANNEL, opMode)) != CCANAPI::NoError) {
        std::cerr << "+++ error: interface could not be initialized" << std::endl;
        return retVal;
    }
    if ((retVal = myDriver.StartController(bitrate)) != CCANAPI::NoError) {
        std::cerr << "+++ error: interface could not be started" << std::endl;
        goto teardown;
    }
    std::cout << "Press Ctrl+C to abort..." << std::endl;
    while (running) {
        if ((retVal = myDriver.ReadMessage(message, CANREAD_INFINITE)) == CCANAPI::NoError) {
            fprintf(stdout, ">>> %i\t", frames++);
            fprintf(stdout, "%7li.%04li\t", message.timestamp.tv_sec, message.timestamp.tv_nsec / 100000);
            if (!opMode.fdoe)
                fprintf(stdout, "%03x\t%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ', message.dlc);
            else
                fprintf(stdout, "%03x\t%c%c%c%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ',
                        message.fdf ? 'F' : ' ', message.brs ? 'B' : ' ', message.esi ? 'E' :' ', CCANAPI::DLc2Len(message.dlc));
            for (uint8_t i = 0; i < CCANAPI::DLc2Len(message.dlc); i++)
                fprintf(stdout, " %02x", message.data[i]);
            if (message.sts)
                fprintf(stdout, " <<< status frame");
            fprintf(stdout, "\n");
        }
        else if (retVal != CCANAPI::ReceiverEmpty) {
            fprintf(stderr, "+++ error: read message returned %i", retVal);
            running = 0;
        }
    }
    std::cout << std::endl;
teardown:
    if ((retVal = myDriver.TeardownChannel()) != CCANAPI::NoError)
        std::cerr << "+++ error: interface could not be shutdown" << std::endl;
    return retVal;
}

static void sigterm(int signo) {
     //fprintf(stderr, "%s: got signal %d\n", __FILE__, signo);
     running = 0;
     (void)signo;
}