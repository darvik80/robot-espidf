FILE(GLOB_RECURSE app_sources
        ${CMAKE_SOURCE_DIR}/main/*.*
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")

idf_component_register(SRCS ${app_sources} INCLUDE_DIRS ".")

spiffs_create_partition_image(storage ../data)