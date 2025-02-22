### RocketCAN - CAN-over-Ethernet

_Copyright &copy; 2008-2025  Uwe Vogt, UV Software, Berlin (info@uv-software.com)_ \
_All rights reserved._

# CAN/IPC - CAN Interprocess Communication 

To allow multiple applications to communicate simultaneously with the same CAN channel, a CAN/IPC server (also called a RocketCAN server) provides a set of functions that manage the connections and data exchange with this CAN channel.
Applications (RocketCAN clients) then connect to the RocketCAN server to send and receive CAN messages as if they were communicating directly with the CAN channel.
Client applications can be run both locally and remotely as long as the port is enabled.

The connection between the server and the client applications is realized as a sequential, reliable, bidirectional, connection-oriented communication mechanism (stream socket).

_Note: The hostname `localhost` must be connected to the IPv4 address `127.0.0.1`.
No default port is specified. The selection of a port for CAN/IPC communication is the responsibility of the user._

# CAN/IPC API

## CAN/IPC Server API

```C
typedef int (*tcp_event_cbk_t)(const void *, size_t);

extern tcp_server_t tcp_server_start(const char *service, size_t data_size, tcp_event_cbk_t recv_cbk, int logging);
extern int tcp_server_stop(tcp_server_t server);

extern int tcp_server_send(tcp_server_t server, const void *data, size_t size);
```
See header file `tcp_server.h` for a description of the provided functions.

## CAN/IPC Client API

```C
extern int tcp_client_connect(const char *server);
extern int tcp_client_close(int fildes);

extern ssize_t tcp_client_send(int fildes, const void *buffer, size_t length);
extern ssize_t tcp_client_recv(int fildes, void *buffer, size_t length, unsigned short timeout);
```
See header file `tcp_client.h` for a description of the provided functions.

# RocketCAN Message 

## Message Format

```C
typedef struct can_tcp_message_t_ {
    uint32_t id;                    /**< CAN identifier (11-bit or 29-bit) */
    uint8_t  flags;                 /**< message flags (8-bit, CAN API V3) */
    uint8_t  length;                /**< data length (in [byte], not CAN DLC!) */
    uint8_t  status;                /**< status register (8-bit, CAN API Vx) */
    uint8_t  extra;                 /**< unspecific bit-field (8-bit) */
    uint8_t  data[CANTCP_MAX_LEN];  /**< data (to hold CAN FD payload) */
    struct timespec timestamp;      /**< time-stamp { sec, nsec } */
    uint8_t  reserved[4];           /**< reserved (4-byte) */
    uint16_t busload;               /**< bus load (0 .. 10'000 = 0 .. 100%) */
    uint8_t  ctrlchar;              /**< control character (ETX or EOT) */
    uint8_t  checksum;              /**< J1850 checksum (8-bit) */
} can_tcp_message_t;
```

## Byte Order

RocketCAN messages must be transmitted in __network byte order__.

## This and That

_Note: Nagle's algorithm is disabled by default. This can be overridden by setting `OPTION_TCPIP_TCPDELAY` to a non-zero value (e.g. in the build environment)._

### Dual-License

This work is dual-licensed under the terms of the BSD 2-Clause "Simplified" License and under the terms of the GNU General Public License v2.0 (or any later version).
You can choose between one of them if you use this work in whole or in part.

### Contact

E-Mail: mailto://info@uv-software.com \
Internet: https://www.uv-software.com
