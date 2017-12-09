#!/bin/sh

ProjName=UVCAN300
ProjVersion=0.1.rc1
BaseDir=$(dirname $0)
InstallDir=/usr/local/lib
IncludeDir=/usr/local/include

if [ -f "$BaseDir/lib$ProjName.$ProjVersion.dylib" ]
then
	if [ -d "$InstallDir" ]
	then
		cp "$BaseDir/lib$ProjName.$ProjVersion.dylib" "$InstallDir/lib$ProjName.$ProjVersion.dylib"
	else
		mkdir "$InstallDir"; cp "$BaseDir/lib$ProjName.$ProjVersion.dylib" "$InstallDir/lib$ProjName.$ProjVersion.dylib"
	fi
	if [ ! -f "$InstallDir/lib$ProjName.dylib" ]
	then
		ln -s "$InstallDir/lib$ProjName.$ProjVersion.dylib" "$InstallDir/lib$ProjName.dylib"
	else
		rm "$InstallDir/lib$ProjName.dylib"; ln -s "$InstallDir/lib$ProjName.$ProjVersion.dylib" "$InstallDir/lib$ProjName.dylib"
	fi
	chmod 755 "$InstallDir/lib$ProjName.$ProjVersion.dylib"
	chmod 755 "$InstallDir/lib$ProjName.dylib"
else
	echo "error dylib"
fi

# This library shall by used through libUVCAN300 and its headers!
# if [ -f "$BaseDir/can_api.h" ] && [ -f "$BaseDir/can_defs.h" ]
# then
#	if [ -d "$IncludeDir" ]
#	then
#		cp "$BaseDir/inc/can_api.h" "$IncludeDir/can_api.h"; cp "$BaseDir/inc/can_defs.h" "$IncludeDir/can_defs.h"
#	else
#		mkdir "$IncludeDir"; cp "$BaseDir/inc/can_api.h" "$IncludeDir/can_api.h"; cp "$BaseDir/inc/can_defs.h" "$IncludeDir/can_defs.h"
#	fi
# else
#	echo "error header"
# fi
