TOOLPATH ?= $(shell \
sh -c "if [ -d /usr/include/linux ]; then echo /usr/bin; \
      else echo; fi" \
)

# Get the $(CFLAGSENV), $(CXXFLAGSENV), $(LDFLAGSENV)
include $(OPENMRNPATH)/etc/env.mk

CC = gcc
CXX = g++
AR = ar
LD = g++

CFLAGS = -c -g -O0 -Wall -Werror -MD -MP -std=gnu99 -m32 -fno-stack-protector \
         -D_GNU_SOURCE
CXXFLAGS = -c -g -O0 -Wall -Werror -MD -MP -m32 -fno-stack-protector \
           -D_GNU_SOURCE -D__STDC_FORMAT_MACROS

LDFLAGS = -g -m32
SYSLIBRARIES = -lrt -lpthread

EXTENTION =

