/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'tcp_server' - Stream Socket Server (TCP/IP)
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
/** @file        tcp_server.c
 *
 *  @brief       Stream Socket Server (TCP/IP).
 *
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 1473 $
 *
 *  @addtogroup  tcp
 *  @{
 */
#include "tcp_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>


/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */
#if (OPTION_TCPIP_BACKLOG == 0)
#define BACKLOG  5  /* how many pending connections queue will hold */
#else
#define BACKLOG  OPTION_TCPIP_BACKLOG
#endif
#if (TCP_BUF_SIZE < TCP_MSS_SIZE)
#define MAX_BUF_SIZE  TCP_MSS_SIZE
#else
#define MAX_BUF_SIZE  TCP_BUF_SIZE
#endif
#define ENTER_CRITICAL_SECTION(srv)     assert(0 == pthread_mutex_lock(&((struct tcp_server_desc*)srv)->mutex))
#define LEAVE_CRITICAL_SECTION(srv)     assert(0 == pthread_mutex_unlock(&((struct tcp_server_desc*)srv)->mutex))

#define DESTROY_MUTEX(srv)  assert(0 == pthread_mutex_destroy(&((struct tcp_server_desc*)srv)->mutex))
#define CLOSE_SOCKET(srv)   assert(0 == close(((struct tcp_server_desc*)srv)->sock_fd))
#define FREE_SERVER(srv)    do { if (srv) { free(srv); srv = NULL; } } while(0)

#define LOG_INFO(srv, fmt, ...)     do { if (srv->log_opt >= TCP_LOGGING_INFO) log_info(srv->log_fp, fmt, ##__VA_ARGS__); } while(0)
#define LOG_RECV(srv, fmt, ...)     do { if (srv->log_opt >= TCP_LOGGING_DATA) log_info(srv->log_fp, fmt, ##__VA_ARGS__); } while(0)
#define LOG_SENT(srv, fmt, ...)     do { if (srv->log_opt >= TCP_LOGGING_DATA) log_info(srv->log_fp, fmt, ##__VA_ARGS__); } while(0)
#define LOG_DATA(srv, dir, buf, n)  do { if (srv->log_opt >= TCP_LOGGING_ALL) log_data(srv->log_fp, dir, buf, n); } while(0)
#define LOG_ERROR(srv, fmt, ...)    do { if (srv->log_opt) fprintf(srv->log_fp, "!!! error: " fmt "\n", ##__VA_ARGS__); } while(0)
#define LOG_CLOSE(srv)              do { if (srv->log_opt) fclose(srv->log_fp); } while(0)

#define LOG_DIR_RECV  0
#define LOG_DIR_SENT  1


/*  -----------  types  --------------------------------------------------
 */
