# Parent scope still has the core project name (e.g. lh); this file's project() overwrites PROJECT_NAME.
set(LH_CORE_TARGET ${PROJECT_NAME})
project(${LH_CORE_TARGET}_test)

# Paths for bundled test dependencies and test sources.
set(LH_TEST_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(LH_TEST_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LH_TEST_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

# The HTTP brick must work without a full LH runtime, OS backend or GoogleTest.
add_executable(lh_test_http_parser http/parser.c "${LH_LIB_SRC_PATH}/http/parser.c")
target_include_directories(lh_test_http_parser PRIVATE
        "$<TARGET_PROPERTY:${LH_CORE_TARGET},INTERFACE_INCLUDE_DIRECTORIES>")
option(LH_HTTP_PARSER_SANITIZE "Sanitize the standalone HTTP parser test" OFF)
if(LH_HTTP_PARSER_SANITIZE AND NOT MSVC)
    target_compile_options(lh_test_http_parser PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(lh_test_http_parser PRIVATE -fsanitize=address,undefined)
endif()
add_test(NAME lh_http_parser COMMAND lh_test_http_parser)

# Skip test targets if googletest submodule is not initialized.
if (LH_LIBRARY_OPTION_UI AND LH_LIBRARY_OPTION_MATH_FPU)
    add_executable(lh_test_renderer3d renderer3d.c)
    target_link_libraries(lh_test_renderer3d PRIVATE ${LH_CORE_TARGET})
    add_test(NAME lh_renderer3d COMMAND lh_test_renderer3d)
endif ()

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

# One executable per core, not one for everything.
#
# The list used to be a single block naming every test file the library has, and
# that is how test/src/lh/math/rescale.cpp came to be in the tree with two tests
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

# lh_add_test_core(<name> <sources...>): one executable, linked like the one it
# replaces, and registered with CTest under the bare core name. It also records
# what it was given, which is what the guard at the end of this file reads.
function(lh_add_test_core name)
    add_executable(${name} ${LH_TEST_RUNNER} ${ARGN})
    if (LH_TEST_IPO)
        set_property(TARGET ${name} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    endif ()
    target_include_directories(${name} PRIVATE "${LH_TEST_INCLUDE_PATH}")
    target_link_libraries(${name} PRIVATE ${LH_CORE_TARGET} gtest)
    add_test(NAME ${name} COMMAND $<TARGET_FILE:${name}>)
    set(LH_TEST_LISTED_SOURCES ${LH_TEST_LISTED_SOURCES} ${ARGN} PARENT_SCOPE)
endfunction()

# The path decides the core: a file under util/str belongs to util even though it
# is about strings, and one under wchar belongs to str. Anything else makes the
# guard at the end unhappy, which is the point of the guard.
set(LH_TEST_LISTED_SOURCES)

# Types, errors and the few pieces every core leans on.
lh_add_test_core(lh_test_core
        ${LH_TEST_SRC_PATH}/lh/array.cpp
        ${LH_TEST_SRC_PATH}/lh/error.cpp
        ${LH_TEST_SRC_PATH}/lh/exception.cpp
        ${LH_TEST_SRC_PATH}/lh/exception/origin.cpp
        ${LH_TEST_SRC_PATH}/lh/foreach.cpp
        ${LH_TEST_SRC_PATH}/lh/list.cpp
        ${LH_TEST_SRC_PATH}/lh/logger.cpp
        ${LH_TEST_SRC_PATH}/lh/value.cpp
        ${LH_TEST_SRC_PATH}/lh/real.cpp
        ${LH_TEST_SRC_PATH}/lh/pair.cpp
        ${LH_TEST_SRC_PATH}/lh/ini/entry.cpp
        ${LH_TEST_SRC_PATH}/lh/ini/parser.cpp
        ${LH_TEST_SRC_PATH}/lh/json/node.cpp
        ${LH_TEST_SRC_PATH}/lh/json/minify.cpp
        ${LH_TEST_SRC_PATH}/lh/json/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/json/writer.cpp
        ${LH_TEST_SRC_PATH}/lh/sql/db.cpp
        ${LH_TEST_SRC_PATH}/lh/sql/mysql/query.cpp
        ${LH_TEST_SRC_PATH}/lh/version.cpp
        ${LH_TEST_SRC_PATH}/lh/werror.cpp
)

lh_add_test_core(lh_test_runtime
        ${LH_TEST_SRC_PATH}/lh/runtime/check.cpp
        ${LH_TEST_SRC_PATH}/lh/runtime/error.cpp
        ${LH_TEST_SRC_PATH}/lh/runtime/logger.cpp
)

lh_add_test_core(lh_test_bit
        ${LH_TEST_SRC_PATH}/lh/bit/bswap.cpp
        ${LH_TEST_SRC_PATH}/lh/bit/endian.cpp
        ${LH_TEST_SRC_PATH}/lh/bit/half.cpp
        ${LH_TEST_SRC_PATH}/lh/bit/packed.cpp
        ${LH_TEST_SRC_PATH}/lh/bit/rotate.cpp
        ${LH_TEST_SRC_PATH}/lh/bit/scan.cpp
)

lh_add_test_core(lh_test_char
        ${LH_TEST_SRC_PATH}/lh/char/digit.cpp
        ${LH_TEST_SRC_PATH}/lh/char/dot.cpp
        ${LH_TEST_SRC_PATH}/lh/char/letter.cpp
        ${LH_TEST_SRC_PATH}/lh/char/slash.cpp
        ${LH_TEST_SRC_PATH}/lh/char/xdigit.cpp
)

lh_add_test_core(lh_test_cpu
        ${LH_TEST_SRC_PATH}/lh/cpu/cache.cpp
        ${LH_TEST_SRC_PATH}/lh/cpu/id.cpp
        ${LH_TEST_SRC_PATH}/lh/cpu/simd.cpp
        ${LH_TEST_SRC_PATH}/lh/cpu/vendor.cpp
)

lh_add_test_core(lh_test_crypto
        ${LH_TEST_SRC_PATH}/lh/crypto/ed25519.cpp
        ${LH_TEST_SRC_PATH}/lh/crypto/rijndael.cpp
        ${LH_TEST_SRC_PATH}/lh/crypto/sha.cpp
)

lh_add_test_core(lh_test_hash
        ${LH_TEST_SRC_PATH}/lh/hash/fnv.cpp
)

lh_add_test_core(lh_test_fs
        ${LH_TEST_SRC_PATH}/lh/fs/path.cpp
        ${LH_TEST_SRC_PATH}/lh/fs/path/style.cpp
)

lh_add_test_core(lh_test_io
        ${LH_TEST_SRC_PATH}/lh/io/dgram.cpp
        ${LH_TEST_SRC_PATH}/lh/io/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/io/stream.cpp
        ${LH_TEST_SRC_PATH}/lh/io/writer.cpp
)

set(LH_TEST_MATH_FPU_SOURCES
        ${LH_TEST_SRC_PATH}/lh/math/fpoint.cpp
        ${LH_TEST_SRC_PATH}/lh/math/fsize.cpp
        ${LH_TEST_SRC_PATH}/lh/math/frect.cpp
        ${LH_TEST_SRC_PATH}/lh/math/vec2.cpp
        ${LH_TEST_SRC_PATH}/lh/math/vec3.cpp
        ${LH_TEST_SRC_PATH}/lh/math/vec4.cpp
        ${LH_TEST_SRC_PATH}/lh/math/quat.cpp
        ${LH_TEST_SRC_PATH}/lh/math/mat4.cpp
        ${LH_TEST_SRC_PATH}/lh/math/convert.cpp
        ${LH_TEST_SRC_PATH}/lh/math/float/sqrt.cpp
        ${LH_TEST_SRC_PATH}/lh/math/float/sin_cos.cpp
        ${LH_TEST_SRC_PATH}/lh/math/float/round.cpp
        ${LH_TEST_SRC_PATH}/lh/math/slice.cpp
        ${LH_TEST_SRC_PATH}/lh/math/view.cpp
)
set(LH_TEST_MATH_SOURCES
        ${LH_TEST_SRC_PATH}/lh/math.cpp
        ${LH_TEST_SRC_PATH}/lh/math/point.cpp
        ${LH_TEST_SRC_PATH}/lh/math/size.cpp
        ${LH_TEST_SRC_PATH}/lh/math/rect.cpp
        ${LH_TEST_SRC_PATH}/lh/math/rescale.cpp
        ${LH_TEST_SRC_PATH}/lh/math/scalar_centre.cpp
)
if (LH_LIBRARY_OPTION_MATH_FPU)
    list(APPEND LH_TEST_MATH_SOURCES ${LH_TEST_MATH_FPU_SOURCES})
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_MATH_FPU_SOURCES})
endif ()
lh_add_test_core(lh_test_math ${LH_TEST_MATH_SOURCES})

lh_add_test_core(lh_test_memory
        ${LH_TEST_SRC_PATH}/lh/memory/allocator.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/bounds.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/bounds/allocated.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/memory.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/raw.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/scan_page.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/sized/allocator.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/std.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/tree.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/typed.cpp
        ${LH_TEST_SRC_PATH}/lh/memory/view.cpp
)

lh_add_test_core(lh_test_net
        ${LH_TEST_SRC_PATH}/lh/net/ip.cpp
        ${LH_TEST_SRC_PATH}/lh/net/modbus.cpp
        ${LH_TEST_SRC_PATH}/lh/net/modbus_link.cpp
        ${LH_TEST_SRC_PATH}/lh/net/port.cpp
        ${LH_TEST_SRC_PATH}/lh/net/socket/addr.cpp
        ${LH_TEST_SRC_PATH}/lh/net/socket/addr/ip4.cpp
)

lh_add_test_core(lh_test_numeric
        ${LH_TEST_SRC_PATH}/lh/numeric/big.cpp
        ${LH_TEST_SRC_PATH}/lh/numeric/float.cpp
        ${LH_TEST_SRC_PATH}/lh/numeric/mul.cpp
        ${LH_TEST_SRC_PATH}/lh/numeric/parse/bytes.cpp
        ${LH_TEST_SRC_PATH}/lh/size/ops.cpp
)

set(LH_TEST_OS_SOURCES
        ${LH_TEST_SRC_PATH}/lh/os/fs/dir.cpp
        ${LH_TEST_SRC_PATH}/lh/os/input.cpp
        ${LH_TEST_SRC_PATH}/lh/os/memory.cpp
        ${LH_TEST_SRC_PATH}/lh/os/net/interface.cpp
        ${LH_TEST_SRC_PATH}/lh/os/net/modbus_tcp.cpp
        ${LH_TEST_SRC_PATH}/lh/os/net/socket.cpp
        ${LH_TEST_SRC_PATH}/lh/os/serial.cpp
        ${LH_TEST_SRC_PATH}/lh/os/shared.cpp
        ${LH_TEST_SRC_PATH}/lh/os/tick.cpp
        ${LH_TEST_SRC_PATH}/lh/sql/mysql.cpp
        ${LH_TEST_SRC_PATH}/lh/os/window.cpp
        ${LH_TEST_SRC_PATH}/lh/os/render/backend/gdi.cpp
        ${LH_TEST_SRC_PATH}/lh/os/system/posix/input.cpp
        ${LH_TEST_SRC_PATH}/lh/os/system/posix/key.cpp
        ${LH_TEST_SRC_PATH}/lh/os/system/unix/input.cpp
        ${LH_TEST_SRC_PATH}/lh/os/system/win/input.cpp
)
if (NOT LH_LIBRARY_OPTION_OS)
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
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/modbus_tcp.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/modbus_tcp.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/socket.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/net/socket.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/serial.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/serial.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/shared.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/shared.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/sql/mysql.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/sql/mysql.cpp)
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
if (NOT (WIN32 AND LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW))
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/render/backend/gdi.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/render/backend/gdi.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/win/input.cpp)
    list(REMOVE_ITEM LH_TEST_OS_SOURCES ${LH_TEST_SRC_PATH}/lh/os/system/win/input.cpp)
