# Parent scope still has the core project name (e.g. lh); this file's project() overwrites PROJECT_NAME.
set(LH_CORE_TARGET ${PROJECT_NAME})
project(${LH_CORE_TARGET}_test)

# Paths for bundled test dependencies and test sources.
set(LH_TEST_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(LH_TEST_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LH_TEST_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Skip test targets if googletest submodule is not initialized.
if (NOT EXISTS "${LH_TEST_LIB_PATH}/googletest/CMakeLists.txt")
    message(WARNING
            "googletest not found — tests disabled.\n"
            "Run: git submodule update --init --recursive")
    return()
endif ()

# Keep GoogleTest CRT consistent with this project on MSVC (/MD vs /MT).
if (MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif ()

# Add GoogleTest from the local submodule.
add_subdirectory(${LH_TEST_LIB_PATH}/googletest)

# Test runner sources.
set(LH_TEST_SOURCES
        ${LH_TEST_SRC_PATH}/main.cpp
        ${LH_TEST_SRC_PATH}/lh/foreach.cpp
        ${LH_TEST_SRC_PATH}/lh/char/digit.cpp
        ${LH_TEST_SRC_PATH}/lh/char/dot.cpp
        ${LH_TEST_SRC_PATH}/lh/char/letter.cpp
        ${LH_TEST_SRC_PATH}/lh/char/slash.cpp
        ${LH_TEST_SRC_PATH}/lh/char/xdigit.cpp
        ${LH_TEST_SRC_PATH}/lh/error.cpp
        ${LH_TEST_SRC_PATH}/lh/werror.cpp
        ${LH_TEST_SRC_PATH}/lh/runtime/error.cpp
        ${LH_TEST_SRC_PATH}/lh/runtime/check.cpp
        ${LH_TEST_SRC_PATH}/lh/exception.cpp
        ${LH_TEST_SRC_PATH}/lh/version.cpp
        ${LH_TEST_SRC_PATH}/lh/date.cpp
        ${LH_TEST_SRC_PATH}/lh/time.cpp
        ${LH_TEST_SRC_PATH}/lh/time/second.cpp
        ${LH_TEST_SRC_PATH}/lh/time/minute.cpp
        ${LH_TEST_SRC_PATH}/lh/time/hour.cpp
        ${LH_TEST_SRC_PATH}/lh/datetime.cpp
        ${LH_TEST_SRC_PATH}/lh/timestamp.cpp
        ${LH_TEST_SRC_PATH}/lh/cpu/simd.cpp
        ${LH_TEST_SRC_PATH}/lh/cpu/vendor.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/allocator.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/std.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/memory.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/raw.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/bounds.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/typed.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/view.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/bounds/slice.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/bounds/allocated.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/view/slice.cpp
        ${LH_TEST_SRC_PATH}/lh/vector.cpp
        ${LH_TEST_SRC_PATH}/lh/str.cpp
        ${LH_TEST_SRC_PATH}/lh/str/list.cpp
        ${LH_TEST_SRC_PATH}/lh/wstr.cpp
        ${LH_TEST_SRC_PATH}/lh/wstr/list.cpp
        ${LH_TEST_SRC_PATH}/lh/util/swap.cpp
        ${LH_TEST_SRC_PATH}/lh/util/bit/endian.cpp
        ${LH_TEST_SRC_PATH}/lh/util/bit/bswap.cpp
        ${LH_TEST_SRC_PATH}/lh/util/bit/rotate.cpp
        ${LH_TEST_SRC_PATH}/lh/util/bit/scan.cpp
        ${LH_TEST_SRC_PATH}/lh/util/bit/half.cpp
        ${LH_TEST_SRC_PATH}/lh/util/interval.cpp
        ${LH_TEST_SRC_PATH}/lh/util/math.cpp
        ${LH_TEST_SRC_PATH}/lh/util/memory.cpp
        ${LH_TEST_SRC_PATH}/lh/str/view.cpp
        ${LH_TEST_SRC_PATH}/lh/fs/path/style.cpp
        ${LH_TEST_SRC_PATH}/lh/wstr/view.cpp
        ${LH_TEST_SRC_PATH}/lh/util/str/ptr.cpp
        ${LH_TEST_SRC_PATH}/lh/util/str/ptr/dot.cpp
        ${LH_TEST_SRC_PATH}/lh/util/str/case/map.cpp
        ${LH_TEST_SRC_PATH}/lh/util/wstr/ptr.cpp
        ${LH_TEST_SRC_PATH}/lh/wchar/case/map.cpp
        ${LH_TEST_SRC_PATH}/lh/str/parse/uint.cpp
        ${LH_TEST_SRC_PATH}/lh/str/parse/hex.cpp
        ${LH_TEST_SRC_PATH}/lh/str/parse/text.cpp
        ${LH_TEST_SRC_PATH}/lh/str/scanf/next.cpp
        ${LH_TEST_SRC_PATH}/lh/str/split/next.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/uint.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/sint.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/hex.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/bytes.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/text.cpp
        ${LH_TEST_SRC_PATH}/lh/crypto/sha.cpp
        ${LH_TEST_SRC_PATH}/lh/crypto/rijndael.cpp
        ${LH_TEST_SRC_PATH}/lh/net/ip.cpp
        ${LH_TEST_SRC_PATH}/lh/net/port.cpp
        ${LH_TEST_SRC_PATH}/lh/net/socket/addr/ip4.cpp
        ${LH_TEST_SRC_PATH}/lh/net/socket/addr.cpp
        ${LH_TEST_SRC_PATH}/lh/io/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/io/writer.cpp
        ${LH_TEST_SRC_PATH}/lh/io/stream.cpp
        ${LH_TEST_SRC_PATH}/lh/io/dgram.cpp
        ${LH_TEST_SRC_PATH}/lh/logger.cpp
)

if (LH_LIBRARY_OPTION_OS)
    list(APPEND LH_TEST_SOURCES
            # os is being rebuilt from scratch, one piece at a time.
    )
endif ()

# Build the test executable.
add_executable(${PROJECT_NAME} ${LH_TEST_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${LH_TEST_INCLUDE_PATH}")

# Link library under test and GoogleTest.
target_link_libraries(${PROJECT_NAME}
        PRIVATE ${LH_CORE_TARGET} gtest
)

# Register the executable in CTest (enable_testing() is in the root CMakeLists.txt).
add_test(
        NAME ${PROJECT_NAME}
        COMMAND "$<TARGET_FILE:${PROJECT_NAME}>"
)
