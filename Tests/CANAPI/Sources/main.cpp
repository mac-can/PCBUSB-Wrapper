//  SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-or-later
//
//  CAN Interface API, Version 3 (Testing)
//
//  Copyright (c) 2004-2025 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//  All rights reserved.
//
//  This file is part of CAN API V3.
//
//  CAN API V3 is dual-licensed under the BSD 2-Clause "Simplified" License
//  and under the GNU General Public License v2.0 (or any later version).
//  You can choose between one of them if you use this file.
//
//  (1) BSD 2-Clause "Simplified" License
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this
//     list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  CAN API V3 IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF CAN API V3, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  (2) GNU General Public License v2.0 or later
//
//  CAN API V3 is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  CAN API V3 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with CAN API V3; if not, see <https://www.gnu.org/licenses/>.
//
#include <cstdio>
#include <iostream>
#include "anykey.h"
#include "pch.h"

GTEST_API_ int main(int argc, char **argv) {
    std::cout << "CAN API V3 C++ Testing (";
#if !defined(_MSC_VER)
    std::cout << __VERSION__ << ")" << std::endl;
#else
#if !defined(_WIN64)
    std::cout << "MSC " << _MSC_VER << " for x86)" << std::endl;
#else
    std::cout << "MSC " << _MSC_VER << " for x64)" << std::endl;
#endif
#endif
    std::cout << CCanDriver::GetVersion() << std::endl;
    std::cout << "Copyright (c) 2004-2012 by UV Software, Friedrichshafen" << std::endl;
    std::cout << "Copyright (c) 2013-2025 by UV Software, Berlin" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << " (" << REVISION_NO << ")" << std::endl;
    // --- initialize GoogleTest framework --
    printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    // --- scan own command-line options ---
    char szError[256] = "";
    if (!g_Options.ScanOptions(argc, argv, szError, 256)) {
        std::cerr << "+++ error: " << szError << std::endl;
        return 1;
    }
    else if (g_Options.ShowHelp()) {
        return 0;
    }
#if (OPTION_CANTCP_ENABLED != 0)
    // --- start CAN server ---
    if (g_Options.IsCanServerEnabled()) {
        g_CanServer.SetLoggingLevel(g_Options.GetCanServerLoggingLevel());
        if (g_CanServer.StartServer(g_Options.GetCanServerService()) < 0) {
            std::cerr << "+++ error: failed to start CAN server" << std::endl;
            return 1;
        }
    }
#endif
    // --- interactive start ---
    if (g_Options.StartInteractive()) {
        std::cout << "Press any key to start testing...";
        std::cout << std::flush;
        (void)getkey();
        std::cout << std::endl;
    }
    // --- test execution starts here --
    int res = RUN_ALL_TESTS();
#if (OPTION_CANTCP_ENABLED != 0)
    // --- stop CAN server ---
    if (g_Options.IsCanServerEnabled()) {
        g_CanServer.StopServer();
    }
#endif
    // --- say goodbye ---
    return res;
}

// $Id: main.cpp 1456 2025-02-19 21:22:16Z sedna $  Copyright (c) UV Software, Berlin //
