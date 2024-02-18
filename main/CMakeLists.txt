    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")

    if (${IDF_TARGET} STREQUAL "linux")

        FILE(GLOB_RECURSE app_sources
                ${CMAKE_SOURCE_DIR}/main/main.cpp
                ${CMAKE_SOURCE_DIR}/main/LinuxHidGamepad.cpp
                ${CMAKE_SOURCE_DIR}/main/v2/*.cpp
        )

        idf_component_register(SRCS ${app_sources} INCLUDE_DIRS "." REQUIRES core-esp32 extra-linux)
        include_directories(
                /opt/homebrew/include
        )
    else ()
        FILE(GLOB_RECURSE app_sources
                ${CMAKE_SOURCE_DIR}/main/*.cpp
        )
        idf_component_register(SRCS ${app_sources} INCLUDE_DIRS "." REQUIRES core-esp32 bluetooth)

        spiffs_create_partition_image(storage ../data)
    endif ()

    if (${IDF_TARGET} STREQUAL "linux")
        file(COPY ${CMAKE_SOURCE_DIR}/data/config.json DESTINATION ${PROJECT_BINARY_DIR}/../)
    endif ()
