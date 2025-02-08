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
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 1433 $
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
#include <time.h>
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
/* Need to link with Ws2_32.lib */
#pragma comment(lib, "ws2_32.lib")
#endif

/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */
#define BACKLOG  10  /* how many pending connections queue will hold */
#define MTU_SIZE  1500  /* maximum transmission unit (MTU) size */

#define ENTER_CRITICAL_SECTION(srv)     assert(0 == pthread_mutex_lock(&((struct ipc_server_desc*)srv)->mutex))
#define LEAVE_CRITICAL_SECTION(srv)     assert(0 == pthread_mutex_unlock(&((struct ipc_server_desc*)srv)->mutex))

#define DESTROY_MUTEX(srv)  assert(0 == pthread_mutex_destroy(&((struct ipc_server_desc*)srv)->mutex))
#define CLOSE_SOCKET(srv)   assert(0 == close(((struct ipc_server_desc*)srv)->sock_fd))
#define FREE_SERVER(srv)    do { if (srv) { free(srv); srv = NULL; } } while(0)

#define LOG_INFO(srv, fmt, ...)     do { if (srv->log_opt >= IPC_LOGGING_INFO) log_info(srv->log_fp, fmt, ##__VA_ARGS__); } while(0)
#define LOG_RECV(srv, fmt, ...)     do { if (srv->log_opt >= IPC_LOGGING_DATA) log_info(srv->log_fp, fmt, ##__VA_ARGS__); } while(0)
#define LOG_SENT(srv, fmt, ...)     do { if (srv->log_opt >= IPC_LOGGING_DATA) log_info(srv->log_fp, fmt, ##__VA_ARGS__); } while(0)
#define LOG_DATA(srv, dir, buf, n)  do { if (srv->log_opt >= IPC_LOGGING_ALL) log_data(srv->log_fp, dir, buf, n); } while(0)
#define LOG_ERROR(srv, fmt, ...)    do { if (srv->log_opt) fprintf(srv->log_fp, "!!! error: " fmt "\n", ##__VA_ARGS__); } while(0)
#define LOG_CLOSE(srv)              do { if (srv->log_opt) fclose(srv->log_fp); } while(0)

#define LOG_DIR_RECV  0
#define LOG_DIR_SENT  1


/*  -----------  types  --------------------------------------------------
 */
struct ipc_server_desc {                /* IPC server descriptor: */
    int sock_fd;                        /* - socket file descriptor */
    int sock_type;                      /* - socket type */
    int sock_domain;                    /* - socket domain */
    int sock_protocol;                  /* - socket protocol */
    size_t mtu_size;                    /* - maximum transmission unit (MTU) size */
    ipc_event_cbk_t recv_cbk;           /* - receive callback */
    void *recv_ref;                     /* - receive reference */
    pthread_t thread;                   /* - thread for listening */
    pthread_mutex_t mutex;              /* - mutex for mutual exclusion */
    fd_set master;                      /* - master file descriptor list */
    int fdmax;                          /* - maximum file descriptor number */
    FILE *log_fp;                       /* - log file */
    unsigned char log_opt;              /* - logging option */
    struct timespec start;              /* - server start time */
    unsigned long sent_pkg;             /* - number of sent packets */
    unsigned long recv_pkg;             /* - number of received packets */
    unsigned long lost_pkg;             /* - number of unprocessed packets */
    unsigned short sock_port;           /* - socket port (just for logging) */
};

/*  -----------  prototypes  ---------------------------------------------
 */
static void *listening(void *arg);
static void *get_in_addr(struct sockaddr *sa);

static void log_info(FILE *fp, const char *fmt, ...);
static void log_data(FILE *fp, int dir, const void *data, size_t size);

static struct timespec time_get(void);
static double time_diff(struct timespec start, struct timespec stop);

/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  functions  ----------------------------------------------
 */

