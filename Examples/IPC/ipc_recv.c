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
    int frames = 0;

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
    
    printf("Press Ctrl+C to abort...\n");
    while (running) {
        memset(&msg, 0, sizeof(msg));
        if (ipc_client_recv(fildes, (void*)&msg, sizeof(msg), IPC_WAIT_FOREVER) < 0) {
            if (errno != ENODATA)
                perror("+++ error");
            continue;
        }
        CAN_IPC_MSG_NTOH(msg);
        printf("))) %i\t", frames++);
        printf("%7li.%04li\t", (long)msg.timestamp.tv_sec, msg.timestamp.tv_nsec / 100000);
        printf("%03X\t", msg.id);
        if (!(msg.flags & CANIPC_STS_MASK)) {
            putchar((msg.flags & CANIPC_XTD_MASK) ? 'X' : 'S');
            putchar((msg.flags & CANIPC_FDF_MASK) ? 'F' : '-');
            putchar((msg.flags & CANIPC_BRS_MASK) ? 'B' : '-');
            putchar((msg.flags & CANIPC_ESI_MASK) ? 'E' : '-');
            putchar((msg.flags & CANIPC_RTR_MASK) ? 'R' : '-');
        } else {
            printf("Error");
        }
        printf(" [%u]", msg.length);
        for (uint8_t i = 0; i < msg.length; i++) {
            printf(" %02X", msg.data[i]);
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
