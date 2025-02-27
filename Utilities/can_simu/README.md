__CAN-over-Ethernet Server Simulator, Version 0.4__ \
Copyright &copy; 2008,2025 by Uwe Vogt, UV Software, Berlin

```
Usage: can_port <port> [<option>...]
Options:
     --security-risks="I ACCEPT"      accept security risks (skip interactive input)
     --logging=<level>                set logging level (default=0)
 -h, --help                           display this help screen and exit
     --version                        show version information and exit
Arguments:
  <port>                              TCP/IP port number
Syntax:
 <can_frame>:
  <can_id>#{data}                     for CAN CC data frames
  <can_id>#R{len}                     for CAN CC remote frames
  <can_id>##<flags>{data}             for CAN FD data frames (up to 64 bytes)
 <can_id>:
  3  ASCII hex-chars (0 .. F) for Standard frame format (SFF) or
  8  ASCII hex-chars (0 .. F) for eXtended frame format (EFF)
 {data}:
  0 .. 8   ASCII hex-values in CAN CC mode (optionally separated by '.') or
  0 .. 64  ASCII hex-values in CAN FD mode (optionally separated by '.')
 {len}:
  an optional 0 .. 8 value as RTR frames can contain a valid DLC field
 <flags>:
  one ASCII hex-char (0 .. F) which defines CAN FD flags:
    4 = FDF                           for CAN FD frame format
    5 = FDF and BRS                   for CAN FD with Bit Rate Switch
    6 = FDF and ESI                   for CAN FD with Error State Indicator
    7 = FDF, BRS and ESI              all together now
Hazard note:
  If you open a port for socket communication while using this program, it may
  expose your computer to security vulnerabilities, unauthorized access, data
  interception, denial of service attacks, and resource exhaustion.
  It is strongly recommended to implement appropriate security measures to
  mitigate these risks.
  If you connect your CAN device to a real CAN network when using this program,
  you might damage your application.
```

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, see <https://www.gnu.org/licenses/>.
