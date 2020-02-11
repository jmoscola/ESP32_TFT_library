set(SOURCES tft_demo.c)
file(TO_CMAKE_PATH "$ENV{IDF_PATH}" ENV_IDF_PATH)
idf_component_register(
        SRCS ${SOURCES}
        INCLUDE_DIRS
          ${CMAKE_CURRENT_LIST_DIR}
          ${ENV_IDF_PATH}/components
        REQUIRES
            tft
            spiffs
            touch
)
