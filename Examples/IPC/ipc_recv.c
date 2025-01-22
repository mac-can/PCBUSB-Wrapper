//
//  ipc_recv.c
//  RocketCAN
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "ipc_can.h"
#include "ipc_client.h"

static void sigterm(int signo);
static volatile int running = 1;

static const char *server = "127.0.0.1:60000";

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
    if ((fildes = ipc_client_connect(server)) < 0) {
        perror("+++ error");
        return EXIT_FAILURE;
    }
    printf("Connected to server %s\n", server);
    
    while (running) {
        memset(&msg, 0, sizeof(msg));
        if (recv(fildes, (void*)&msg, sizeof(msg), 0) < 0) {
            perror("+++ error");
            continue;
        }
        CAN_IPC_MSG_NTOH(msg);
        printf("<<< %03x [%u]", msg.id, msg.length);
        for (uint8_t i = 0; i < msg.length; i++) {
            printf(" %02x", msg.data[i]);
        }
        printf("\n");
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
