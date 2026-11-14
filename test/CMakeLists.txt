# Parent scope still has the core project name (e.g. lh); this file's project() overwrites PROJECT_NAME.
set(LH_CORE_TARGET ${PROJECT_NAME})
project(${LH_CORE_TARGET}_test)

# Paths for bundled test dependencies and test sources.
set(LH_TEST_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(LH_TEST_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LH_TEST_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Test builds require the dependency already pinned by LH's submodule.
if (NOT EXISTS "${LH_TEST_LIB_PATH}/googletest/CMakeLists.txt")
    message(FATAL_ERROR
            "GoogleTest is required with LH_BUILD_TESTS=ON. "
            "Run git submodule update --init --recursive test/lib/googletest in the LH tree.")
endif ()

# Keep GoogleTest CRT consistent with this project on MSVC (/MD vs /MT).
if (MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif ()

# Add GoogleTest from the local submodule.
add_subdirectory(${LH_TEST_LIB_PATH}/googletest)

# One executable per core, not one for everything.
#
# The list used to be a single block naming every test file the library has, and
# that is how test/src/lh/core/math/rescale.cpp came to be in the tree with two tests
# in it and in no build at all: a file nobody ran, and nothing said so. The cores
# below are named the way the library is cut, and the guard at the end of this
# file turns "a test file nobody runs" from a silent hole into a configure error.
set(LH_TEST_RUNNER ${LH_TEST_SRC_PATH}/main.cpp)

# Test files that this configuration does not build, because the option that
# carries them is off. The guard below allows exactly these and nothing else.
set(LH_TEST_OPTION_OFF_SOURCES)

# The core library is built with IPO (LH_IPO_SUPPORTED, root CMakeLists.txt): its
# archive holds IR, so every test executable must be linked with IPO as well.
set(LH_TEST_IPO FALSE)
if (LH_IPO_SUPPORTED)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LH_TEST_IPO_SUPPORTED LANGUAGES CXX)
    set(LH_TEST_IPO ${LH_TEST_IPO_SUPPORTED})
endif ()

# lh_add_test_core(<name> [LIBRARY <target>] <sources...>): one executable, linked like the one it
# replaces, and registered with CTest under the bare core name. It also records
# what it was given, which is what the guard at the end of this file reads.
function(lh_add_test_core name)
    cmake_parse_arguments(LH_TEST "" "LIBRARY" "" ${ARGN})
    if (LH_TEST_LIBRARY)
        set(LH_TEST_LIBRARY_TARGET ${LH_TEST_LIBRARY})
    else ()
        set(LH_TEST_LIBRARY_TARGET ${LH_CORE_TARGET})
    endif ()
    add_executable(${name} ${LH_TEST_RUNNER} ${LH_TEST_UNPARSED_ARGUMENTS})
    target_compile_features(${name} PRIVATE cxx_std_17)
    if (LH_TEST_IPO)
        set_property(TARGET ${name} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    endif ()
    target_include_directories(${name} PRIVATE "${LH_TEST_INCLUDE_PATH}")
    target_link_libraries(${name} PRIVATE ${LH_TEST_LIBRARY_TARGET} gtest)
    add_test(NAME ${name} COMMAND $<TARGET_FILE:${name}>)
    set(LH_TEST_LISTED_SOURCES ${LH_TEST_LISTED_SOURCES} ${LH_TEST_UNPARSED_ARGUMENTS} PARENT_SCOPE)
endfunction()

# The path decides the core: a file under util/str belongs to util even though it
# is about strings, and one under wchar belongs to str. Anything else makes the
# guard at the end unhappy, which is the point of the guard.
set(LH_TEST_LISTED_SOURCES)

lh_add_test_core(lh_test_http_parser ${LH_TEST_SRC_PATH}/lh/core/http/parser.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/header/name.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/format.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/framing.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/expect.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/body/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/content/encoding.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/chunked.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/chunked/decoder.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/cursor.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/method.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/status.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/request.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/target.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/char.cpp ${LH_TEST_SRC_PATH}/lh/core/http/version.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/forwarded.cpp
        ${LH_TEST_SRC_PATH}/lh/core/http/limit.cpp)

# The event bus modules talk through: registry, subscribers, deferred removal.
if (LH_LIBRARY_OPTION_OS)
    lh_add_test_core(lh_test_events ${LH_TEST_SRC_PATH}/lh/os/module/event/bus.cpp
                     ${LH_TEST_SRC_PATH}/lh/os/module/hook.cpp)
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/module/event/bus.cpp
         ${LH_TEST_SRC_PATH}/lh/os/module/hook.cpp)
endif ()

# rail: tasks, the queue that carries them between threads, the owner's
# mailbox, the executor interface, the inline executor, the thread context, the
# base a platform gives rail and its slot, and (on the OS base) the thread
# executor and the loop.
set(LH_TEST_RAIL_SOURCES
        ${LH_TEST_SRC_PATH}/lh/rail/bare.cpp
        ${LH_TEST_SRC_PATH}/lh/rail/base.cpp
        ${LH_TEST_SRC_PATH}/lh/rail/runtime/base.cpp
        ${LH_TEST_SRC_PATH}/lh/rail/task.cpp
        ${LH_TEST_SRC_PATH}/lh/rail/queue.cpp
        ${LH_TEST_SRC_PATH}/lh/rail/mailbox.cpp
        ${LH_TEST_SRC_PATH}/lh/rail/executor.cpp
        ${LH_TEST_SRC_PATH}/lh/rail/inline.cpp
        ${LH_TEST_SRC_PATH}/lh/rail/context.cpp)
if (LH_LIBRARY_OPTION_OS)
    list(APPEND LH_TEST_RAIL_SOURCES ${LH_TEST_SRC_PATH}/lh/rail/thread.cpp
            ${LH_TEST_SRC_PATH}/lh/rail/loop.cpp)
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/rail/thread.cpp
            ${LH_TEST_SRC_PATH}/lh/rail/loop.cpp)
endif ()
lh_add_test_core(lh_test_rail ${LH_TEST_RAIL_SOURCES})

# gl: the graphics bricks. The integer point, the size that is a point, the rectangle, and the portable
# UI core on top of them: the entity class, the entity and the layer.
lh_add_test_core(lh_test_gl ${LH_TEST_SRC_PATH}/lh/gl/point.cpp
        ${LH_TEST_SRC_PATH}/lh/gl/size.cpp
        ${LH_TEST_SRC_PATH}/lh/gl/rect.cpp
        ${LH_TEST_SRC_PATH}/lh/gl/entity/class.cpp
        ${LH_TEST_SRC_PATH}/lh/gl/entity.cpp
        ${LH_TEST_SRC_PATH}/lh/gl/layer.cpp
        ${LH_TEST_SRC_PATH}/lh/gl/surface.cpp)

# Types, errors and the few pieces every core leans on.
lh_add_test_core(lh_test_console ${LH_TEST_SRC_PATH}/lh/core/console/console.cpp)

lh_add_test_core(lh_test_core
        ${LH_TEST_SRC_PATH}/lh/core/atomic/bool.cpp
        ${LH_TEST_SRC_PATH}/lh/core/atomic/ptr.cpp
        ${LH_TEST_SRC_PATH}/lh/core/atomic/uint.cpp
        ${LH_TEST_SRC_PATH}/lh/core/spsc.cpp
        ${LH_TEST_SRC_PATH}/lh/core/array.cpp
        ${LH_TEST_SRC_PATH}/lh/core/error.cpp
        ${LH_TEST_SRC_PATH}/lh/core/exception.cpp
        ${LH_TEST_SRC_PATH}/lh/core/exception/origin.cpp
        ${LH_TEST_SRC_PATH}/lh/core/foreach.cpp
        ${LH_TEST_SRC_PATH}/lh/core/list.cpp
        ${LH_TEST_SRC_PATH}/lh/core/lanes.cpp
        ${LH_TEST_SRC_PATH}/lh/core/logger.cpp
        ${LH_TEST_SRC_PATH}/lh/core/value.cpp
        ${LH_TEST_SRC_PATH}/lh/core/real.cpp
        ${LH_TEST_SRC_PATH}/lh/core/pair.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/js/name.cpp
        ${LH_TEST_SRC_PATH}/lh/core/ini/entry.cpp
        ${LH_TEST_SRC_PATH}/lh/core/ini/parser.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/json/node.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/json/minify.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/json/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/json/writer.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/attr.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/char.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/comment.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/entity.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/escape.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/name.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/node/entity.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/node.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/xml/writer.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/html/rawtext.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/html/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/html/void.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/js/op.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/js/node.cpp
        ${LH_TEST_SRC_PATH}/lh/core/web/js/writer.cpp
        ${LH_TEST_SRC_PATH}/lh/core/sql/address.cpp
        ${LH_TEST_SRC_PATH}/lh/core/sql/db.cpp
        ${LH_TEST_SRC_PATH}/lh/core/sql/mysql/query.cpp
        ${LH_TEST_SRC_PATH}/lh/core/version.cpp
        ${LH_TEST_SRC_PATH}/lh/core/werror.cpp
)

lh_add_test_core(lh_test_runtime
        ${LH_TEST_SRC_PATH}/lh/core/runtime/allocator.cpp
        ${LH_TEST_SRC_PATH}/lh/core/runtime/critical.cpp
        ${LH_TEST_SRC_PATH}/lh/core/critical.cpp
        ${LH_TEST_SRC_PATH}/lh/core/runtime/check.cpp
        ${LH_TEST_SRC_PATH}/lh/core/runtime/check/handler.cpp
        ${LH_TEST_SRC_PATH}/lh/core/runtime/error.cpp
        ${LH_TEST_SRC_PATH}/lh/core/runtime/terminate.cpp
        ${LH_TEST_SRC_PATH}/lh/core/runtime/terminate/handler.cpp
        ${LH_TEST_SRC_PATH}/lh/core/runtime/logger.cpp
        ${LH_TEST_SRC_PATH}/lh/core/runtime/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/core/runtime/writer.cpp
)

lh_add_test_core(lh_test_bit
        ${LH_TEST_SRC_PATH}/lh/core/bit/bswap.cpp
        ${LH_TEST_SRC_PATH}/lh/core/bit/endian.cpp
        ${LH_TEST_SRC_PATH}/lh/core/bit/half.cpp
        ${LH_TEST_SRC_PATH}/lh/core/bit/packed.cpp
        ${LH_TEST_SRC_PATH}/lh/core/bit/reverse.cpp
        ${LH_TEST_SRC_PATH}/lh/core/bit/rotate.cpp
        ${LH_TEST_SRC_PATH}/lh/core/bit/scan.cpp
)

lh_add_test_core(lh_test_char
        ${LH_TEST_SRC_PATH}/lh/core/char/digit.cpp
        ${LH_TEST_SRC_PATH}/lh/core/char/dot.cpp
        ${LH_TEST_SRC_PATH}/lh/core/char/word.cpp
        ${LH_TEST_SRC_PATH}/lh/core/char/ascii.cpp
        ${LH_TEST_SRC_PATH}/lh/core/char/letter.cpp
        ${LH_TEST_SRC_PATH}/lh/core/char/slash.cpp
        ${LH_TEST_SRC_PATH}/lh/core/char/xdigit.cpp
)

lh_add_test_core(lh_test_cpu
        ${LH_TEST_SRC_PATH}/lh/core/cpu/cache.cpp
        ${LH_TEST_SRC_PATH}/lh/core/cpu/id.cpp
        ${LH_TEST_SRC_PATH}/lh/core/cpu/simd.cpp
        ${LH_TEST_SRC_PATH}/lh/core/cpu/vendor.cpp
)

lh_add_test_core(lh_test_crypto
        ${LH_TEST_SRC_PATH}/lh/core/crypto/curve25519/field.cpp
        ${LH_TEST_SRC_PATH}/lh/core/crypto/ed25519.cpp
        ${LH_TEST_SRC_PATH}/lh/core/crypto/ed25519/point.cpp
        ${LH_TEST_SRC_PATH}/lh/core/crypto/ed25519/scalar.cpp
        ${LH_TEST_SRC_PATH}/lh/core/crypto/is_equal.cpp
        ${LH_TEST_SRC_PATH}/lh/core/crypto/rijndael.cpp
        ${LH_TEST_SRC_PATH}/lh/core/crypto/sha.cpp
)

lh_add_test_core(lh_test_hash
        ${LH_TEST_SRC_PATH}/lh/core/hash/fnv.cpp
        ${LH_TEST_SRC_PATH}/lh/core/hash/crc.cpp
        ${LH_TEST_SRC_PATH}/lh/core/compress/deflate.cpp
        ${LH_TEST_SRC_PATH}/lh/core/compress/deflate/encoder.cpp
        ${LH_TEST_SRC_PATH}/lh/core/compress/deflate/matcher.cpp
        ${LH_TEST_SRC_PATH}/lh/core/compress/deflate/tokens.cpp
        ${LH_TEST_SRC_PATH}/lh/core/compress/gzip.cpp
)

lh_add_test_core(lh_test_fs
        ${LH_TEST_SRC_PATH}/lh/core/fs/path.cpp
        ${LH_TEST_SRC_PATH}/lh/core/fs/path/style.cpp
)

lh_add_test_core(lh_test_io
        ${LH_TEST_SRC_PATH}/lh/core/io/dgram.cpp
        ${LH_TEST_SRC_PATH}/lh/core/io/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/core/io/stream.cpp
        ${LH_TEST_SRC_PATH}/lh/core/io/writer.cpp
)

set(LH_TEST_MATH_FPU_SOURCES
        ${LH_TEST_SRC_PATH}/lh/core/math/fpoint.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/fsize.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/frect.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/vec2.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/vec3.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/vec4.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/quat.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/mat4.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/convert.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/float/sqrt.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/float/sin_cos.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/float/round.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/float/finite.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/float/tan.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/slice.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/view.cpp
)
set(LH_TEST_MATH_SOURCES
        ${LH_TEST_SRC_PATH}/lh/core/math.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/point.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/size.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/rect.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/rescale.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/fit.cpp
        ${LH_TEST_SRC_PATH}/lh/core/math/scalar/centre.cpp
)
if (LH_LIBRARY_OPTION_MATH_FPU)
    list(APPEND LH_TEST_MATH_SOURCES ${LH_TEST_MATH_FPU_SOURCES})
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_MATH_FPU_SOURCES})
endif ()
lh_add_test_core(lh_test_math ${LH_TEST_MATH_SOURCES})