endif ()
if (LH_TEST_OS_SOURCES)
    # os is being rebuilt from scratch, one piece at a time.
    lh_add_test_core(lh_test_os ${LH_TEST_OS_SOURCES})
endif ()

set(LH_TEST_STR_SOURCES
        ${LH_TEST_SRC_PATH}/lh/str.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/bytes.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/base64.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/hex.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/sint.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/sllong.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/text.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/uint.cpp
        ${LH_TEST_SRC_PATH}/lh/str/format/ullong.cpp
        ${LH_TEST_SRC_PATH}/lh/str/list.cpp
        ${LH_TEST_SRC_PATH}/lh/str/parse/hex.cpp
        ${LH_TEST_SRC_PATH}/lh/str/parse/radix.cpp
        ${LH_TEST_SRC_PATH}/lh/str/parse/text.cpp
        ${LH_TEST_SRC_PATH}/lh/str/parse/uint.cpp
        ${LH_TEST_SRC_PATH}/lh/str/parse/ullong.cpp
        ${LH_TEST_SRC_PATH}/lh/str/parse/sllong.cpp
        ${LH_TEST_SRC_PATH}/lh/str/scanf/next.cpp
        ${LH_TEST_SRC_PATH}/lh/str/split/next.cpp
        ${LH_TEST_SRC_PATH}/lh/str/split/line.cpp
        ${LH_TEST_SRC_PATH}/lh/str/split/once.cpp
        ${LH_TEST_SRC_PATH}/lh/str/utf8.cpp
        ${LH_TEST_SRC_PATH}/lh/str/line/reader.cpp
        ${LH_TEST_SRC_PATH}/lh/str/view.cpp
        ${LH_TEST_SRC_PATH}/lh/wchar/case/map.cpp
        ${LH_TEST_SRC_PATH}/lh/wstr.cpp
        ${LH_TEST_SRC_PATH}/lh/wstr/list.cpp
        ${LH_TEST_SRC_PATH}/lh/wstr/view.cpp
)
set(LH_TEST_STR_FPU_SOURCES ${LH_TEST_SRC_PATH}/lh/str/format/float.cpp)
if (LH_LIBRARY_OPTION_MATH_FPU)
    list(APPEND LH_TEST_STR_SOURCES ${LH_TEST_STR_FPU_SOURCES})
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_STR_FPU_SOURCES})
endif ()
lh_add_test_core(lh_test_str ${LH_TEST_STR_SOURCES})

