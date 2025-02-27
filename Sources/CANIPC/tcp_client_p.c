/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'tcp_client' - Stream Socket Client (TCP/IP)
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
/** @file        tcp_client.c
 *
 *  @brief       Stream Socket Client (TCP/IP).
 *
 *  @author      $Author: quaoar $
 *
 *  @version     $Rev: 1469 $
 *
 *  @addtogroup  tcp
 *  @{
 */
#include "tcp_client.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>


/*  -----------  options  ------------------------------------------------
 */


/*  -----------  defines  ------------------------------------------------
 */


/*  -----------  types  --------------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  functions  ----------------------------------------------
 */
int tcp_client_connect(const char *server) {
    int fildes = (-1);
    char *host = NULL, *port = NULL;
    struct addrinfo hints, *ai, *p;
    int error;
    errno = 0;
    /* Obtain address(es) matching host/port. */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // alow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // stream socket (TCP)
    hints.ai_flags = 0;               // no flags (client)
    hints.ai_protocol = IPPROTO_IP;   // any protocol

    /* check the server address */
    if ((server == NULL) || (strlen(server) == 0)) {
        errno = EINVAL;
        return (-1);
    }
    /* copy the server address ("<host>:<port>") */
    if ((host = strdup(server)) == NULL) {
        /* errno set */
        return (-1);
    }
    /* get the network address from the server address */
    if ((port = strchr(host, ':')) == NULL) {
        error = errno;
        free(host);
        errno = error;
        return (-1);
    }
    /* host is now the network address */
    *port++ = '\0';

    /* obtain address(es) matching host/port */
    if (getaddrinfo(host, port, &hints, &ai) != 0) {
        error = errno ? errno : EADDRNOTAVAIL;
        free(host);
        errno = error;
        return (-1);
    }
    /* loop through all the results and connect to the first we can */
    for (p = ai; p != NULL; p = p->ai_next) {
        if ((fildes = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            error = errno;
            fildes = (-1);
            continue;
        }
        if (connect(fildes, p->ai_addr, p->ai_addrlen) < 0) {
            error = errno;
            close(fildes);
            fildes = (-1);
            continue;
        }
        /* Yeah, successfull connect to the server! */
        break;
    }
    /* get rid of garbage */
    freeaddrinfo(ai);
    free(host);
    host = NULL;
    port = NULL;

    /* check if connected to the server */
    if (p == NULL) {
        return (-1);
    }
    /* disable Nagle's algorithm for TCP connections */
#if (OPTION_TCPIP_TCPDELAY == 0) 
    int opt = 1;
    if (setsockopt(fildes, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0) {
        error = errno;
        close(fildes);
        fildes = (-1);
        errno = error;
        return (-1);
    }
#endif
    /* return the socket file descriptor */
    return fildes;
}

int tcp_client_close(int fildes) {
    errno = 0;

    return close(fildes);
}

ssize_t tcp_client_send(int fildes, const void *buffer, size_t length) {
    ssize_t n = 0;
    errno = 0;

    /* check the buffer and its length */
    if ((buffer == NULL) || (length == 0)) {
        errno = EINVAL;
        return (-1);
    }
    /* send data to the server */
    n = send(fildes, buffer, length, 0);

    /* return the number of bytes sent, or -1 on error */
    return n;
}

ssize_t tcp_client_recv(int fildes, void *buffer, size_t length, unsigned short timeout) {
    ssize_t n = 0;
    fd_set readfds;
    struct timeval tv;
    errno = 0;

    /* check the buffer and its length */
    if ((buffer == NULL) || (length == 0)) {
        errno = EINVAL;
        return (-1);
    }
    /* add the file descriptor to the set */
    FD_ZERO(&readfds);
    FD_SET(fildes, &readfds);
    /* check the file descriptor for readability */
    if (timeout != 0) {
        /* set the timeout */
        tv.tv_sec = (time_t)(timeout / 1000);
        tv.tv_usec = (suseconds_t)((timeout % 1000) * 1000);
        /* wait for the file descriptor to become readable */
        if (select(fildes + 1, &readfds, NULL, NULL, (timeout != USHRT_MAX) ? &tv : NULL) <= 0) {
            /* errno set */
            return (-1);
        }
        /* check if there is data to read */
        if (!FD_ISSET(fildes, &readfds)) {
            errno = ENODATA;
            return (-1);
        }
    }
    /* receive data from the server (if any) */
    n = recv(fildes, buffer, length, 0);

    /* check the number of bytes received */
    if (n != (ssize_t)length) {
        if (n >= 0) {
            errno = (n == 0) ? ENODATA : EBADMSG;
        } else {
            /* errno set by recv() */
        }        
        return (-1);
    }
    /* return the number of bytes received */
    return n;
}

/*  -----------  local functions  ----------------------------------------
 */

/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
