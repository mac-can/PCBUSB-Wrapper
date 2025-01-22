/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'ipc_server' - Inter-Process Communication (IPC) server
 *
 *  This module is dual-licensed under the BSD 2-Clause "Simplified" License
 *  and under the GNU General Public License v2.0 (or any later version).
 *  You can choose between one of them if you use this module.
 *
 *  (1) BSD 2-Clause "Simplified" License
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  THIS MODULE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS MODULE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  (2) GNU General Public License v2.0 or later
 *
 *  This module is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This module is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this module; if not, see <https://www.gnu.org/licenses/>.
 */
/** @file        ipc_server.c
 *
 *  @brief       Inter-Process Communication (IPC) server.
 *
 *  @author      $Author: makemake $
 *
 *  @version     $Rev: 844 $
 *
 *  @addtogroup  ipc
 *  @{
 */
#include "ipc_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
#endif

/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */
#define BACKLOG  10  /* how many pending connections queue will hold */
#define MTU_SIZE  1500  /* maximum transmission unit (MTU) size */

#define ENTER_CRITICAL_SECTION(srv)     assert(0 == pthread_mutex_lock(&(srv)->mutex))
#define LEAVE_CRITICAL_SECTION(srv)     assert(0 == pthread_mutex_unlock(&(srv)->mutex))
#define DESTROY_CRITICAL_SECTION(srv)   assert(0 == pthread_mutex_destroy(&(srv)->mutex))
#define DESTROY_SERVER_DESCRIPTOR(srv)  free(srv)

#define LOG_INFO(srv, fmt, ...)     do { if(srv->log_opt >= IPC_LOGGER_INFO) log_info(srv->log_fp, fmt, ##__VA_ARGS__); } while(0)
#define LOG_RECV(srv, fmt, ...)     do { if(srv->log_opt >= IPC_LOGGER_DATA) log_info(srv->log_fp, fmt, ##__VA_ARGS__); } while(0)
#define LOG_SENT(srv, fmt, ...)     do { if(srv->log_opt >= IPC_LOGGER_DATA) log_info(srv->log_fp, fmt, ##__VA_ARGS__); } while(0)
#define LOG_DATA(srv, data, size)   do { if(srv->log_opt >= IPC_LOGGER_ALL) log_data(srv->log_fp, data, size); } while(0)
#define LOG_ERROR(srv, fmt, ...)    do { if(srv->log_fp) fprintf(srv->log_fp, "!!! error: " fmt "\n", ##__VA_ARGS__); } while(0)


/*  -----------  types  --------------------------------------------------
 */
struct ipc_server_desc {                /* IPC server descriptor: */
    int sock_fd;                        /* - socket file descriptor */
    size_t mtu_size;                    /* - maximum transmission unit (MTU) size */
    ipc_server_recv_cbk_t recv_cbk;     /* - receive callback */
    pthread_t thread;                   /* - thread for listening */
    pthread_mutex_t mutex;              /* - mutex for mutual exclusion */
    fd_set master;                      /* - master file descriptor list */
    int fdmax;                          /* - maximum file descriptor number */
    FILE *log_fp;                       /* - log file */
    unsigned char log_opt;              /* - logging option */
};

/*  -----------  prototypes  ---------------------------------------------
 */
static void *listening(void *arg);
static void *get_in_addr(struct sockaddr *sa);

static void log_info(FILE *fp, const char *fmt, ...);
static void log_data(FILE *fp, const void *data, size_t size);


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  functions  ----------------------------------------------
 */
