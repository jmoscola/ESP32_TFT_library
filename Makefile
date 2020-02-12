#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := tft_demo

EXTRA_CFLAGS += --save-temps

include $(IDF_PATH)/make/project.mk

# CFLAGS for the C compiler, CXXFLAGS for C++, and CPPFLAGS for both.

CFLAGS += \
          -Wno-unused-function \
          -Wno-unused-variable \
          -Wno-unused-but-set-variable

# for use with gcc 8.20
CFLAGS += \
          -Wno-error=tautological-compare \
          -Wno-tautological-compare \
          -Wno-implicit-fallthrough 

# Create a SPIFFS image from the contents of the 'spiffs_image' directory
# that fits the partition named 'storage'. FLASH_IN_PROJECT indicates that
# the generated image should be flashed when the entire project is flashed to
# the target with 'make flash'. 
SPIFFS_IMAGE_FLASH_IN_PROJECT := 1
$(eval $(call spiffs_create_partition_image,storage,./components/spiffs_image/image/images))
