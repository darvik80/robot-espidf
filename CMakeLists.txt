# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

include_directories(
        ${CMAKE_SOURCE_DIR}/main
)

if (${IDF_TARGET} STREQUAL "linux")
        set(COMPONENTS main)
        #list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/tools/mocks/esp_wifi/)
endif()

list(APPEND EXTRA_COMPONENT_DIRS libs/darvik/core-esp32/)
list(APPEND EXTRA_COMPONENT_DIRS libs/darvik/bluetooth/)

project(robot-espidf)
