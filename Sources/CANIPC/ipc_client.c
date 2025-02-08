/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later */
/*
 *  Software for Industrial Communication, Motion Control and Automation
 *
 *  Copyright (c) 2002-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *  All rights reserved.
 *
 *  Module 'ipc_client' - Inter-Process Communication (IPC) client
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
 *  This module is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This module is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this module.  If not, see <https://www.gnu.org/licenses/>.
 */
/** @file        ipc_client.c
 *
 *  @brief       Inter-Process Communication (IPC) client.
 *
 *  @author      $Author: sedna $
 *
 *  @version     $Rev: 1433 $
 *
 *  @addtogroup  ipc
 *  @{
 */
#include "ipc_client.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
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
#define LOCALHOST_STR   "localhost"
#define LOCALHOST_ADDR  IPC_ADDR_LOCALHOST


/*  -----------  types  --------------------------------------------------
 */


/*  -----------  prototypes  ---------------------------------------------
 */


/*  -----------  variables  ----------------------------------------------
 */


/*  -----------  functions  ----------------------------------------------
 */
int ipc_client_connect(const char *server, int sock_type) {
    int fildes = (-1);
    char *host = NULL;
    long port = 0;
    char *colon = NULL;
    char *endptr = NULL;
    struct sockaddr_in address;
    int error, result = 0;
    errno = 0;
#if (1)
    // only stream sockets are supported yet!
    if (sock_type != IPC_SOCK_TCP) {
        errno = EINVAL;
        return (-1);
    }
#else
    // TODO: implement UDP, SCTP and raw socket
    // TODO: map the following attributes to corresponding values:
    //       - sock_type: SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW
    //       - sock_domain: AF_INET, AF_INET6, AF_UNIX, AF_PACKET
    //       - sock_protocol: IPPROTO_IP, IPPROTO_TCP, IPPROTO_UDP, IPPROTO_SCTP
    // TODO: what's about Nagle's algorithm, and other stuff like this?
#endif
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
    if ((colon = strchr(host, ':')) == NULL) {
        error = errno;
        free(host);
        errno = error;
        return (-1);
    }
    /* host is now the network address */
    *colon = '\0';
    colon++;
    /* check for "localhost" and replace it with ipv4 address "127.0.0.1" */
    if ((strcmp(host, LOCALHOST_STR) == 0) && (strlen(host) == strlen(LOCALHOST_ADDR))) {
        strcpy(host, LOCALHOST_ADDR);
        errno = 0;
    }
    /* get the port number from the server address */
    port = strtol(colon, &endptr, 10);
    if ((*endptr != '\0') || (port <= 0) || (port > (long)UINT16_MAX)) {
        error = EADDRNOTAVAIL;
        free(host);
        errno = error;
        return (-1);
    }
    /* create the socket file descriptor */
    if ((fildes = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {  // TODO: sock_type and protocol
        error = errno;
        free(host);
        errno = error;
        return (-1);
    }
    address.sin_family = AF_INET;
    address.sin_port = htons((uint16_t)port);

    /* convert IPv4 and IPv6 addresses from text to binary form */
    if ((result = inet_pton(AF_INET, host, &address.sin_addr)) <= 0) {
        if (result == 0) {
            error = EADDRNOTAVAIL;
        } else {
            error = errno;
        }
        close(fildes);
        free(host);
        errno = error;
        return (-1);
    }
    /* free the host string (not needed anymore) */
    free(host);
    errno = 0;

    /* connect to the server */
    if (connect(fildes, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error = errno;
        close(fildes);
        errno = error;
        return (-1);
    }
    /* return the socket file descriptor */
    return fildes;
}

int ipc_client_close(int fildes) {
    errno = 0;
#if !defined(_WIN32) && !defined(_WIN64)
    return close(fildes);
#else
    return closesocket(fildes);
#endif
}

ssize_t ipc_client_send(int fildes, const void *buffer, size_t length) {
    ssize_t n = 0;
    errno = 0;

    /* check the buffer and its length */
    if ((buffer == NULL) || (length == 0)) {
        errno = EINVAL;
        return (-1);
    }
    /* send data to the server */
#if !defined(_WIN32) && !defined(_WIN64)
    n = send(fildes, buffer, length, 0);
#else
    n = send(fildes, buffer, (int)length, 0);
#endif
    /* return the number of bytes sent, or -1 on error */
    return n;
}

ssize_t ipc_client_recv(int fildes, void *buffer, size_t length, unsigned short timeout) {
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
#if !defined(_WIN32) && !defined(_WIN64)
    n = recv(fildes, buffer, length, 0);
#else    
    n = recv(fildes, buffer, (int)length, 0);
#endif
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


/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
