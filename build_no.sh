#!/bin/sh
echo "/*  -- Do not commit this file --" > Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  project   :  CAN - Controller Area Network" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  purpose   :  CAN Interface API, Version 3 (PCBUSB)" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  copyright :  (C) 2019-2021, UV Software, Berlin" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  compiler  :  Apple clang version 12.0.0 (clang-1200.0.32.28)" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  export    :  BUILD_NO" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  includes  :  (none)" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  author    :  Uwe Vogt, UV Software" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " *  e-mail    :  uwe.vogt@uv-software.de" >> Sources/build_no.h
echo " *" >> Sources/build_no.h
echo " */" >> Sources/build_no.h
echo "#ifndef BUILD_NO_H_INCLUDED" >> Sources/build_no.h
echo "#define BUILD_NO_H_INCLUDED" >> Sources/build_no.h
echo "#define BUILD_NO 0x"$(git log -1 --pretty=format:%h) >> Sources/build_no.h
echo "#define STRINGIFY(X) #X" >> Sources/build_no.h
echo "#define TOSTRING(X) STRINGIFY(X)" >> Sources/build_no.h
echo "#define SVN_REV_INT (BUILD_NO)" >> Sources/build_no.h
echo "#define SVN_REV_STR TOSTRING(BUILD_NO)" >> Sources/build_no.h
echo "#endif" >> Sources/build_no.h
echo "/*  ----------------------------------------------------------------------" >> Sources/build_no.h
echo " *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany" >> Sources/build_no.h
echo " *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903" >> Sources/build_no.h
echo " *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/" >> Sources/build_no.h
echo " */" >> Sources/build_no.h
