cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

cmake_policy(SET CMP0092 NEW) # don't add /W3 as default

set(VCPKG_OVERLAY_PORTS    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg-overlays/ports)
set(VCPKG_OVERLAY_TRIPLETS ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg-overlays/triplets)
set(CMAKE_TOOLCHAIN_FILE   ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(PicoTorrentServer)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost                      REQUIRED COMPONENTS log program_options system)
find_package(GTest               CONFIG REQUIRED)
find_package(LibtorrentRasterbar CONFIG REQUIRED)
find_package(nlohmann_json       CONFIG REQUIRED)
find_package(unofficial-sqlite3  CONFIG REQUIRED)

add_library(
    PicoTorrentCore
    STATIC

    src/database.cpp
    src/log.cpp
    src/options.cpp
    src/sessionmanager.cpp

    # data
    src/data/migrations/0001_initialsetup.cpp
    src/data/statement.cpp
    src/data/transaction.cpp
    src/data/models/config.cpp
    src/data/models/listeninterface.cpp
    src/data/models/profile.cpp
    src/data/models/proxy.cpp
    src/data/models/settingspack.cpp
    src/data/models/settingspacknames.cpp

    # http
    src/http/httplistener.cpp
    src/http/httpsession.cpp

    # http handlers
    src/http/handlers/jsonrpchandler.cpp
    src/http/handlers/jsonrpchandler.hpp
    src/http/handlers/websockethandler.cpp
    src/http/handlers/websockethandler.hpp

    # rpc
    src/rpc/configget.cpp
    src/rpc/configset.cpp
    src/rpc/listeninterfacescreate.cpp
    src/rpc/listeninterfacesgetall.cpp
    src/rpc/listeninterfacesremove.cpp
    src/rpc/profilesgetactive.cpp
    src/rpc/profilesgetall.cpp
    src/rpc/profilesupdate.cpp
    src/rpc/proxycreate.cpp
    src/rpc/proxygetall.cpp
    src/rpc/sessionaddmagnetlink.cpp
    src/rpc/sessionaddtorrent.cpp
    src/rpc/sessionremovetorrent.cpp
    src/rpc/settingspackcreate.cpp
    src/rpc/settingspackgetbyid.cpp
    src/rpc/settingspackupdate.cpp
    src/rpc/settingspacklist.cpp
    src/rpc/torrentspause.cpp
    src/rpc/torrentsresume.cpp

    # tsdb
    src/tsdb/influxdb.cpp
    src/tsdb/influxdb.hpp
    src/tsdb/prometheus.cpp
    src/tsdb/prometheus.hpp
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
    src/main.cpp
)

target_link_libraries(
    PicoTorrentServer
    PicoTorrentCore
)

add_executable(
    PicoTorrentTests
    tests/main.cpp
    tests/data/models.cpp
    tests/rpc/configget.cpp
    tests/rpc/configset.cpp
    tests/rpc/torrentspause.cpp
    tests/rpc/torrentsresume.cpp
)

target_link_libraries(
    PicoTorrentTests
    PRIVATE
    PicoTorrentCore
    GTest::gmock GTest::gtest GTest::gmock_main
)
