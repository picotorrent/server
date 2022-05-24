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
    duktape
    STATIC
    vendor/duktape/src/duktape.c
    vendor/duktape/src/duktape.h
)

target_include_directories(
    duktape
    INTERFACE
    vendor/duktape/src
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
    src/data/statement.cpp
    src/data/transaction.cpp
    src/data/models/addtorrentparams.cpp
    src/data/models/addtorrentparams.hpp
    src/data/models/config.cpp
    src/data/models/config.hpp
    src/data/models/sessionparams.cpp
    src/data/models/sessionparams.hpp

    # http
    src/http/httplistener.cpp
    src/http/httpsession.cpp

    # http handler
    src/http/handlers/eventshandler.cpp
    src/http/handlers/eventshandler.hpp
    src/http/handlers/jsonrpchandler.cpp
    src/http/handlers/jsonrpchandler.hpp
    src/http/handlers/webroothandler.cpp
    src/http/handlers/webroothandler.hpp

    # rpc
    src/rpc/configget.cpp
    src/rpc/configset.cpp
    src/rpc/sessionaddtorrent.cpp
    src/rpc/sessionlisttorrents.cpp
    src/rpc/sessionlisttorrents.hpp
    src/rpc/sessionremovetorrent.cpp
    src/rpc/torrentsmovestorage.cpp
    src/rpc/torrentspause.cpp
    src/rpc/torrentsresume.cpp

    # plugins
    src/plugins/modules/configmodule.cpp
    src/plugins/modules/configmodule.hpp
    src/plugins/modules/httpmodule.cpp
    src/plugins/modules/httpmodule.hpp
    src/plugins/modules/loggermodule.cpp
    src/plugins/modules/loggermodule.hpp
    src/plugins/pluginfactory.cpp
    src/plugins/pluginfactory.hpp
)

target_link_libraries(
    pikacore
    Boost::boost
    Boost::log
    Boost::program_options
    duktape
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
)

add_executable(
    pikatests
    tests/main.cpp
    tests/data/models.cpp
    tests/rpc/configget.cpp
    tests/rpc/configset.cpp
    tests/rpc/torrentspause.cpp
    tests/rpc/torrentsresume.cpp
)

target_link_libraries(
    pikatests
    PRIVATE
    pikacore
    GTest::gmock
    GTest::gtest
    GTest::gmock_main
)
