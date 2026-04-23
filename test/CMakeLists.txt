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
        ${LH_TEST_SRC_PATH}/lh/error.cpp
        ${LH_TEST_SRC_PATH}/lh/version.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/allocator.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/std.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/memory.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/raw.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/range.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/view.cpp
        ${LH_TEST_SRC_PATH}/lh/util/interval.cpp
        ${LH_TEST_SRC_PATH}/lh/util/str/raw.cpp
        ${LH_TEST_SRC_PATH}/lh/util/str/case/map.cpp
        ${LH_TEST_SRC_PATH}/lh/util/wstr/raw.cpp
        ${LH_TEST_SRC_PATH}/lh/wchar/case/map.cpp
)

# Build the test executable.
add_executable(${PROJECT_NAME} ${LH_TEST_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${LH_TEST_INCLUDE_PATH}")

# Link library under test and GoogleTest.
target_link_libraries(${PROJECT_NAME}
        PRIVATE ${LH_CORE_TARGET} gtest
)

# Windows: put the shared library next to the test executable so the loader finds it (avoids
# 0xC0000135 STATUS_DLL_NOT_FOUND when running lh_test.exe from the IDE or arbitrary cwd).
if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${LH_CORE_TARGET}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "[----] Copy ${LH_CORE_TARGET} library next to ${PROJECT_NAME}"
    )
endif ()

# Register the executable in CTest (enable_testing() is in the root CMakeLists.txt).
add_test(
        NAME ${PROJECT_NAME}
        COMMAND "$<TARGET_FILE:${PROJECT_NAME}>"
)
