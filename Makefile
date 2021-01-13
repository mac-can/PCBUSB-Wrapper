#
#	CAN Interface API, Version 3 (for PEAK PCAN-Basic Interfaces)
#
#	Copyright (C) 2010-2021  Uwe Vogt, UV Software, Berlin (info@uv-software.com)
#
#	This library is part of PCANBasic-Wrapper.
#
#	PCANBasic-Wrapper is free software: you can redistribute it and/or modify
#	it under the terms of the GNU Lesser General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	PCANBasic-Wrapper is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU Lesser General Public License for more details.
#
#	You should have received a copy of the GNU Lesser General Public License
#	along with PCANBasic-Wrapper.  If not, see <http://www.gnu.org/licenses/>.
#
all:
	@./build_no.sh
	@echo "Building PCANBasic-Wrapper..."
	$(MAKE) -C Trial $@
	$(MAKE) -C Libraries/PCAN $@
	$(MAKE) -C Libraries/CANAPI $@
#	$(MAKE) -C Utilities/can_test $@
#	$(MAKE) -C Utilities/can_moni $@

clean:
	$(MAKE) -C Trial $@
	$(MAKE) -C Libraries/PCAN $@
	$(MAKE) -C Libraries/CANAPI $@
#	$(MAKE) -C Utilities/can_test $@
#	$(MAKE) -C Utilities/can_moni $@

install:
	$(MAKE) -C Trial $@
	$(MAKE) -C Libraries/PCAN $@
	$(MAKE) -C Libraries/CANAPI $@
##	$(MAKE) -C Utilities/can_test $@
##	$(MAKE) -C Utilities/can_moni $@

build_no:
	@./build_no.sh
	@cat Sources/build_no.h

# #############################################################################
