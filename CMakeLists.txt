cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

cmake_policy(SET CMP0092 NEW) # don't add /W3 as default

set(CMAKE_TOOLCHAIN_FILE   ${CMAKE_CURRENT_SOURCE_DIR}/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(pika)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost                      REQUIRED COMPONENTS log program_options system)
find_package(GTest               CONFIG REQUIRED)
find_package(LibtorrentRasterbar CONFIG REQUIRED)
find_package(nlohmann_json       CONFIG REQUIRED)
find_package(tomlplusplus        CONFIG REQUIRED)
find_package(unofficial-sqlite3  CONFIG REQUIRED)

add_library(
    pikacore
    STATIC
    src/libpika/src/bittorrent/session.cpp
    src/libpika/src/bittorrent/_aux/models/addtorrentparams.cpp
    src/libpika/src/bittorrent/_aux/models/addtorrentparams.hpp
    src/libpika/src/bittorrent/_aux/models/labels.cpp
    src/libpika/src/bittorrent/_aux/models/labels.hpp
    src/libpika/src/bittorrent/_aux/models/sessionparams.cpp
    src/libpika/src/bittorrent/_aux/models/sessionparams.hpp
    src/libpika/src/bittorrent/_aux/torrenthandle.cpp
    src/libpika/src/bittorrent/_aux/torrenthandle.hpp

    src/libpika/src/data/_aux/row.cpp
    src/libpika/src/data/_aux/row.hpp
    src/libpika/src/data/_aux/statement.cpp
    src/libpika/src/data/_aux/statement.hpp
    src/libpika/src/data/_aux/transaction.cpp
    src/libpika/src/data/_aux/transaction.hpp
    src/libpika/src/data/database.cpp
    src/libpika/src/data/migrator.cpp

    src/libpika/src/jsonrpc/jsonrpcserver.cpp

    src/libpika/src/http/httpsession.cpp
    src/libpika/src/http/httpserver.cpp
)

target_include_directories(
    pikacore
    PUBLIC
    src/libpika/include
)

target_link_libraries(
    pikacore
    PUBLIC
    Boost::boost
    nlohmann_json::nlohmann_json
    LibtorrentRasterbar::torrent-rasterbar
    unofficial::sqlite3::sqlite3
)

add_executable(
    pika
    src/main.cpp

    src/config.cpp
    src/config.hpp
    src/log.cpp

    # data
    src/data/migrations/0001_initialsetup.cpp
    src/data/migrations/0002_labels.cpp
    src/data/migrations/0002_labels.hpp
    src/data/models/config.cpp
    src/data/models/config.hpp

    # http handler
    src/http/handlers/eventshandler.cpp
    src/http/handlers/eventshandler.hpp
    src/http/handlers/jsonrpchandler.cpp
    src/http/handlers/jsonrpchandler.hpp

    # json
    src/json/torrenthandle.hpp

    # rpc
    src/rpc/configget.cpp
    src/rpc/configset.cpp
    src/rpc/sessionaddtorrent.cpp
    src/rpc/sessiongettorrents.cpp
    src/rpc/sessiongettorrents.hpp
    src/rpc/sessionfindtorrents.cpp
    src/rpc/sessionfindtorrents.hpp
    src/rpc/sessionremovetorrent.cpp
    src/rpc/sessionstats.cpp
    src/rpc/sessionstats.hpp
    src/rpc/torrentsfilesget.cpp
    src/rpc/torrentsfilesget.hpp
    src/rpc/torrentslabelsget.cpp
    src/rpc/torrentslabelsget.hpp
    src/rpc/torrentsmovestorage.cpp
    src/rpc/torrentspause.cpp
    src/rpc/torrentsresume.cpp
    src/rpc/torrentssetlabels.cpp
    src/rpc/torrentssetlabels.hpp
)

target_link_libraries(
    pika
    PRIVATE
    Boost::log
    Boost::program_options
    pikacore
    tomlplusplus::tomlplusplus
)

add_executable(
    pikatests
    tests/main.cpp
    tests/data/models.cpp
    tests/jsonrpc/jsonrpcserver.cpp
    tests/rpc/configget.cpp
    tests/rpc/configset.cpp
    tests/rpc/torrentspause.cpp
    tests/rpc/torrentsresume.cpp
)

target_link_libraries(
    pikatests
    PRIVATE
    pikacore
    pikajsonrpc
    GTest::gmock
    GTest::gtest
    GTest::gmock_main
)