# MSVC x86 reports C1001 in overlapping_backward_copy while generating this
# test under /LTCG for XP (seen on 14.44.35207 and 14.51.36231). Not tied to
# one version: the next toolset on the runners would bring the error back.
# Keep all tests and library LTO, but compile this test translation unit
# without /GL to avoid that codegen path.
if (MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 4
        AND LH_OS_WINDOWS_VERSION STREQUAL "0x0501" AND LH_TEST_IPO)
    set_source_files_properties(${LH_TEST_SRC_PATH}/lh/core/memory/std.cpp
            PROPERTIES COMPILE_OPTIONS "/GL-")
endif ()

lh_add_test_core(lh_test_memory
        ${LH_TEST_SRC_PATH}/lh/core/memory/allocator.cpp
        ${LH_TEST_SRC_PATH}/lh/core/memory/bounds.cpp
        ${LH_TEST_SRC_PATH}/lh/core/memory/bounds/allocated.cpp
        ${LH_TEST_SRC_PATH}/lh/core/memory/memory.cpp
        ${LH_TEST_SRC_PATH}/lh/core/memory/scan_page.cpp
        ${LH_TEST_SRC_PATH}/lh/core/memory/sized/allocator.cpp
        ${LH_TEST_SRC_PATH}/lh/core/memory/std.cpp
        ${LH_TEST_SRC_PATH}/lh/core/memory/tree.cpp
        ${LH_TEST_SRC_PATH}/lh/core/memory/typed.cpp
        ${LH_TEST_SRC_PATH}/lh/core/memory/view.cpp
)

