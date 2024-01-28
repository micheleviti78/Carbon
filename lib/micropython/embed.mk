# This file is part of the MicroPython project, http://micropython.org/
# The MIT License (MIT)
# Copyright (c) 2022-2023 Damien P. George
# 
# This file is intended to be included by a Makefile in a custom project.

ROOT_LIB = $(ROOT_PROJECT)/lib

ROOT_MICROPYTHON = $(ROOT_LIB)/micropython/

MICROPYTHON_TOP = $(ROOT_MICROPYTHON)/micropython

# Set the build output directory for the generated files.
BUILD = build-embed

# Include the core environment definitions; this will set $(TOP).
include $(MICROPYTHON_TOP)/py/mkenv.mk

# Include py core make definitions.
include $(MICROPYTHON_TOP)/py/py.mk

# Set the location of the MicroPython embed port.
MICROPYTHON_EMBED_PORT = $(ROOT_MICROPYTHON)/port/

# Set default makefile-level MicroPython feature configurations.
MICROPY_ROM_TEXT_COMPRESSION ?= 0

# Set CFLAGS for the MicroPython build.
CFLAGS += -I$(ROOT_MICROPYTHON) -I$(MICROPYTHON_TOP) -I$(BUILD) -I$(MICROPYTHON_EMBED_PORT) -I$(ROOT_LIB)/freertos/include -I$(ROOT_LIB)/freertos/portable/GCC/ARM_CM7/r0p1 -I$(ROOT_PROJECT)/CM7/conf
CFLAGS += -Wall -Werror -std=c99

# Define the required generated header files.
GENHDR_OUTPUT = $(addprefix $(BUILD)/genhdr/, \
	moduledefs.h \
	mpversion.h \
	qstrdefs.generated.h \
	root_pointers.h \
	)

# Define the top-level target, the generated output files.
.PHONY: all
all: micropython-embed-package

clean: clean-micropython-embed-package

.PHONY: clean-micropython-embed-package
clean-micropython-embed-package:
	$(RM) -rf $(PACKAGE_DIR)

PACKAGE_DIR ?= micropython_embed
PACKAGE_DIR_LIST = $(addprefix $(PACKAGE_DIR)/,py extmod shared/runtime genhdr port)

.PHONY: micropython-embed-package
micropython-embed-package: $(GENHDR_OUTPUT)
	$(ECHO) "Generate micropython_embed output:"
	$(Q)$(RM) -rf $(PACKAGE_DIR_LIST)
	$(Q)$(MKDIR) -p $(PACKAGE_DIR_LIST)
	$(ECHO) "- py"
	$(Q)$(CP) $(TOP)/py/*.[ch] $(PACKAGE_DIR)/py
	$(ECHO) "- extmod"
	$(Q)$(CP) $(TOP)/extmod/modplatform.h $(PACKAGE_DIR)/extmod
	$(ECHO) "- shared"
	$(Q)$(CP) $(TOP)/shared/runtime/gchelper.h $(PACKAGE_DIR)/shared/runtime
	$(Q)$(CP) $(TOP)/shared/runtime/gchelper_thumb2.s $(PACKAGE_DIR)/shared/runtime
	$(Q)$(CP) $(TOP)/shared/runtime/gchelper_native.c $(PACKAGE_DIR)/shared/runtime
	$(ECHO) "- genhdr"
	$(Q)$(CP) $(GENHDR_OUTPUT) $(PACKAGE_DIR)/genhdr
	$(Q)$(CP) $(TOP)/../port/*.[ch] $(PACKAGE_DIR)/port

# Include remaining core make rules.
include $(MICROPYTHON_TOP)/py/mkrules.mk