struct tcp_server_desc {                /* TCP/IP server descriptor: */
    int sock_fd;                        /* - socket file descriptor */
    int sock_type;                      /* - socket type */
    int sock_family;                    /* - address family */
    int sock_protocol;                  /* - protocol to be used */
    size_t data_size;                   /* - data size */
    tcp_event_cbk_t recv_cbk;           /* - receive callback */
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
    char sock_port[NI_MAXSERV];         /* - socket port (just for logging) */
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
tcp_server_t tcp_server_start(const char *service, size_t data_size,
                              tcp_event_cbk_t recv_cbk, void *recv_ref, int logging) {
    struct tcp_server_desc *server = NULL;
    struct addrinfo hints, *ai, *p;
    char filename[10 + NI_MAXSERV];
    int rc, opt;
    int error;
    errno = 0;

    /* sanity ckeck */
    if ((service == NULL) || (strlen(service) == 0) || (strlen(service) >= NI_MAXSERV)) {
        errno = EINVAL;
        return NULL;
    }
    /* create the server descriptor */
    if ((server = (struct tcp_server_desc *)malloc(sizeof(struct tcp_server_desc))) == NULL) {
        /* errno set */
        return NULL;
    }
    /* initialize the server descriptor */
    memset(server, 0, sizeof(struct tcp_server_desc));
    strncpy(server->sock_port, service, NI_MAXSERV);
    server->sock_fd = (-1);
    /* set MTU size (but at most MSS size) and receive callback */
    server->data_size = (data_size < TCP_MSS_SIZE) ? data_size : TCP_MSS_SIZE;  // TODO: think about this!
    server->recv_cbk = recv_cbk;
    server->recv_ref = recv_ref;
    server->sent_pkg = 0;
    server->recv_pkg = 0;
    server->lost_pkg = 0;
    /* set options to create a socket */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;      // alow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // stream socket (TCP)
    hints.ai_flags = AI_PASSIVE;      // use my IP address
    hints.ai_protocol = IPPROTO_IP;   // any protocol
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    /* open the log file for writing ("tcp_<service>.log") */
    if (logging) {
        snprintf(filename, sizeof(filename), "tcp_%s.log", service);
        if ((server->log_fp = fopen(filename, "w")) == NULL) {
            error = errno;
            FREE_SERVER(server);
            errno = error;
            return NULL;
        }
        fprintf(server->log_fp, "+++ TCP/IP Server on port %s with data size %zu +++\n", service, data_size);
        server->log_opt = logging;
        server->start = time_get();
    }
    /* get the address information for the specified port */
    if ((rc = getaddrinfo(NULL, server->sock_port, &hints, &ai)) != 0) {
        error = errno ? errno : EADDRNOTAVAIL;
        LOG_ERROR(server, "%s\n", gai_strerror(rc));
        LOG_CLOSE(server);
        FREE_SERVER(server);
        errno = error;
        return NULL;
    }
    /* loop through all the results and bind to the first we can */
    for (p = ai; p != NULL; p = p->ai_next) {
        if ((server->sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            error = errno;
            server->sock_fd = (-1);
            continue;
        }
        /* suppress the "address already in use" error message */
        opt = 1;
        if (setsockopt(server->sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            error = errno;
            server->sock_fd = (-1);
            continue;
        }
        if (bind(server->sock_fd, p->ai_addr, p->ai_addrlen) < 0) {
            error = errno;
            close(server->sock_fd);
            server->sock_fd = (-1);
            continue;
        }
        /* Yeah, successfull bind to the socket! */
        server->sock_type = p->ai_socktype;
        server->sock_family = p->ai_family;
        server->sock_protocol = p->ai_protocol;
        break;
    }
    freeaddrinfo(ai);

    /* check if socket file dsecriptor */
    if (server->sock_fd < 0) {
        error = errno;
        LOG_ERROR(server, "Socket not created (errno=%d)", error);
        LOG_CLOSE(server);
        FREE_SERVER(server);
        errno = error;
        return NULL;
    }
    /* check if binded to the socket */
    if (p == NULL) {
        error = errno;
        LOG_ERROR(server, "Bind socket failed (errno=%d)", error);
        LOG_CLOSE(server);
        CLOSE_SOCKET(server);
        FREE_SERVER(server);
        errno = error;
        return NULL;
    }
    /* disable Nagle's algorithm for TCP connections */
#if (OPTION_TCPIP_TCPDELAY == 0) 
    opt = 1;
    if (setsockopt(server->sock_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0) {
        error = errno;
        LOG_ERROR(server, "Set TCP_NODELAY failed (errno=%d)", error);
        LOG_CLOSE(server);
        CLOSE_SOCKET(server);
        FREE_SERVER(server);
        errno = error;
        return NULL;
    }
#endif
    /* start listening for incoming connections */
    if (listen(server->sock_fd, BACKLOG) < 0) {
        error = errno;
        LOG_ERROR(server, "Start listening on port %s failed (errno=%d)", service, error);
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
    return (tcp_server_t)server;
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
int tcp_server_stop(tcp_server_t server) {
    int fildes = server ? ((struct tcp_server_desc *)server)->sock_fd : (-1);
    int i;

    /* the server must be running */
    if (server == NULL) {
        errno = ESRCH;
        return (-1);
    }
    /* terminate the listening thread */
    if (pthread_cancel(((struct tcp_server_desc *)server)->thread) != 0) {
        /* errno set */
        LOG_ERROR(server, "Server could not be stopped (errno=%d)", errno);
        LOG_CLOSE(server);
        return (-1);
    }
    /* wait for the listening thread to terminate */
    LOG_INFO(server, "Server stopped on socket %d\n", fildes);
    errno = 0;
    /* close all client sockets */
    for (i = 0; i <= ((struct tcp_server_desc *)server)->fdmax; i++) {
        if ((i != fildes) && FD_ISSET(i, &server->master)) {
            (void)close(i);
        }
    }
    /* close the log file */
    if (server->log_fp != NULL) {
        fprintf(server->log_fp, "+++ Connection summary for TCP/IP Server on port %s with data size %zu +++\n",
            server->sock_port, server->data_size);
        fprintf(server->log_fp, "%11lu packet(s) sent to clients\n", server->sent_pkg);
        fprintf(server->log_fp, "%11lu packet(s) received from clients\n", server->recv_pkg);
        fprintf(server->log_fp, "%11lu packet(s) not processed by the host\n", server->lost_pkg);
        fprintf(server->log_fp, "+++ TCP/IP Server terminated: elapsed time %.4f sec +++\n",
            time_diff(server->start, time_get()));
        fclose(server->log_fp);
        server->log_fp = NULL;
        server->log_opt = 0;
    }
    /* destroy the TCP/IP server descriptor */
    DESTROY_MUTEX(server);
    FREE_SERVER(server);
    /* close the socket */
    errno = 0;
    return close(fildes);
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
int tcp_server_send(tcp_server_t server, const void *data, size_t size) {
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
    ENTER_CRITICAL_SECTION((struct tcp_server_desc *)server);
    write_fds = ((struct tcp_server_desc *)server)->master;
    fdmax = ((struct tcp_server_desc *)server)->fdmax;
    LEAVE_CRITICAL_SECTION((struct tcp_server_desc *)server);

    /* remove the listener (we don't want to hear our own crap) */
    FD_CLR(((struct tcp_server_desc *)server)->sock_fd, &write_fds);
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
        ((struct tcp_server_desc *)server)->sent_pkg++;
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
    struct tcp_server_desc *server = (struct tcp_server_desc *)arg;

    fd_set read_fds;  /* file descriptor list for select() */
    FD_ZERO(&read_fds);

    int newfd;        /* newly accept()ed socket descriptor */
    struct sockaddr_storage remoteaddr; /* client address */
    socklen_t addrlen;

    char buf[MAX_BUF_SIZE];  /* buffer for client data */
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
        if ((rc = select(server->fdmax+1, &read_fds, NULL, NULL, NULL)) < 0) {
            if (errno != EINTR) {
                LOG_ERROR(server, "%s (errno=%d)", strerror(errno), errno);
                // TODO: Is emergency treatment required?
            }
            continue;  // NOTE: EINTR (Ctrl+C) is intentionally ignored!
        }
        /* loop through the existing connections looking for data to read */
        for (i = 0; i <= server->fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server->sock_fd) {
                    /* handle new connections */
                    addrlen = sizeof(remoteaddr);
                    if ((newfd = accept(server->sock_fd, (struct sockaddr *)&remoteaddr, &addrlen)) >= 0) {
                        /* disable Nagle's algorithm for TCP connections */
#if (OPTION_TCPIP_TCPDELAY == 0)
                        int opt = 1;
                        if (setsockopt(newfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0) {
                            LOG_ERROR(server, "Set TCP_NODELAY failed on socket %d (errno=%d)", newfd, errno);
                            close(newfd);
                            continue;  // TODO: Is emergency treatment required?
                        }
#endif
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
                        continue; // TODO: Is emergency treatment required?
                    }
                } else {
                    /* handle data from a client */
                    if ((nbytes = recv(i, buf, server->data_size, 0)) > 0) {
                        LOG_RECV(server, "Received %ld bytes from socket %d\n", nbytes, i);
                        LOG_DATA(server, LOG_DIR_RECV, buf, nbytes);
                        server->recv_pkg++;
                        /* notify the server application */
                        if (server->recv_cbk != NULL) {
                            if ((rc = server->recv_cbk(buf, nbytes, server->recv_ref)) < 0) {
                                LOG_ERROR(server, "Receive callback failed for socket %d (error=%d)", i, rc);
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
                                // TODO: Is further treatment required?
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

/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
