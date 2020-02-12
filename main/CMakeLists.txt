file(GLOB SOURCES *.c)
idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS "."
                       REQUIRES tft spiffs touch)

spiffs_create_partition_image(storage ../components/spiffs_image FLASH_IN_PROJECT)
