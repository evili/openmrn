OPENMRNPATH ?= $(shell \
sh -c "if [ \"X`printenv OPENMRNPATH`\" != \"X\" ]; then printenv OPENMRNPATH; \
     elif [ -d /opt/openmrn/src ]; then echo /opt/openmrn; \
     elif [ -d ~/openmrn/src ]; then echo ~/openmrn; \
     elif [ -d ~/train/openlcb/openmrn/src ]; then echo ~/train/openlcb/openmrn; \
     elif [ -d ../../../src ]; then echo ../../..; \
     else echo OPENMRNPATH not found; fi" \
)

##TARGET##

# Find Mbed
include $(OPENMRNPATH)/etc/mbed.mk

ifndef HAVE_MBED

all docs clean veryclean:
	@echo "******************************************************************"
	@echo "*"
	@echo "*   Unable to build for $(TARGET), mbed unavailable"
	@echo "*"
	@echo "******************************************************************"

else

OBJEXTRA = $(OPENMRNPATH)/targets/freertos.armv4t/freertos_drivers/mbed_can.o \
	$(OPENMRNPATH)/targets/freertos.armv4t/freertos_drivers/mbed_usbserial.o

include $(OPENMRNPATH)/etc/prog.mk

all: $(EXECUTABLE).bin $(EXECUTABLE).lst

%.lst: %.elf
	$(SIZE) $<
	$(OBJDUMP) -d $< > $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
	$(CHECKSUM) -p LPC2362 -d $@

$(EXECUTABLE)$(EXTENTION): depmake

.PHONY: depmake

depmake:
	make -C $(OPENMRNPATH)/targets/$(TARGET) all

endif