lh_add_test_core(lh_test_net
        ${LH_TEST_SRC_PATH}/lh/core/net/ip.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus/bus.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus/health.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus/image.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus/batch.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus/job.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus/job/queue.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus/link.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus/schedule.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/modbus/server.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/port.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/authority.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/cidr.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/socket/addr.cpp
        ${LH_TEST_SRC_PATH}/lh/core/net/socket/addr/ip4.cpp
)

lh_add_test_core(lh_test_numeric
        ${LH_TEST_SRC_PATH}/lh/core/numeric/big.cpp
        ${LH_TEST_SRC_PATH}/lh/core/numeric/float.cpp
        ${LH_TEST_SRC_PATH}/lh/core/numeric/mul.cpp
        ${LH_TEST_SRC_PATH}/lh/core/numeric/parse/bytes.cpp
)

set(LH_TEST_OS_SOURCES
        ${LH_TEST_SRC_PATH}/lh/os.cpp
        ${LH_TEST_SRC_PATH}/lh/os/fs/dir.cpp
        ${LH_TEST_SRC_PATH}/lh/os/fs/file.cpp
        ${LH_TEST_SRC_PATH}/lh/os/fs/path.cpp
        ${LH_TEST_SRC_PATH}/lh/os/input.cpp
        ${LH_TEST_SRC_PATH}/lh/os/memory.cpp
        ${LH_TEST_SRC_PATH}/lh/os/net/interface.cpp
        ${LH_TEST_SRC_PATH}/lh/os/net/modbus_emu.cpp
        ${LH_TEST_SRC_PATH}/lh/os/net/modbus_tcp.cpp
        ${LH_TEST_SRC_PATH}/lh/os/net/socket.cpp
        ${LH_TEST_SRC_PATH}/lh/os/serial.cpp
        ${LH_TEST_SRC_PATH}/lh/os/shared.cpp
        ${LH_TEST_SRC_PATH}/lh/os/tick.cpp
        ${LH_TEST_SRC_PATH}/lh/os/timestamp.cpp
        ${LH_TEST_SRC_PATH}/lh/os/debug.cpp
        ${LH_TEST_SRC_PATH}/lh/os/thread/group.cpp
        ${LH_TEST_SRC_PATH}/lh/os/logger/queue.cpp
        ${LH_TEST_SRC_PATH}/lh/os/logger/daily.cpp
        ${LH_TEST_SRC_PATH}/lh/os/logger/capture.cpp
        ${LH_TEST_SRC_PATH}/lh/os/loader/image.cpp
        ${LH_TEST_SRC_PATH}/lh/os/loader/scan.cpp
        ${LH_TEST_SRC_PATH}/lh/os/loader/list.cpp
        ${LH_TEST_SRC_PATH}/lh/os/system/name.cpp
        ${LH_TEST_SRC_PATH}/lh/os/fs/stat.cpp
        ${LH_TEST_SRC_PATH}/lh/os/sql/mysql.cpp
        ${LH_TEST_SRC_PATH}/lh/os/window.cpp
        ${LH_TEST_SRC_PATH}/lh/os/render/backend/gdi.cpp
        ${LH_TEST_SRC_PATH}/lh/os/system/posix/input.cpp
        ${LH_TEST_SRC_PATH}/lh/os/system/posix/key.cpp
        ${LH_TEST_SRC_PATH}/lh/os/system/unix/input.cpp
        ${LH_TEST_SRC_PATH}/lh/os/system/win/input.cpp
)
set(LH_TEST_OS_PRIMITIVE_SOURCES
        ${LH_TEST_SRC_PATH}/lh/os/concurrency.cpp
        ${LH_TEST_SRC_PATH}/lh/os/rail/base.cpp
        ${LH_TEST_SRC_PATH}/lh/os/net/nonblocking.cpp
        ${LH_TEST_SRC_PATH}/lh/os/poll_file.cpp
)
if (LH_LIBRARY_OPTION_OS)
    list(APPEND LH_TEST_OS_SOURCES ${LH_TEST_OS_PRIMITIVE_SOURCES})
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_OS_PRIMITIVE_SOURCES})
endif ()
if (NOT LH_LIBRARY_OPTION_OS)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/fs/path.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/fs/path.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/fs/file.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/fs/file.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/fs/dir.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/fs/dir.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/memory.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/memory.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/interface.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/interface.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/input.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/input.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/tick.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/tick.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/modbus_emu.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/modbus_emu.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/modbus_tcp.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/modbus_tcp.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/socket.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/socket.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/serial.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/serial.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/shared.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/shared.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/sql/mysql.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/sql/mysql.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/timestamp.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/timestamp.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/debug.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/debug.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/thread/group.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/thread/group.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/logger/queue.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/logger/queue.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/logger/daily.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/logger/daily.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/logger/capture.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/logger/capture.cpp)
endif ()
if (NOT (LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW))
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/window.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/window.cpp)
endif ()
if (WIN32 OR NOT LH_LIBRARY_OPTION_OS)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/unix/input.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/unix/input.cpp)
endif ()
if (WIN32 OR NOT (LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW))
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/posix/key.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/posix/key.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/posix/input.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/posix/input.cpp)
endif ()
if (NOT (WIN32 AND LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW AND LH_LIBRARY_OPTION_UI))
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/render/backend/gdi.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/render/backend/gdi.cpp)
endif ()
if (NOT (WIN32 AND LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW))
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/win/input.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/win/input.cpp)
endif ()
if (NOT WIN32 AND LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW)
    list(APPEND LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/posix/surface.cpp)
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/posix/surface.cpp)
endif ()
if (LH_TEST_OS_SOURCES)
    # os is being rebuilt from scratch, one piece at a time.
    lh_add_test_core(lh_test_os ${LH_TEST_OS_SOURCES})
    target_include_directories(lh_test_os PRIVATE "${LH_LIB_SRC_PATH}/..")
    set_tests_properties(lh_test_os PROPERTIES TIMEOUT 120)
