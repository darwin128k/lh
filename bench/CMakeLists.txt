# Parent scope still has the core project name (e.g. lh); this file's project() overwrites PROJECT_NAME.
set(LH_CORE_TARGET ${PROJECT_NAME})
project(${LH_CORE_TARGET}_bench)

set(LH_BENCH_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LH_BENCH_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(LH_BENCH_SOURCES
        ${LH_BENCH_SRC_PATH}/main.cpp
        ${LH_BENCH_SRC_PATH}/lh/memory/std.cpp
        ${LH_BENCH_SRC_PATH}/lh/vector.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/format/uint.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/format/sint.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/format/hex.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/format/text.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/parse/uint.cpp
        ${LH_BENCH_SRC_PATH}/lh/net/ip.cpp
        ${LH_BENCH_SRC_PATH}/lh/io/stream.cpp
)

add_executable(${PROJECT_NAME} ${LH_BENCH_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${LH_BENCH_INCLUDE_PATH}")

target_link_libraries(${PROJECT_NAME} PRIVATE ${LH_CORE_TARGET})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# Windows: put the shared library next to the bench executable so the loader finds it.
if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${LH_CORE_TARGET}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif ()
