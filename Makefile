#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := tft_demo

EXTRA_CFLAGS += --save-temps

include $(IDF_PATH)/make/project.mk

# CFLAGS for the C compiler, CXXFLAGS for C++, and CPPFLAGS for both.

# for use with gcc 8.20
CFLAGS += \
          -Wno-error=tautological-compare \
          -Wno-tautological-compare \
          -Wno-implicit-fallthrough 