endif ()

set(LH_TEST_STR_SOURCES
        ${LH_TEST_SRC_PATH}/lh/core/str.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/format/bytes.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/format/base64.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/format/hex.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/template.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/template/registry.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/format/sint.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/format/sllong.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/format/text.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/format/uint.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/format/ullong.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/list.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/list/span.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/parse/hex.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/parse/radix.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/parse/text.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/parse/uint.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/parse/ullong.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/parse/sllong.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/scanf/next.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/scanf/spec.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/split/next.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/split/line.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/split/once.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/utf8.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/line/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/view.cpp
        ${LH_TEST_SRC_PATH}/lh/core/str/cursor.cpp
        ${LH_TEST_SRC_PATH}/lh/core/wchar/case/map.cpp
        ${LH_TEST_SRC_PATH}/lh/core/wstr.cpp
        ${LH_TEST_SRC_PATH}/lh/core/wstr/list.cpp
        ${LH_TEST_SRC_PATH}/lh/core/wstr/view.cpp
)
set(LH_TEST_STR_FPU_SOURCES ${LH_TEST_SRC_PATH}/lh/core/str/format/float.cpp)
if (LH_LIBRARY_OPTION_MATH_FPU)
    list(APPEND LH_TEST_STR_SOURCES ${LH_TEST_STR_FPU_SOURCES})
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_STR_FPU_SOURCES})
endif ()
lh_add_test_core(lh_test_str ${LH_TEST_STR_SOURCES})

