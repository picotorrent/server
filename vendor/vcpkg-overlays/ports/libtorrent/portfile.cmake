vcpkg_fail_port_install(ON_TARGET "uwp")

if(VCPKG_TARGET_IS_WINDOWS)
    if(VCPKG_CRT_LINKAGE STREQUAL "static")
        set(_static_runtime ON)
    endif()
endif()

vcpkg_check_features(
        OUT_FEATURE_OPTIONS FEATURE_OPTIONS
        FEATURES
        deprfun     deprecated-functions
        examples    build_examples
        test        build_tests
        tools       build_tools
)

vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO arvidn/libtorrent
        REF 3c20db6a1e0ed2d57401aa7fdf82c9c5a3cdc84d # v2.0.5
        SHA512 750baab503e36bda612b01c5cfee974eb98c65bdfc81831d814adcac87c8502c3550b0f5cc76ddd567ba2a38caa4cbb433da319e6a51c1918b6cba9a9ffa6504
        HEAD_REF RC_2_0
)

vcpkg_from_github(
        OUT_SOURCE_PATH TRYSIGNAL_SOURCE_PATH
        REPO arvidn/try_signal
        REF 334fd139e2bb387017b42d36753a03935e3bca75
        SHA512 a25d439b2d979e975f9dd125a34072f70bfc7a08fab950e3829130742c05c584ae88d9f58fc0f1b4fa0b51df2c0e32c5b24c5828d53b121b4bc183a4c68d6a5a
        HEAD_REF master
)

file(
        COPY
        ${TRYSIGNAL_SOURCE_PATH}/signal_error_code.cpp
        ${TRYSIGNAL_SOURCE_PATH}/signal_error_code.hpp
        ${TRYSIGNAL_SOURCE_PATH}/try_signal.cpp
        ${TRYSIGNAL_SOURCE_PATH}/try_signal.hpp
        ${TRYSIGNAL_SOURCE_PATH}/try_signal_mingw.hpp
        ${TRYSIGNAL_SOURCE_PATH}/try_signal_msvc.hpp
        ${TRYSIGNAL_SOURCE_PATH}/try_signal_posix.hpp
        DESTINATION ${SOURCE_PATH}/deps/try_signal)

vcpkg_configure_cmake(
        SOURCE_PATH ${SOURCE_PATH}
        PREFER_NINJA
        OPTIONS
        ${FEATURE_OPTIONS}
        -Dstatic_runtime=${_static_runtime}
)

vcpkg_install_cmake()

vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/LibtorrentRasterbar TARGET_PATH share/LibtorrentRasterbar)

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

# Do not duplicate include files
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include ${CURRENT_PACKAGES_DIR}/debug/share ${CURRENT_PACKAGES_DIR}/share/cmake)

vcpkg_fixup_pkgconfig()
