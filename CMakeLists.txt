cmake_minimum_required(VERSION 3.16)

project(PicoTorrentServer)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# if we run OSX, set the OPENSSL_ROOT_DIR to the one where homebrew installed
# openssl, otherwise CMake will throw an err.
if(APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()

# set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_CXX_STANDARD 17)
# set(OPENSSL_USE_STATIC_LIBS TRUE)

find_package(Boost REQUIRED COMPONENTS system log)
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

# --------- libtorrent options
option(BUILD_SHARED_LIBS    "" off)
option(deprecated-functions "" off)
# ----------------------------

# --------- nlohmann-json options
option(JSON_BuildTests "enables tests" OFF)
set(JSON_BuildTests OFF)
# -------------------------------

add_subdirectory(vendor/libtorrent)
add_subdirectory(vendor/nlohmann-json)

add_executable(
    PicoTorrentServer
    src/database
    src/log
    src/main
    src/options
    src/sessionmanager

    # data
    src/data/transaction
    src/data/models/profile
    src/data/models/settingspack
    src/data/models/settingspacknames

    # migrations
    src/migrations/0001_initialsetup

    # http
    src/http/httplistener
    src/http/httpsession
    src/http/websocketsession

    # rpc
    src/rpc/configget
    src/rpc/configset
    src/rpc/listeninterfacesgetall
    src/rpc/profileslist
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
    PicoTorrentServer
    PRIVATE
    -DTORRENT_NO_DEPRECATE
    -DTORRENT_USE_LIBCRYPTO
    -DTORRENT_USE_OPENSSL
)

target_include_directories(
    PicoTorrentServer
    PRIVATE
    vendor/libtorrent
)

target_link_libraries(
    PicoTorrentServer
    ${Boost_LIBRARIES}
    nlohmann_json::nlohmann_json
    sqlite3
    "torrent-rasterbar"
)