lh_add_test_core(lh_test_time
        ${LH_TEST_SRC_PATH}/lh/core/date.cpp
        ${LH_TEST_SRC_PATH}/lh/core/datetime.cpp
        ${LH_TEST_SRC_PATH}/lh/core/time.cpp
        ${LH_TEST_SRC_PATH}/lh/core/time/hour.cpp
        ${LH_TEST_SRC_PATH}/lh/core/time/minute.cpp
        ${LH_TEST_SRC_PATH}/lh/core/time/second.cpp
        ${LH_TEST_SRC_PATH}/lh/core/timestamp.cpp
)

# lh/os/input: the event, the queue, the recognizer and the wheel step's conversion
# need no OS, so they are tested in every build.
lh_add_test_core(lh_test_os_input
        ${LH_TEST_SRC_PATH}/lh/os/input/contacts.cpp
        ${LH_TEST_SRC_PATH}/lh/os/input/event.cpp
        ${LH_TEST_SRC_PATH}/lh/os/input/queue.cpp
        ${LH_TEST_SRC_PATH}/lh/os/input/recognizer.cpp
        ${LH_TEST_SRC_PATH}/lh/os/input/wheel.cpp
)

# lh/os/controller: the controller between an input source and its handler needs no
# OS either, so it is tested in every build.
lh_add_test_core(lh_test_os_controller
        ${LH_TEST_SRC_PATH}/lh/os/controller.cpp
)

