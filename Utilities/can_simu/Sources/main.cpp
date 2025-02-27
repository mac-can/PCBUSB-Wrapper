//  SPDX-License-Identifier: GPL-2.0-or-later
//
//  CAN-over-Ethernet Server Simulator (RocketCAN)
//
//  Copyright (c) 2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, see <https://www.gnu.org/licenses/>.
//
#include "Driver.h"
#include "CanTcpServer.h"
#include "Options.h"
#include "Message.h"
#include "Timer.h"
#if (SERIAL_CAN_SUPPORTED != 0)
#include "SerialCAN_Defines.h"
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include <inttypes.h>

#if defined(_WIN64)
#define PLATFORM  "x64"
#elif defined(_WIN32)
#define PLATFORM  "x86"
#elif defined(__linux__)
#define PLATFORM  "Linux"
#elif defined(__APPLE__)
#define PLATFORM  "macOS"
#elif defined(__CYGWIN__)
#define PLATFORM  "Cygwin"
#else
#error Platform not supported
#endif
#ifdef _MSC_VER
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#define MAX_ID  (CAN_MAX_STD_ID + 1)

class CCanDevice {
public:
    uint64_t SendMessage();
public:
    CANAPI_Return_t SignalChannel() {
        // TODO: kill the server (?)
        return CCanApi::NoError;
    }
};
static void sigterm(int signo);

static volatile int running = 1;

static CCanDevice canDevice = CCanDevice();  // global due to SignalChannel() in sigterm()
static CCanTcpServer ipcServer = CCanTcpServer();

int main(int argc, const char* argv[]) {
    CANAPI_Return_t retVal = CANERR_FATAL;
    bool ipcFault = false;

    /* signal handler */
    if ((signal(SIGINT, sigterm) == SIG_ERR) ||
#if !defined(_WIN32) && !defined(_WIN64)
       (signal(SIGHUP, sigterm) == SIG_ERR) ||
#endif
       (signal(SIGTERM, sigterm) == SIG_ERR)) {
        perror("+++ error");
        return errno;
    }
    /* scan command-line */
    SOptions opts = SOptions();
    if (opts.ScanCommanline(argc, argv) != 0) {
        /* program usage already shown */
        return 1;
    }
    /* RocketCAN Server (simulation) */
    opts.ShowGreetings(stdout);
    /* - accept security risks */
    fprintf(stdout, CAN_SIMU_SECURITY "\n");
    if (!opts.m_fRisksAccepted) {
        fprintf(stdout, "\n" CAN_SIMU_QUESTION);
        fflush(stdout);
        if (getchar() != 'Y') {
            fprintf(stderr, "Execution aborted: Security risks were not accepted by the user.\n");
            goto teardown;
        }
        (void)getchar();  // consume newline
    }
    fprintf(stdout, "\n" CAN_SIMU_ACCEPTED "\n\n");
    /* - start CAN-over-Ethernet server */
    fprintf(stdout, "CAN-over-Ethernet server on port %s...", opts.m_szServerPort);
    fflush(stdout);
    /* -- set callback function and logging level */
    if (!ipcServer.SetCallback(NULL, (void*)&canDevice))
        ipcFault = true;
    if (!ipcServer.SetLoggingLevel(opts.m_nLoggingLevel))
        ipcFault = true;
    if (ipcFault) {
        fprintf(stderr, "+++ error: CAN-over-Ethernet server could not be initialized\n");
        if (errno) perror("+++ cause");
        goto teardown;
    }
    /* -- let the show begin */
    retVal = ipcServer.Start(opts.m_szServerPort);
    if (retVal != 0) {
        fprintf(stdout, "FAILED!\n");
        fprintf(stderr, "+++ error: CAN-over-Ethernet server could not be started (%i)\n", retVal);
        if (errno) perror("+++ cause");
        goto teardown;
    }
    fprintf(stdout, "\b\b\b started.\n");
    /* - parse and send messages */
    canDevice.SendMessage();
	/* - send abort to all clients */
	(void)ipcServer.SendAbort();
    /* - stop CAN-over-Ethernet server */
    retVal = ipcServer.Stop();
    if (retVal != 0) {
        fprintf(stderr, "+++ error: CAN-over-Ethernet server could not be stopped (%i)\n", retVal);
        if (errno) perror("+++ cause");
        goto teardown;
    } else {
        fprintf(stdout, "CAN-over-Ethernet server on port %s stopped.\n\n", opts.m_szServerPort);
    }
teardown:   
    /* So long and farewell! */
    opts.ShowFarewell(stdout);
    return 0;
}

uint64_t CCanDevice::SendMessage() {
    CANAPI_Message_t message = {};
    CANAPI_Return_t retVal;
    char string[1024];
    uint32_t count = 0;
    uint64_t cycle = 0;
    int incr = 0;

    struct timespec t0;
    uint64_t dt = 0;
    uint32_t data = 0;
#if !defined(_WIN32) && !defined(_WIN64)
    fprintf(stdout, "\nEnter a message to send (or ^D to quit):\n");
#else
	fprintf(stdout, "\nEnter a message to send (or ^Z + Enter to quit):\n");
#endif
    while (!feof(stdin) && running) {
        fprintf(stdout, "> "); fflush(stdout);
        memset(&message, 0, sizeof(message));
        string[0] = '\0';
        // read a string from stdin
        if (fgets(string, sizeof(string), stdin) == NULL) {
            break;
        }
        if (string[0] == '\n') {
            continue;
        }
        // parse message from string
        if (!CCanMessage::Parse(string, message, count, cycle, incr)) {
            fprintf(stderr, "! Sorry, you entered an invalid message (syntax error)\n");
            continue;
        }
        // send message one or more times
        for (uint32_t i = 0; i < count; i++) {
            // t0 timestamp - start of journey
            t0 = CTimer::GetTime();
            // fake CAN timestamp
            message.timestamp.tv_sec = t0.tv_sec;
            message.timestamp.tv_nsec = t0.tv_nsec;
            // send message to all clients
            retVal = ipcServer.Send(message, CANTCP_STAT_RESET);
            // abort on error
            if (retVal != CCanApi::NoError) {
                fprintf(stderr, "! Sorry, the message could not be sent (error=%i)\n", retVal);
                break;
            }
            // t1 timestamp - end of journey
            dt = CTimer::DiffTimeInUsec(t0, CTimer::GetTime());
            // delay if t1-t0 < cycle time
            if ((0 < cycle) && (dt < cycle)) {
                CTimer::Delay(cycle - dt);
            }
            // increment or decrement data
            data = (uint32_t)message.data[0]
                 | ((uint32_t)message.data[1] << 8)
                 | ((uint32_t)message.data[2] << 16)
                 | ((uint32_t)message.data[3] << 24);
            if (incr > 0) {
                data++;
            } else if (incr < 0) {
                data--;
            } else {
                data = 0;
            }
            // up- or down-counting number
            message.data[0] = (uint8_t)(data & 0xFF);
            message.data[1] = (uint8_t)((data >> 8) & 0xFF);
            message.data[2] = (uint8_t)((data >> 16) & 0xFF);
            message.data[3] = (uint8_t)((data >> 24) & 0xFF);
        }
    }
    fprintf(stdout, "\n");
    return 0;
}

/*  Signal handler to catch Ctrl+C:
 *  - signo: signal number (SIGINT, SIGHUP, SIGTERM)
 */
static void sigterm(int signo)
{
    //fprintf(stderr, "%s: got signal %d\n", __FILE__, signo);
    (void)canDevice.SignalChannel();
    running = 0;
    (void)signo;
}
