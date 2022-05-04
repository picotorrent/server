cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

cmake_policy(SET CMP0092 NEW) # don't add /W3 as default

set(CMAKE_TOOLCHAIN_FILE   ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(pikacore)

set(CMAKE_CXX_STANDARD 20)

#find_package(Boost                      REQUIRED COMPONENTS log program_options system)
find_package(LibtorrentRasterbar CONFIG REQUIRED)

add_library(
    pikacore
    SHARED
    src/libpika.cpp
)


target_link_libraries(
    pikacore
    LibtorrentRasterbar::torrent-rasterbar
)