/*  Start the server on the specified port.
 *
 *  List of called functions:
 *  - malloc() — allocate memory (errno = ENOMEM)
 *  - socket() — create an endpoint for communication (errno = EACCES, EAFNOSUPPORT, EMFILE, ENFILE, ENOBUFS, ENOMEM, EPROTONOSUPPORT)
 *  - setsockopt() — set options on sockets (errno = EBADF, EFAULT, EINVAL, ENOPROTOOPT, ENOTSOCK, EOPNOTSUPP)
 *  - bind() — bind a name to a socket (errno = EACCES, EADDRINUSE, EBADF, EINVAL, ENOTSOCK, EADDRNOTAVAIL, EFAULT)
 *  - listen() — listen for connections on a socket (errno = EADDRINUSE, EBADF, ENOTSOCK, EOPNOTSUPP)
 *  - FD_ZERO() — clear a file descriptor set (w/o error handling)
 *  - FD_SET() — add a file descriptor to a set (w/o error handling)
 *  - pthread_mutex_init() — initialize a mutex (errno = EAGAIN, ENOMEM)
 *  - pthread_create() — create a new thread (errno = EAGAIN, EINVAL, EPERM)
 *  - close() — close a file descriptor (errno = EBADF)
 *  - free() — deallocate memory (errno = EINVAL)
 */
ipc_server_t ipc_server_start(unsigned short port, int sock_type, size_t mtu_size,
                              ipc_event_cbk_t recv_cbk, void *recv_ref, int logging) {
    struct ipc_server_desc *server = NULL;
    struct sockaddr_in address;
    char filename[32];
    int opt = 1;
    int error;
    errno = 0;
#if (1)
    // only stream sockets are supported yet!
    if (sock_type != IPC_SOCK_TCP) {
        errno = EINVAL;
        return NULL;
    }
#else
    // TODO: implement UDP, SCTP and raw socket
    // TODO: map the following attributes to corresponding values:
    //       - sock_type: SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW
    //       - sock_domain: AF_INET, AF_INET6, AF_UNIX, AF_PACKET
    //       - sock_protocol: IPPROTO_IP, IPPROTO_TCP, IPPROTO_UDP, IPPROTO_SCTP
    // TODO: what's about Nagle's algorithm, and other stuff like this?
#endif
    /* create the server descriptor */
    if ((server = (struct ipc_server_desc *)malloc(sizeof(struct ipc_server_desc))) == NULL) {
        /* errno set */
        return NULL;
    }
    /* initialize the server descriptor */
    memset(server, 0, sizeof(struct ipc_server_desc));
    server->sock_fd = (-1);
    server->sock_port = port;
    server->sock_type = SOCK_STREAM;
    server->sock_domain = AF_INET;
    server->sock_protocol = IPPROTO_IP;
    /* set MTU size (but at most 1500) and receive callback */
    server->mtu_size = (mtu_size < MTU_SIZE) ? mtu_size : MTU_SIZE;
    server->recv_cbk = recv_cbk;
    server->recv_ref = recv_ref;
    server->sent_pkg = 0;
    server->recv_pkg = 0;
    server->lost_pkg = 0;

    /* open the log file for writing ("ipc_<port>.log") */
    if (logging) {
        snprintf(filename, sizeof(filename), "ipc_%d.log", port);
        if ((server->log_fp = fopen(filename, "w")) == NULL) {
            error = errno;
            FREE_SERVER(server);
            errno = error;
            return NULL;
        }
        fprintf(server->log_fp, "+++ IPC Server on port %d using %s with mtu size %zu +++\n", port,
            (sock_type == IPC_SOCK_TCP) ? "TCP" : ((sock_type == IPC_SOCK_UDP) ? "UDP" :
            ((sock_type == IPC_SOCK_SCTP) ? "SCTP" : "IP (raw socket)")), mtu_size);
        server->log_opt = logging;
        server->start = time_get();
    }
    /* create the socket file descriptor */
    if ((server->sock_fd = socket(server->sock_domain, server->sock_type, server->sock_protocol)) < 0) {
        error = errno;
        LOG_ERROR(server, "Socket could not be created (errno=%d)", error);
        LOG_CLOSE(server);
        FREE_SERVER(server);
        errno = error;
        return NULL;
    }
    /* forcefully attaching socket to the port */
    if (setsockopt(server->sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        error = errno;
        LOG_ERROR(server, "Socket could not be attached to port %d (errno=%d)\n", port, error);
        LOG_CLOSE(server);
        CLOSE_SOCKET(server);
        FREE_SERVER(server);
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
        LOG_CLOSE(server);
        CLOSE_SOCKET(server);
        FREE_SERVER(server);
        errno = error;
        return NULL;
    }
    /* start listening for incoming connections */
    if (listen(server->sock_fd, BACKLOG) < 0) {
        error = errno;
        LOG_ERROR(server, "Start listening on port %d failed (errno=%d)", port, error);
        LOG_CLOSE(server);
        CLOSE_SOCKET(server);
        FREE_SERVER(server);
        errno = error;
        return NULL;
    }
    /* create a mutex for mutual exclusion */
    if (pthread_mutex_init(&server->mutex, NULL) != 0) {
        error = errno;
        LOG_ERROR(server, "The mutex could not be created (errno=%d)", error);
        LOG_CLOSE(server);
        CLOSE_SOCKET(server);
        FREE_SERVER(server);
        errno = error;
        return NULL;
    }
    /* add the listener to the master set */
    FD_ZERO(&server->master);
    FD_SET(server->sock_fd, &server->master);
    /* keep track of the biggest file descriptor */
    server->fdmax = server->sock_fd;
    /* create a new thread for listening */
    if (pthread_create(&server->thread, NULL, listening, (void *)server) != 0) {
        error = errno;
        LOG_ERROR(server, "Server could not be started (errno=%d)", error);
        LOG_CLOSE(server);
        DESTROY_MUTEX(server);
        CLOSE_SOCKET(server);
        FREE_SERVER(server);
        errno = error;
        return NULL;
    }
    /* return the server descriptor */
    return (ipc_server_t)server;
}

/*  Stop the server.
 *
 *  List of called functions:
 *  - pthread_cancel() — send a cancellation request to a thread (errno = EINVAL, ESRCH)
 *  - pthread_mutex_destroy() — destroy a mutex (errno = EINVAL)
 *  - close() — close a file descriptor (errno = EBADF)
 *  - free() — deallocate memory (errno = EINVAL)
 *  + NULL pointer dereference (errno = ESRCH)
 */
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
        LOG_CLOSE(server);
        return (-1);
    }
    /* wait for the listening thread to terminate */
    LOG_INFO(server, "Server stopped on socket %d\n", fildes);
    errno = 0;
    /* close the log file */
    if (server->log_fp != NULL) {
        fprintf(server->log_fp, "+++ Connection Summary for IPC Server on port %u +++\n", server->sock_port);
        fprintf(server->log_fp, "%11lu packet(s) sent to clients\n", server->sent_pkg);
        fprintf(server->log_fp, "%11lu packet(s) received from clients\n", server->recv_pkg);
        fprintf(server->log_fp, "%11lu packet(s) not processed by the host\n", server->lost_pkg);
        fprintf(server->log_fp, "+++ IPC Server terminated: elapsed time %.4f sec +++\n",
            time_diff(server->start, time_get()));
        fclose(server->log_fp);
        server->log_fp = NULL;
        server->log_opt = 0;
    }
    /* destroy the IPC server descriptor */
    DESTROY_MUTEX(server);
    FREE_SERVER(server);
    /* close the socket */
    errno = 0;
