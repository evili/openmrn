-include ../../config.mk
OPENMRNPATH ?= $(shell \
sh -c "if [ \"X`printenv OPENMRNPATH`\" != \"X\" ]; then printenv OPENMRNPATH; \
     elif [ -d /opt/openmrn/src ]; then echo /opt/openmrn; \
     elif [ -d ~/openmrn/src ]; then echo ~/openmrn; \
     elif [ -d ../../../src ]; then echo ../../..; \
     else echo OPENMRNPATH not found; fi" \
)

# Find STM32CubeF7 libraries
include $(OPENMRNPATH)/etc/stm32cubef7.mk

LDFLAGSEXTRA += 
SYSLIBRARIESEXTRA +=  -Wl,--whole-archive \
                      -lfreertos_drivers_stm32cubef74xx \
                      -Wl,--no-whole-archive
OBJEXTRA +=

CFLAGS += -DSTM32F746xx  -DUSE_HAL_DRIVER \
	-D__weak='__attribute__((weak))' -D__packed='__attribute__((__packed__))'
CXXFLAGS += -DSTM32F746xx  -DUSE_HAL_DRIVER \
	-D__weak='__attribute__((weak))' -D__packed='__attribute__((__packed__))'
OPENOCDARGS = -f board/stm32f7.cfg

TARGET := freertos.armv7em
include $(OPENMRNPATH)/etc/prog.mk

ifeq ($(call find_missing_deps,OPENOCDPATH OPENOCDSCRIPTSPATH),)

flash: $(EXECUTABLE)$(EXTENTION) $(EXECUTABLE).lst
	@if ps ax -o comm | grep -q openocd ; then echo openocd already running. quit existing first. ; exit 1 ; fi
	$(GDB) $< -ex "target remote | $(OPENOCDPATH)/openocd -c \"gdb_port pipe\" --search $(OPENOCDSCRIPTSPATH) $(OPENOCDARGS)" -ex "monitor reset halt" -ex "load" -ex "monitor reset init" -ex "monitor reset run"  -ex "detach" -ex "quit"

gdb:
	@if ps ax -o comm | grep -q openocd ; then echo openocd already running. quit existing first. ; exit 1 ; fi
	$(GDB) $(EXECUTABLE)$(EXTENTION) -ex "target remote | $(OPENOCDPATH)/openocd -c \"gdb_port pipe\" --search $(OPENOCDSCRIPTSPATH) $(OPENOCDARGS)" -ex "monitor reset halt"

else

flash gdb:
	echo OPENOCD not found ; exit 1

endif
