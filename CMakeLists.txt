cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
cmake_policy(SET CMP0092 NEW) # don't add /W3 as default
cmake_policy(SET CMP0115 OLD) # Source file extensions must be explicit.

set(VCPKG_OVERLAY_PORTS    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg-overlays/ports)
set(CMAKE_TOOLCHAIN_FILE   ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(PicoTorrentServer)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

find_package(Boost                      REQUIRED COMPONENTS log program_options system)
find_package(Catch2              CONFIG REQUIRED)
find_package(LibtorrentRasterbar CONFIG REQUIRED)
find_package(nlohmann_json       CONFIG REQUIRED)
find_package(unofficial-sqlite3  CONFIG REQUIRED)

add_library(
    PicoTorrentCore
    STATIC

    src/database
    src/log
    src/options
    src/sessionmanager

    # data
    src/data/migrations/0001_initialsetup
    src/data/statement
    src/data/transaction
    src/data/models/listeninterface
    src/data/models/profile
    src/data/models/proxy
    src/data/models/settingspack
    src/data/models/settingspacknames

    # http
    src/http/httplistener
    src/http/httpsession
    src/http/httpsession_handlejsonrpc
    src/http/websocketsession

    # rpc
    src/rpc/configget
    src/rpc/configset
    src/rpc/listeninterfacescreate
    src/rpc/listeninterfacesgetall
    src/rpc/listeninterfacesremove
    src/rpc/profilesgetactive
    src/rpc/profilesgetall
    src/rpc/profilesupdate
    src/rpc/proxycreate
    src/rpc/proxygetall
    src/rpc/sessionaddmagnetlink
    src/rpc/sessionaddtorrent
    src/rpc/sessionremovetorrent
    src/rpc/settingspackcreate
    src/rpc/settingspackgetbyid
    src/rpc/settingspackupdate
    src/rpc/settingspacklist
    src/rpc/torrentspause
    src/rpc/torrentsresume
)

target_compile_definitions(
    PicoTorrentCore
    PRIVATE
    -DTORRENT_NO_DEPRECATE
    -DTORRENT_USE_LIBCRYPTO
    -DTORRENT_USE_OPENSSL
)

target_link_libraries(
    PicoTorrentCore
    Boost::boost
    Boost::log
    Boost::program_options
    nlohmann_json::nlohmann_json
    LibtorrentRasterbar::torrent-rasterbar
    unofficial::sqlite3::sqlite3
)

add_executable(
    PicoTorrentServer
    src/main
)

target_link_libraries(
    PicoTorrentServer
    PicoTorrentCore
)

add_executable(
    PicoTorrentTests
    tests/main
    tests/data/models
)

target_link_libraries(
    PicoTorrentTests
    PicoTorrentCore
)
