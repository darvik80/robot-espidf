if (${IDF_TARGET} STREQUAL "linux")
    FILE(GLOB_RECURSE app_sources
            ${CMAKE_SOURCE_DIR}/main/main.cpp
            ../libs/darvik/core-esp32/src/core/EventBus.cpp
            ../libs/darvik/core-esp32/src/core/Properties.cpp
    )
    idf_component_register(SRCS ${app_sources} INCLUDE_DIRS "." REQUIRES json esp_event nvs_flash spiffs)
    file(COPY ../data/config.json DESTINATION ${PROJECT_BINARY_DIR}/../)
else ()
    FILE(GLOB_RECURSE app_sources
            ${CMAKE_SOURCE_DIR}/main/main.cpp
    )
    idf_component_register(SRCS ${app_sources} INCLUDE_DIRS ".")
endif ()

spiffs_create_partition_image(storage ../data)