lh_add_test_core(lh_test_time
        ${LH_TEST_SRC_PATH}/lh/date.cpp
        ${LH_TEST_SRC_PATH}/lh/datetime.cpp
        ${LH_TEST_SRC_PATH}/lh/time.cpp
        ${LH_TEST_SRC_PATH}/lh/time/hour.cpp
        ${LH_TEST_SRC_PATH}/lh/time/minute.cpp
        ${LH_TEST_SRC_PATH}/lh/time/second.cpp
        ${LH_TEST_SRC_PATH}/lh/timestamp.cpp
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
        ${LH_TEST_SRC_PATH}/lh/timer.cpp
        ${LH_TEST_SRC_PATH}/lh/timer/tick.cpp
)

lh_add_test_core(lh_test_util
        ${LH_TEST_SRC_PATH}/lh/util/interval.cpp
        ${LH_TEST_SRC_PATH}/lh/util/memory.cpp
        ${LH_TEST_SRC_PATH}/lh/util/null.cpp
        ${LH_TEST_SRC_PATH}/lh/util/str/case/map.cpp
        ${LH_TEST_SRC_PATH}/lh/util/str/ptr.cpp
        ${LH_TEST_SRC_PATH}/lh/util/str/ptr/dot.cpp
        ${LH_TEST_SRC_PATH}/lh/util/swap.cpp
        ${LH_TEST_SRC_PATH}/lh/util/wstr/ptr.cpp
)