ipc_server_t ipc_server_start(unsigned short port, size_t mtu_size,
                              ipc_server_recv_cbk_t recv_cbk,
                              unsigned char logging) {
    struct ipc_server_desc *server = NULL;
    struct sockaddr_in address;
    int opt = 1;
    int error = errno = 0;

    /* create the server descriptor */
    if ((server = (struct ipc_server_desc *)malloc(sizeof(struct ipc_server_desc))) == NULL) {
        /* errno set */
        return NULL;
    }    
    /* open the log file for writing */
    if (logging) {
        if ((server->log_fp = fopen("ipc_server.log", "w")) == NULL) {
            /* errno set */
            return NULL;
        }
        fprintf(server->log_fp, "+++ IPC Server at port %d with mtu size %zu +++\n", port, mtu_size);
        server->log_opt = logging;
    }
    /* create the socket file descriptor */
    if ((server->sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error = errno;
        LOG_ERROR(server, "Socket could not be created (errno=%d)", error);
        free(server);
        errno = error;
        return NULL;
    }
    /* forcefully attaching socket to the port */
    if (setsockopt(server->sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        error = errno;
        LOG_ERROR(server, "Socket could not be attached to port %d (errno=%d)\n", port, error);
        close(server->sock_fd);
        free(server);
        errno = error;
        return NULL;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    /* bind the socket to the network address and port */
    if (bind(server->sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error = errno;
        LOG_ERROR(server, "Socket could not be bound to port %d (errno=%d)", port, error);
        close(server->sock_fd);
        free(server);
        errno = error;
        return NULL;
    }
    /* start listening for incoming connections */
    if (listen(server->sock_fd, BACKLOG) < 0) {
        error = errno;
        LOG_ERROR(server, "Start listening on port %d failed (errno=%d)", port, error);
        close(server->sock_fd);
        free(server);
        errno = error;
        return NULL;
    }
    /* create a mutex for mutual exclusion */
    if (pthread_mutex_init(&server->mutex, NULL) != 0) {
        error = errno;
        LOG_ERROR(server, "The mutex could not be created (errno=%d)", error);
        close(server->sock_fd);
        free(server);
        errno = error;
        return NULL;
    }
    /* set MTU size (but at most 1500) and receive callback */
    server->mtu_size = (mtu_size < MTU_SIZE) ? mtu_size : MTU_SIZE;
    server->recv_cbk = recv_cbk;
    /* add the listener to the master set */
    FD_ZERO(&server->master);
    FD_SET(server->sock_fd, &server->master);
    /* keep track of the biggest file descriptor */
    server->fdmax = server->sock_fd;
    /* create a new thread for listening */
    if (pthread_create(&server->thread, NULL, listening, (void *)server) != 0) {
        error = errno;
        LOG_ERROR(server, "Server could not be started (errno=%d)", error);
        DESTROY_CRITICAL_SECTION(server);
        close(server->sock_fd);
        free(server);
        errno = error;
        return NULL;
    }
    /* return the server descriptor */
    return (ipc_server_t)server;
}

int ipc_server_stop(ipc_server_t server) {
    int fildes = server ? ((struct ipc_server_desc *)server)->sock_fd : (-1);

    /* the server must be running */
    if (server == NULL) {
        errno = ESRCH;
        return (-1);
    }
    /* terminate the listening thread */
    if (pthread_cancel(((struct ipc_server_desc *)server)->thread) != 0) {
        /* errno set */
        LOG_ERROR(server, "Server could not be stopped (errno=%d)", errno);
        return (-1);
    }
    /* wait for the listening thread to terminate */
    LOG_INFO(server, "Server stopped on socket %d\n", fildes);
    errno = 0;
    /* close the log file */
    if (server->log_fp != NULL) {
        server->log_opt = IPC_LOGGER_NONE;
        fclose(server->log_fp);
        server->log_fp = NULL;
    }
    /* destroy the IPC server descriptor */
    DESTROY_CRITICAL_SECTION((struct ipc_server_desc *)server);
    DESTROY_SERVER_DESCRIPTOR((struct ipc_server_desc *)server);
    /* close the socket */
    errno = 0;
#if !defined(_WIN32) && !defined(_WIN64)
    return close(fildes);
#else
    return closesocket(fildes);
#endif
}

int ipc_server_send(ipc_server_t server, const void *data, size_t size) {
    fd_set write_fds;    /* file descriptor list for send() */
    int fdmax = 0;       /* maximum file descriptor number */
    ssize_t nbytes = 0;  /* number of bytes sent */
    int i;

    /* the server must be running */
    if (server == NULL) {
        errno = ESRCH;
        return (-1);
    }
    /* check for NULL pointer */
    if (data == NULL) {
        errno = EINVAL;
        return (-1);
    }
    /* get the master set and the maximum file descriptor number */
    ENTER_CRITICAL_SECTION((struct ipc_server_desc *)server);
    write_fds = ((struct ipc_server_desc *)server)->master;
    fdmax = ((struct ipc_server_desc *)server)->fdmax;
    LEAVE_CRITICAL_SECTION((struct ipc_server_desc *)server);

    /* remove the listener (we don't want to hear our own crap) */
    FD_CLR(((struct ipc_server_desc *)server)->sock_fd, &write_fds);
    /* send data to all clients */
    LOG_DATA(server, data, size);
    for(i = 0; i <= fdmax; i++) {
        if (FD_ISSET(i, &write_fds)) {
            if ((nbytes = send(i, data, size, 0)) < 0) {
                LOG_ERROR(server, "Send failed on socket %d (errno=%d)", i, errno);
                continue;   // FIXME: how to handle this?
            }
            LOG_SENT(server, "Sent %ld bytes to socket %d\n", nbytes, i);
        }
    }
    errno = 0;
    return 0;
}

/*  -----------  local functions  ----------------------------------------
 */
static void *listening(void *arg) {
    struct ipc_server_desc *server = (struct ipc_server_desc *)arg;

    fd_set read_fds;  /* file descriptor list for select() */
    FD_ZERO(&read_fds);

    int newfd;        /* newly accept()ed socket descriptor */
    struct sockaddr_storage remoteaddr; /* client address */
    socklen_t addrlen;

    char buf[MTU_SIZE];  /* buffer for client data */
    ssize_t nbytes = 0;
    int i;

    /* terminate immediately if the server descriptor is invalid */
    if (server == NULL) {
        return NULL;
    }
    /* set the thread cancelation state and type */
    assert(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) == 0);
    assert(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) == 0);
    /* log the server start */
    LOG_INFO(server, "Server started on socket %d\n", server->sock_fd);
    /* "The torture never stops" */
    for(;;) {
        /* blocking read (the thread is suspended until data arrives) */
        read_fds = server->master;  /* use a copy of the master set */
        if (select(server->fdmax+1, &read_fds, NULL, NULL, NULL) < 0) {
            LOG_ERROR(server, "%s (errno=%d)", strerror(errno), errno);
            continue;   // FIXME: how to handle this?
        }
        /* loop through the existing connections looking for data to read */
        for(i = 0; i <= server->fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server->sock_fd) {
                    /* handle new connections */
                    addrlen = sizeof(remoteaddr);
                    if ((newfd = accept(server->sock_fd, (struct sockaddr *)&remoteaddr, &addrlen)) >= 0) {
                        ENTER_CRITICAL_SECTION(server);
                        /* add the new socket to the master set */
                        FD_SET(newfd, &server->master);
                        /* keep track of the biggest file descriptor */
                        if (newfd > server->fdmax) {
                            server->fdmax = newfd;
                        }
                        LEAVE_CRITICAL_SECTION(server);
                        /* log the new connection */
                        char remoteIP[INET6_ADDRSTRLEN];
                        LOG_INFO(server, "New connection from %s on socket %d\n",
                            inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN), newfd);
                    } else {
                        LOG_ERROR(server, "%s (errno=%d)", strerror(errno), errno);
                        continue;   // FIXME: how to handle this?
                    }
                } else {
                    /* handle data from a client */
                    if ((nbytes = recv(i, buf, server->mtu_size, 0)) > 0) {
                        LOG_RECV(server, "Received %ld bytes from socket %d\n", nbytes, i);
                        LOG_DATA(server, buf, nbytes);
                        /* notify the server application */
                        if (server->recv_cbk != NULL) {
                            server->recv_cbk(buf, nbytes);
                        }
                    } else {
                        /* connection closed by client or an error occurred */
                        if (nbytes == 0) {
                            /* socket hung up */
                            LOG_INFO(server, "Socket %d hung up\n", i);
                        } else {
                            if (errno == ECONNRESET) {
                                /* connection reset by peer */
                                LOG_INFO(server, "Connection reset by peer on socket %d\n", i);
                            } else {
                                /* error occurred */
                                LOG_ERROR(server, "%s (errno=%d)", strerror(errno), errno);
                            }
                        }
                        /* close the socket and remove from the master set */
                        ENTER_CRITICAL_SECTION(server);
                        close(i);
                        FD_CLR(i, &server->master);
                        LEAVE_CRITICAL_SECTION(server);
                    }
                } // END if (i == listener)
            } // END if (FD_ISSET(i, &read_fds))
        } // END for(i = 0; i <= fdmax; i++)
    } // END for(;;)
    return NULL;
}

static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        /* return the IPv4 address */
        return &(((struct sockaddr_in*)sa)->sin_addr);
    } else {
        /* return the IPv6 address */
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

static void log_info(FILE *fp, const char *fmt, ...) {
    va_list args;
    time_t now = time(NULL);
    char *str = ctime(&now);
    str[strlen(str)-1] = '\0';

    if (fp == NULL) {
        return;
    }
    fprintf(fp, "[%s] ", str);
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);
}

static void log_data(FILE *fp, const void *data, size_t size) {
    time_t now = time(NULL);
    char *str = ctime(&now);
    str[strlen(str)-1] = '\0';

    if (fp == NULL) {
        return;
    }
    fprintf(fp, "[%s] %zu Byte(s):", str, size);
    for (size_t i = 0; i < size; i++) {
        fprintf(fp, " %02X", ((unsigned char *)data)[i]);
    }
    fprintf(fp, "\n");
}


/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
