//
//  ipc_send.c
//  RocketCAN
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif
#include "ipc_can.h"
#include "ipc_client.h"

static void sigterm(int signo);
static volatile int running = 1;

static const char *server = "localhost:60000";

int main() {
    int fildes = (-1);
    can_ipc_message_t msg;

    if ((signal(SIGINT, sigterm) == SIG_ERR) ||
#if !defined(_WIN32) && !defined(_WIN64)
        (signal(SIGHUP, sigterm) == SIG_ERR) ||
#endif
        (signal(SIGTERM, sigterm) == SIG_ERR)) {
          perror("+++ error");
          return errno;
    }
    if ((fildes = ipc_client_connect(server, IPC_SOCK_TCP)) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connected to server %s\n", server);
    
    for (uint32_t i = 0; i < 2048; i++) {
        memset(&msg, 0, sizeof(msg));
        msg.id = i;
        msg.flags = 0;
        msg.length = 8;
        msg.data[0] = (uint8_t)(i & 0xFF);
        msg.data[1] = (uint8_t)((i >> 8) & 0xFF);
        msg.data[2] = (uint8_t)((i >> 16) & 0xFF);
        msg.data[3] = (uint8_t)((i >> 24) & 0xFF);
        CAN_IPC_MSG_HTON(msg);
        if (ipc_client_send(fildes, (const char*)&msg, sizeof(msg)) < 0) {
            perror("+++ error");
            break;
        }
    }
    if (ipc_client_close(fildes) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connection closed\n");
    return EXIT_SUCCESS;
}

static void sigterm(int signo) {
    running = 0;
    (void)signo;
}
