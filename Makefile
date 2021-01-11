# #############################################################################
#
# project   :  CAN - Controller Area Network.
#
# purpose   :  CAN API V3 Tester (PCANBasic)
#
# copyright :  (C) 2019-2021 by UV Software, Berlin
#
# compiler  :  Apple clang version 12.0.0 (clang-1200.0.32.28)
#
# author    :  Uwe Vogt, UV Software
#
# e-mail    :  uwe.vogt@uv-software.de
#
# #############################################################################

all:
	@./build_no.sh
	@echo "Building PCBUSB (build "$(shell git log -1 --pretty=format:%h)")..."
	$(MAKE) -C Trial $@
	$(MAKE) -C Library $@
#	$(MAKE) -C Utilities/can_test $@
#	$(MAKE) -C Utilities/can_moni $@

clean:
	$(MAKE) -C Trial $@
	$(MAKE) -C Library $@
#	$(MAKE) -C Utilities/can_test $@
#	$(MAKE) -C Utilities/can_moni $@

install:
	$(MAKE) -C Trial $@
	$(MAKE) -C Library $@
##	$(MAKE) -C Utilities/can_test $@
##	$(MAKE) -C Utilities/can_moni $@

build_no:
	@./build_no.sh
	@cat Sources/build_no.h

# #############################################################################