#if !defined(_WIN32) && !defined(_WIN64)
    return close(fildes);
#else
    return closesocket(fildes);
#endif
}

/*  Send data to the client.
 *
 *  List of called functions:
 *  - pthread_mutex_lock() — lock a mutex (w/o error handling)
 *  - pthread_mutex_unlock() — unlock a mutex (w/o error handling)
 *  - FD_CLR() — clear a file descriptor from a set (w/o error handling)
 *  - send() — send a message on a socket (errno = ECONNRESET, EPIPE)
 *  + NULL pointer dereference (errno = ESRCH, EINVAL)
 */
int ipc_server_send(ipc_server_t server, const void *data, size_t size) {
    fd_set write_fds;    /* file descriptor list for send() */
    int fdmax = 0;       /* maximum file descriptor number */
    ssize_t nbytes = 0;  /* number of bytes sent */
    int i, n = 0;

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
    LOG_DATA(server, LOG_DIR_SENT, data, size);
    for (i = 0; i <= fdmax; i++) {
        if (FD_ISSET(i, &write_fds)) {
            if ((nbytes = send(i, data, size, 0)) < 0) {
                LOG_ERROR(server, "Send failed on socket %d (errno=%d)", i, errno);
                continue;   // FIXME: how to handle this?
            }
            n++;
        }
    }
    if (n) {
        LOG_SENT(server, "Sent %ld bytes to %d client(s)\n", nbytes, n);
        ((struct ipc_server_desc *)server)->sent_pkg++;
    } else {
        LOG_SENT(server, "Lost %lu bytes (no client connected)\n", size);
    }
    errno = 0;
    return 0;
}

