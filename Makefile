##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

PREFIX		?= arm-none-eabi-

TARGETS	:= stm32/f4

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
# Do not print "Entering directory ...".
MAKEFLAGS += --no-print-directory
endif

OPENCM3_DIR ?= $(realpath libopencm3)


all: build


bin: build
hex: build
srec: build
list: build
images: build

build: lib src

lib:
	$(Q)if [ ! "`ls -A $(OPENCM3_DIR)`" ] ; then \
		printf "######## ERROR ########\n"; \
		printf "\tlibopencm3 is not initialized.\n"; \
		printf "\tPlease run:\n"; \
		printf "\t$$ git submodule init\n"; \
		printf "\t$$ git submodule update\n"; \
		printf "\tbefore running make.\n"; \
		printf "######## ERROR ########\n"; \
		exit 1; \
		fi
	$(Q)$(MAKE) -C $(OPENCM3_DIR)

SRC_DIRS:= src
$(SRC_DIRS): lib
	@printf "  BUILD   $@\n";
	$(Q)$(MAKE) --directory=$@ OPENCM3_DIR=$(OPENCM3_DIR)

SRC: $(SRC_DIRS)
	$(Q)true

srcclean: $(SRC_DIRS:=.clean)

clean: srcclean styleclean
	$(Q)$(MAKE) -C libopencm3 clean

stylecheck: $(SRC_DIRS:=.stylecheck)
styleclean: $(SRC_DIRS:=.styleclean)


%.clean:
	$(Q)if [ -d $* ]; then \
		printf "  CLEAN   $*\n"; \
		$(MAKE) -C $* clean OPENCM3_DIR=$(OPENCM3_DIR) || exit $?; \
	fi;

%.styleclean:
	$(Q)$(MAKE) -C $* styleclean OPENCM3_DIR=$(OPENCM3_DIR)

%.stylecheck:
	$(Q)$(MAKE) -C $* stylecheck OPENCM3_DIR=$(OPENCM3_DIR)

