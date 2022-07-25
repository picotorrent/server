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
    pikajsonrpc
    STATIC
    src/libpika/jsonrpc/src/jsonrpcserver.cpp
)

target_include_directories(
    pikajsonrpc
    PUBLIC
    src/libpika/jsonrpc/include
)

target_link_libraries(
    pikajsonrpc
    Boost::boost
    nlohmann_json::nlohmann_json
)

add_library(
    pikahttp
    STATIC
    src/libpika/http/src/httpsession.cpp
    src/libpika/http/src/httpserver.cpp
)

target_include_directories(
    pikahttp
    PUBLIC
    src/libpika/http/include
)

target_link_libraries(
    pikahttp
    Boost::boost
    nlohmann_json::nlohmann_json
)

add_library(
    pikacore
    STATIC

    src/config.cpp
    src/config.hpp
    src/log.cpp
    src/session.cpp
    src/session.hpp
    src/torrenthandle.hpp

    # data
    src/data/migrator.cpp
    src/data/migrator.hpp
    src/data/migrations/0001_initialsetup.cpp
    src/data/migrations/0002_labels.cpp
    src/data/migrations/0002_labels.hpp
    src/data/statement.cpp
    src/data/transaction.cpp
    src/data/models/addtorrentparams.cpp
    src/data/models/addtorrentparams.hpp
    src/data/models/config.cpp
    src/data/models/config.hpp
    src/data/models/labels.cpp
    src/data/models/labels.hpp
    src/data/models/sessionparams.cpp
    src/data/models/sessionparams.hpp

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
    pikacore
    pikajsonrpc
    pikahttp

    Boost::boost
    Boost::log
    Boost::program_options
    nlohmann_json::nlohmann_json
    LibtorrentRasterbar::torrent-rasterbar
    unofficial::sqlite3::sqlite3
    tomlplusplus::tomlplusplus
)

add_executable(
    pika
    src/main.cpp
)

target_link_libraries(
    pika
    pikacore
    pikajsonrpc
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