lh_add_test_core(lh_test_timer
        ${LH_TEST_SRC_PATH}/lh/core/timer.cpp
        ${LH_TEST_SRC_PATH}/lh/core/timer/tick.cpp
)

lh_add_test_core(lh_test_util
        ${LH_TEST_SRC_PATH}/lh/core/util/interval.cpp
        ${LH_TEST_SRC_PATH}/lh/core/util/memory.cpp
        ${LH_TEST_SRC_PATH}/lh/core/util/null.cpp
        ${LH_TEST_SRC_PATH}/lh/core/util/char/case/map.cpp
        ${LH_TEST_SRC_PATH}/lh/core/util/str/case/map.cpp
        ${LH_TEST_SRC_PATH}/lh/core/util/str/ptr.cpp
        ${LH_TEST_SRC_PATH}/lh/core/util/str/ptr/dot.cpp
        ${LH_TEST_SRC_PATH}/lh/core/util/swap.cpp
        ${LH_TEST_SRC_PATH}/lh/core/util/wstr/ptr.cpp
)

set(LH_TEST_UI_SOURCES
        ${LH_TEST_SRC_PATH}/lh/ui/axis.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/blur.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/box.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/brush.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/canvas.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/canvas/stroke.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/display/backend.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/transition.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/canvas/sw.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/canvas/web.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/app.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/app/route.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/host.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/host/serve.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/buf.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/page.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/player.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/ws.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/viewport3d.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/color.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/decl.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/entity.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/button.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/container.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/label.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/frame/stats.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/input.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/scrollbar.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/toggle.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/font.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/gradient.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/image.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/insets.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/layout.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/mask.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/paint.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/pen.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/pixmap.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/point.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/radius.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/range.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/rect.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/rects.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/scalar.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/shadow.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/style.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/text.cpp
        # lh/ui.h as a whole, from C++ and (umbrella.c) from C11.
        ${LH_TEST_SRC_PATH}/lh/ui/umbrella.c
        ${LH_TEST_SRC_PATH}/lh/ui/umbrella.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/view.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/view/present.cpp
        # lh_ui_surface_t is a Windows DIB behind an OS surface, so it is only
        # there when there is a native window to hold one — the same guard its
        # source has.
        ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/window.cpp
)
if (LH_LIBRARY_OPTION_UI AND LH_LIBRARY_OPTION_MATH_FPU)
    list(APPEND LH_TEST_UI_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/renderer3d.cpp)
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/renderer3d.cpp)
endif ()
if (NOT LH_LIBRARY_OPTION_UI)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp ${LH_TEST_SRC_PATH}/lh/ui/window.cpp)
    list(REMOVE_ITEM LH_TEST_UI_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp ${LH_TEST_SRC_PATH}/lh/ui/window.cpp)
