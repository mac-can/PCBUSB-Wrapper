//
//  ipc_send.c
//  RocketCAN CC
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif
#include "tcp_can.h"
#include "tcp_client.h"
#include "crc_j1850.h"

static void sigterm(int signo);
static volatile int running = 1;

static const char *server = "localhost:60000";

int main(int argc, char *argv[]) {
    int fildes = (-1);
    can_tcp_message_t msg;
    struct timespec now;
    uint32_t i, frames = 2048U;

    if (argc > 1) {
        frames = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    if ((signal(SIGINT, sigterm) == SIG_ERR) ||
#if !defined(_WIN32) && !defined(_WIN64)
        (signal(SIGHUP, sigterm) == SIG_ERR) ||
#endif
        (signal(SIGTERM, sigterm) == SIG_ERR)) {
          perror("+++ error");
          return errno;
    }
    if ((fildes = tcp_client_connect(server)) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connected to server %s\n", server);
    
    printf("Press ^C to abort.\n");
    for (i = 0; (i < frames) && running; i++) {
        memset(&msg, 0, sizeof(msg));
        msg.id = i & 0x7FFU;
        msg.flags = 0;
        msg.length = 8;
        msg.data[0] = (uint8_t)(i & 0xFF);
        msg.data[1] = (uint8_t)((i >> 8) & 0xFF);
        msg.data[2] = (uint8_t)((i >> 16) & 0xFF);
        msg.data[3] = (uint8_t)((i >> 24) & 0xFF);
        msg.ctrlchar = CANTCP_ETX_CHAR;
        if (clock_gettime(CLOCK_REALTIME, &now) == 0) {
            msg.timestamp.tv_sec = now.tv_sec;
            msg.timestamp.tv_nsec = now.tv_nsec;
        }
        CANTCP_MSG_HTON(msg);
        msg.checksum = crc_j1850_calc(&msg, sizeof(msg) - 1U, NULL);
        if (tcp_client_send(fildes, (const char*)&msg, sizeof(msg)) < 0) {
            perror("+++ error");
            break;
        }
    }
    if (tcp_client_close(fildes) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connection closed: %u frames sent\n", i);
    return EXIT_SUCCESS;
}

static void sigterm(int signo) {
    running = 0;
    (void)signo;
}
