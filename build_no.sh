#!/bin/sh
echo "/*  -- Do not commit this file --" > Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  CAN Interface API, Version 3 (for PEAK PCAN-Basic Interfaces)" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  Copyright (C) 2010-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  This file is part of PCANBasic-Wrapper." >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  PCANBasic-Wrapper is free software : you can redistribute it and/or modify" >> Sources/build_no.h
echo " *  it under the terms of the GNU Lesser General Public License as published by" >> Sources/build_no.h
echo " *  the Free Software Foundation, either version 3 of the License, or" >> Sources/build_no.h
echo " *  (at your option) any later version." >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  PCANBasic-Wrapper is distributed in the hope that it will be useful," >> Sources/build_no.h
echo " *  but WITHOUT ANY WARRANTY; without even the implied warranty of" >> Sources/build_no.h
echo " *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" >> Sources/build_no.h
echo " *  GNU Lesser General Public License for more details." >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  You should have received a copy of the GNU Lesser General Public License" >> Sources/build_no.h
echo " *  along with PCANBasic-Wrapper.  If not, see <http://www.gnu.org/licenses/>." >> Sources/build_no.h
echo " */" >> Sources/build_no.h
echo "#ifndef BUILD_NO_H_INCLUDED" >> Sources/build_no.h
echo "#define BUILD_NO_H_INCLUDED" >> Sources/build_no.h
git log -1 --pretty=format:%h > /dev/null 2> /dev/null
if [ $? -eq 0 ]
then
    echo "#define BUILD_NO 0x"$(git log -1 --pretty=format:%h) >> Sources/build_no.h
else
    echo "#define BUILD_NO 0xDEADC0DE" >> Sources/build_no.h
fi
echo "#define STRINGIFY(X) #X" >> Sources/build_no.h
echo "#define TOSTRING(X) STRINGIFY(X)" >> Sources/build_no.h
echo "#endif" >> Sources/build_no.h