endif ()
if (LH_LIBRARY_OPTION_UI AND NOT (LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW))
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp ${LH_TEST_SRC_PATH}/lh/ui/window.cpp)
    list(REMOVE_ITEM LH_TEST_UI_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp ${LH_TEST_SRC_PATH}/lh/ui/window.cpp)
endif ()
if (LH_LIBRARY_OPTION_UI)
    # surface.cpp is already in or out of the list above; the rest is the whole
    # of the ui core, which only exists when the option does.
    lh_add_test_core(lh_test_ui ${LH_TEST_UI_SOURCES})
else ()
    list(REMOVE_ITEM LH_TEST_UI_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp ${LH_TEST_SRC_PATH}/lh/ui/window.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_UI_SOURCES})
endif ()

# The SQLite backend exists only with LH_LIBRARY_OPTION_SQL_SQLITE.
if (LH_LIBRARY_OPTION_SQL_SQLITE)
    lh_add_test_core(lh_test_sql ${LH_TEST_SRC_PATH}/lh/os/sql/sqlite.cpp
            ${LH_TEST_SRC_PATH}/lh/os/sql/worker.cpp
            ${LH_TEST_SRC_PATH}/lh/os/sql/keep.cpp)
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/sql/sqlite.cpp
            ${LH_TEST_SRC_PATH}/lh/os/sql/worker.cpp
            ${LH_TEST_SRC_PATH}/lh/os/sql/keep.cpp)
endif ()

