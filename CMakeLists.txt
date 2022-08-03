cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

cmake_policy(SET CMP0092 NEW) # don't add /W3 as default

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(pika)

set(CMAKE_CXX_STANDARD 20)

find_package(LibtorrentRasterbar CONFIG REQUIRED)

# Include Node-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR)
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

add_library(
    native
    SHARED
    src/native/bindings.cpp
    src/native/session.cpp
    src/native/session.hpp
)

set_target_properties(
    native PROPERTIES PREFIX "" SUFFIX ".node"
)

target_include_directories(
    native
    PUBLIC
    ${CMAKE_JS_INC}
    ${NODE_ADDON_API_DIR}
)

target_link_libraries(
    native
    PUBLIC
    ${CMAKE_JS_LIB}
    LibtorrentRasterbar::torrent-rasterbar
)
