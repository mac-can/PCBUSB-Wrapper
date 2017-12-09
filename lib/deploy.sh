#!/bin/sh

ProjName="$1"
ProjVersion="$2"
ProjTarball="CAN_API_V3_for_PCBUSB_on_macOS_v"

pushd $(PWD) > /dev/null
cd ..

if [ -d "$ProjName" ]
then
	rm -dfR "$ProjName"/*
else
	mkdir "$ProjName"
	mkdir "$ProjName/inc"
fi

cp "Release/lib$ProjName.$ProjVersion.dylib" "$ProjName/lib$ProjName.$ProjVersion.dylib"
cp "../api/can_defs.h" "$ProjName/inc/can_defs.h"
cp "../api/can_api.h" "$ProjName/inc/can_api.h"
cp "install.sh" "$ProjName/install.sh"
cp "COPYRIGHT" "$ProjName/COPYRIGHT"
cp "LICENSE" "$ProjName/LICENSE"
cp "README" "$ProjName/README"

if [ -e "$ProjTarball$ProjVersion.tar.gz" ]
then
	rm "$ProjTarball$ProjVersion.tar.gz"
	rm "$ProjTarball$ProjVersion.readme"
fi
tar -czf "$ProjTarball$ProjVersion.tar.gz" "$ProjName"
md5 "$ProjTarball$ProjVersion.tar.gz" > "$ProjTarball$ProjVersion.tar.gz.md5" 
cp "README" "$ProjTarball$ProjVersion.readme"
rm -dfR "$ProjName"
popd > /dev/null