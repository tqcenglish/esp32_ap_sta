idf_component_register(SRCS "wifi_main.c"
                            "rest_server.c"
                    INCLUDE_DIRS ".")

set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
spiffs_create_partition_image(www ${WEB_SRC_DIR}/dist FLASH_IN_PROJECT)