/*  -----------  local functions  ----------------------------------------
 */

/*  Listening thread.
 *
 *  List of called functions:
 *  - pthread_setcancelstate() — set cancelability state
 *  - pthread_setcanceltype() — set cancelability type
 *  - select() — synchronous I/O multiplexing
 *  - accept() — accept a new connection on a socket
 *  - recv() — receive a message from a socket
 *  - close() — close a file descriptor
 *  - pthread_mutex_lock() — lock a mutex
 *  - pthread_mutex_unlock() — unlock a mutex
 *  - FD_ZERO() — clear a file descriptor set
 *  - FD_SET() — add a file descriptor to a set
 *  - FD_ISSET() — test a file descriptor in a set
 *  - FD_CLR() — clear a file descriptor from a set
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
    int i, rc = 0;

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
    for (;;) {
        /* blocking read (the thread is suspended until data arrives) */
        read_fds = server->master;  /* use a copy of the master set */
        if (select(server->fdmax+1, &read_fds, NULL, NULL, NULL) < 0) {
            LOG_ERROR(server, "%s (errno=%d)", strerror(errno), errno);
            continue;   // FIXME: how to handle this?
        }
        /* loop through the existing connections looking for data to read */
        for (i = 0; i <= server->fdmax; i++) {
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
                        LOG_DATA(server, LOG_DIR_RECV, buf, nbytes);
                        server->recv_pkg++;
                        /* notify the server application */
                        if (server->recv_cbk != NULL) {
                            if ((rc = server->recv_cbk(buf, nbytes, server->recv_ref)) < 0) {
                                LOG_ERROR(server, "Receive callback failed on socket %d (error=%d)", i, rc);
                                server->lost_pkg++;
                            }
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
                } /* if (i == listener) */
            } /* if (FD_ISSET(i, &read_fds)) */
        } /* for (i = 0; i <= fdmax; i++) */
    } /* for (;;) */
    return NULL;
}

/* Get sockaddr, IPv4 or IPv6.
 */
static void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        /* return the IPv4 address */
        return &(((struct sockaddr_in*)sa)->sin_addr);
    } else {
        /* return the IPv6 address */
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

/* Log information.
 */
static void log_info(FILE *fp, const char *fmt, ...) {
    va_list args;
    char str[32] = "";
    struct timespec now = time_get();

    if (fp == NULL) {
        return;
    }
    if (ctime_r(&now.tv_sec, str) != NULL) {
        str[strlen(str)-1] = '\0';
    } else {
        str[0] = '\0';
    }
    fprintf(fp, "[%s%12ld.%04ld] ", str, now.tv_sec, (now.tv_nsec + 50000) / 100000);
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);
    fflush(fp);
}

/* Log data as binary.
 */
static void log_data(FILE *fp, int dir, const void *data, size_t size) {
    char str[32] = "";
    struct timespec now = time_get();

    if (fp == NULL) {
        return;
    }
    if (ctime_r(&now.tv_sec, str) != NULL) {
        str[strlen(str)-1] = '\0';
    } else {
        str[0] = '\0';
    }
    fprintf(fp, "[%s%12ld.%04ld] ", str, now.tv_sec, (now.tv_nsec + 50000) / 100000);
    fprintf(fp, "%s %zu byte(s):", (dir == LOG_DIR_RECV) ? "Received" : "Sending", size);
    for (size_t i = 0; i < size; i++) {
        fprintf(fp, " %02X", ((unsigned char *)data)[i]);
    }
    fprintf(fp, "\n");
    fflush(fp);
}

/* Get the current time.
 */
static struct timespec time_get(void) {
    struct timespec now = { 0, 0 };
#if !defined(_WIN32) && !defined(_WIN64)
    if (clock_gettime(CLOCK_REALTIME, &now) != 0) {
        now.tv_sec = 0;
        now.tv_nsec = 0;
    }
#else
    // TODO: insert coin here
#endif
    return now;
}

/* Time difference in seconds.
 */
static double time_diff(struct timespec start, struct timespec stop) {
    return (((double)stop.tv_sec  + ((double)stop.tv_nsec  / 1000000000.f)) -
            ((double)start.tv_sec + ((double)start.tv_nsec / 1000000000.f)));
}

/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