# scripts/css2decl.py, run the two ways it can go wrong: its self-test asks whether it
# still refuses what it should, and lh_test_css2decl asks whether what it writes is C
# this library takes. The second is generated at build time and compiled as C, so a
# generator that drifts from the API it emits calls into breaks the build, not an app.
#
# Both need Python, which a library build does not; without it they are not built and
# the guard below is told so.
set(LH_TEST_CSS2DECL_SOURCE ${LH_TEST_SRC_PATH}/lh/ui/css2decl.cpp)
find_package(Python3 COMPONENTS Interpreter QUIET)
if (LH_LIBRARY_OPTION_UI AND Python3_Interpreter_FOUND)
    set(LH_CSS2DECL_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/css2decl.py")
    set(LH_CSS2DECL_GENERATED)
    # Each sheet is generated under its own prefix and directory, and both are linked
    # into one test: two screens in one program is the case the prefix exists for.
    foreach (LH_CSS2DECL_PAIR IN ITEMS "css2decl:lh_test_css_" "css2decl_flow:lh_test_flow_"
                                       "css2decl_app:lh_test_app_")
        string(REPLACE ":" ";" LH_CSS2DECL_PAIR "${LH_CSS2DECL_PAIR}")
        list(GET LH_CSS2DECL_PAIR 0 LH_CSS2DECL_NAME)
        list(GET LH_CSS2DECL_PAIR 1 LH_CSS2DECL_PREFIX)
        set(LH_CSS2DECL_SHEET "${LH_TEST_SRC_PATH}/lh/ui/${LH_CSS2DECL_NAME}.css")
        set(LH_CSS2DECL_OUT "${CMAKE_CURRENT_BINARY_DIR}/${LH_CSS2DECL_NAME}_generated")
        add_custom_command(
                OUTPUT "${LH_CSS2DECL_OUT}/ui_generated.c" "${LH_CSS2DECL_OUT}/ui_generated.h"
                COMMAND Python3::Interpreter "${LH_CSS2DECL_SCRIPT}" "${LH_CSS2DECL_SHEET}"
                        "${LH_CSS2DECL_OUT}" --prefix ${LH_CSS2DECL_PREFIX}
                DEPENDS "${LH_CSS2DECL_SCRIPT}" "${LH_CSS2DECL_SHEET}"
                COMMENT "css2decl: ${LH_CSS2DECL_SHEET}"
                VERBATIM)
        list(APPEND LH_CSS2DECL_GENERATED "${LH_CSS2DECL_OUT}/ui_generated.c")
    endforeach ()
    lh_add_test_core(lh_test_css2decl ${LH_TEST_CSS2DECL_SOURCE} ${LH_CSS2DECL_GENERATED})
    target_include_directories(lh_test_css2decl PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    add_test(NAME lh_test_css2decl_selftest
             COMMAND Python3::Interpreter "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/css2decl-selftest.py")
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_CSS2DECL_SOURCE})
endif ()

# The JavaScript engine of lh/core/web/js: the lexer needs no OS, but the option that
# carries it decides whether it is built here.
if (LH_LIBRARY_OPTION_JS)
    lh_add_test_core(lh_test_js ${LH_TEST_SRC_PATH}/lh/core/web/js/lexer.cpp)
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/core/web/js/lexer.cpp)
endif ()

# Every test file belongs to a core, or the build says so.
#
# The check runs over the same lists the targets above were built from, so a file
# dropped from them — the way math/rescale.cpp was — cannot slip through again. A
# file whose option is off is the one allowed exception: it is not built here and
# is not run here, and it says so above.
file(GLOB_RECURSE LH_TEST_ALL_SOURCES CONFIGURE_DEPENDS "${LH_TEST_SRC_PATH}/lh/*.cpp")
set(LH_TEST_UNLISTED)
foreach (LH_TEST_SOURCE IN LISTS LH_TEST_ALL_SOURCES)
    if (NOT (LH_TEST_SOURCE IN_LIST LH_TEST_LISTED_SOURCES
                  OR LH_TEST_SOURCE IN_LIST LH_TEST_OPTION_OFF_SOURCES))
        list(APPEND LH_TEST_UNLISTED "${LH_TEST_SOURCE}")
    endif ()
endforeach ()
if (LH_TEST_UNLISTED)
    string(REPLACE ";" "\n  " LH_TEST_UNLISTED_TEXT "${LH_TEST_UNLISTED}")
    message(FATAL_ERROR
            "test files that no lh_test_* core builds — add them to a core above:\n  ${LH_TEST_UNLISTED_TEXT}")
endif ()
