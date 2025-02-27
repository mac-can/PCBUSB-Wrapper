#	SPDX-License-Identifier: GPL-2.0-or-later
#
#	CAN Utilities for generic CAN Interfaces (CAN API V3)
#
#	Copyright (c) 2005-2010 Uwe Vogt, UV Software, Friedrichshafen
#	Copyright (c) 2012-2025 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
#
#	These utilities are free software; you can redistribute them and/or modify
#	them under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
#
#	These utilities are distributed in the hope that they will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License along
#	with these utilities; if not, see <https://www.gnu.org/licenses/>.
#
all:
	@echo "\033[1mBuilding my beloved CAN Utilities...\033[0m"
	$(MAKE) -C can_moni $@
#	$(MAKE) -C can_play $@
	$(MAKE) -C can_port $@
	$(MAKE) -C can_send $@
	$(MAKE) -C can_simu $@
	$(MAKE) -C can_test $@

clean:
	$(MAKE) -C can_moni $@
#	$(MAKE) -C can_play $@
	$(MAKE) -C can_port $@
	$(MAKE) -C can_send $@
	$(MAKE) -C can_simu $@
	$(MAKE) -C can_test $@

pristine:
	$(MAKE) -C can_moni $@
#	$(MAKE) -C can_play $@
	$(MAKE) -C can_port $@
	$(MAKE) -C can_send $@
	$(MAKE) -C can_simu $@
	$(MAKE) -C can_test $@

install:
#	$(MAKE) -C can_moni $@
##	$(MAKE) -C can_play $@
#	$(MAKE) -C can_port $@
#	$(MAKE) -C can_send $@
#	$(MAKE) -C can_simu $@
#	$(MAKE) -C can_test $@

check:
	$(MAKE) -C can_moni $@
#	$(MAKE) -C can_play $@
	$(MAKE) -C can_port $@
	$(MAKE) -C can_send $@
	$(MAKE) -C can_simu $@
	$(MAKE) -C can_test $@