set(LH_TEST_UI_SOURCES
        ${LH_TEST_SRC_PATH}/lh/ui/axis.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/blur.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/box.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/brush.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/canvas.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/transition.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/canvas/sw.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/canvas/web.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/host.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/html.cpp
        ${LH_TEST_SRC_PATH}/lh/ui/web/page.cpp
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
        # lh_ui_surface_t is a Windows DIB behind an OS surface, so it is only
        # there when there is a native window to hold one — the same guard its
        # source has.
        ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp
)
if (NOT LH_LIBRARY_OPTION_UI)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp)
    list(REMOVE_ITEM LH_TEST_UI_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp)
endif ()
if (LH_LIBRARY_OPTION_UI AND NOT (LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW))
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp)
    list(REMOVE_ITEM LH_TEST_UI_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp)
endif ()
if (LH_LIBRARY_OPTION_UI)
    # surface.cpp is already in or out of the list above; the rest is the whole
    # of the ui core, which only exists when the option does.
    lh_add_test_core(lh_test_ui ${LH_TEST_UI_SOURCES})
else ()
    list(REMOVE_ITEM LH_TEST_UI_SOURCES ${LH_TEST_SRC_PATH}/lh/ui/surface.cpp)
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_UI_SOURCES})
endif ()

# The SQLite backend exists only with LH_LIBRARY_OPTION_SQL_SQLITE.
if (LH_LIBRARY_OPTION_SQL_SQLITE)
    lh_add_test_core(lh_test_sql ${LH_TEST_SRC_PATH}/lh/sql/sqlite.cpp)
else ()
    list(APPEND LH_TEST_OPTION_OFF_SOURCES ${LH_TEST_SRC_PATH}/lh/sql/sqlite.cpp)
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
