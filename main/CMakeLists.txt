    if (${IDF_TARGET} STREQUAL "linux")
        FILE(GLOB_RECURSE app_sources
                ${CMAKE_SOURCE_DIR}/main/main.cpp
        )
        idf_component_register(SRCS ${app_sources} INCLUDE_DIRS "." REQUIRES core-esp32)
    else ()
        FILE(GLOB_RECURSE app_sources
                ${CMAKE_SOURCE_DIR}/main/*.cpp
        )
        idf_component_register(SRCS ${app_sources} INCLUDE_DIRS "." REQUIRES core-esp32 bluetooth)
    endif ()

    spiffs_create_partition_image(storage ../data)

    if (${IDF_TARGET} STREQUAL "linux")
        file(COPY ${CMAKE_SOURCE_DIR}/data/config.json DESTINATION ${PROJECT_BINARY_DIR}/../)
    endif ()


