cmake_minimum_required(VERSION 3.12)
project(lh
        DESCRIPTION "Light Helper вЂ” portable C utility library"
        LANGUAGES C
)

# Project helper modules: version parsing, generated config, and feature options.
include(cmake/version.cmake)
include(cmake/config.cmake)
include(cmake/library_options.cmake)
include(cmake/check_simd.cmake)

# Compile-only SIMD tier probes (see cmake/check_simd.cmake) вЂ” sets
# LH_SIMD_HAVE_SSE2 / _SSSE3 / _AVX2 / _AESNI, consumed by lh_generate_config() below.
lh_check_simd()

# ---------------------------------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Windows target floor: XP (5.1) by default. Handwritten Win32 decls in
# lh/os/system/win are the real fence; this stops SDK headers that still get
# pulled elsewhere from exposing Vista+ APIs by default. One value for C and
# C++ alike; scripts/build-windows-legacy.bat passes 0x0600 for Vista.
set(LH_OS_WINDOWS_VERSION "0x0501" CACHE STRING "WINVER / _WIN32_WINNT for Windows builds (0x0501 = XP, 0x0600 = Vista)")
set_property(CACHE LH_OS_WINDOWS_VERSION PROPERTY STRINGS 0x0501 0x0600)
# Four hex digits, 0x0501 (XP) or newer: a typo such as 0x500 or 600 would otherwise
# build quietly against the wrong API level.
if (NOT LH_OS_WINDOWS_VERSION MATCHES "^0x0(50[1-9A-Fa-f]|5[1-9A-Fa-f][0-9A-Fa-f]|[6-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f])$")
    message(FATAL_ERROR "LH_OS_WINDOWS_VERSION must be four hex digits, 0x0501 (XP) or newer (got '${LH_OS_WINDOWS_VERSION}')")
endif ()
if (WIN32)
    add_compile_definitions(WINVER=${LH_OS_WINDOWS_VERSION} _WIN32_WINNT=${LH_OS_WINDOWS_VERSION})
else ()
    # 64-bit off_t / ino_t on 32-bit Unix (a no-op on 64-bit). Without it readdir()
    # and stat() fail with EOVERFLOW on file systems that hand out 64-bit inode
    # numbers (XFS inode64, btrfs, NFS, WSL drvfs): lh_os_fs_dir_read() reports an
    # error and the directory looks empty. No public header uses off_t or struct
    # stat, so dependents need not match.
    add_compile_definitions(_FILE_OFFSET_BITS=64)
endif ()

# ---------------------------------------------------------------------------
# Build options
# ---------------------------------------------------------------------------
option(LH_BUILD_DOCS "Build documentation" ON)
option(LH_BUILD_TESTS "Build tests" ON)
option(LH_BUILD_BENCH "Build microbenchmarks (requires the bench/lib/benchmark submodule)" OFF)
option(LH_BUILD_EXAMPLES "Build the example programs" OFF)

# PIC/GOT: ON for desktop (static lib reusable from a shared object). OFF for
# freestanding / bare-metal вЂ” there is no dynamic loader to relocate .got, and
# an uncopied GOT leaves runtime globals (writer, terminate, вЂ¦) as NULL.
option(LH_POSITION_INDEPENDENT_CODE
        "Build lh as position-independent (-fPIC). OFF for bare-metal / freestanding."
        ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ${LH_POSITION_INDEPENDENT_CODE})

# ---------------------------------------------------------------------------
# Release optimization options
# ---------------------------------------------------------------------------
option(LH_ENABLE_RELEASE_MAX_OPT
        "Apply aggressive optimizations for the lh library in Release only (LTO: LH_LIBRARY_OPTION_LTO)"
        ON)

# ---------------------------------------------------------------------------
# Version (read from VERSION file; substituted into include/lh/config.h via .in)
# ---------------------------------------------------------------------------
lh_read_version("${CMAKE_CURRENT_SOURCE_DIR}/VERSION"
        LH_VERSION_MAJOR LH_VERSION_MINOR LH_VERSION_PATCH)

# Generate `include/lh/config.h` from `include/lh/config.h.in`.
lh_generate_config()

# ---------------------------------------------------------------------------

if (LH_BUILD_DOCS)
    add_subdirectory(docs)
endif ()

# ---------------------------------------------------------------------------

# Library source and public include roots.
set(LH_LIB_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}")
set(LH_LIB_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Core library translation units.
set(LH_LIB_SOURCES
        ${LH_LIB_SRC_PATH}/runtime/allocator.c
        ${LH_LIB_SRC_PATH}/runtime/terminate.c
        ${LH_LIB_SRC_PATH}/runtime/logger.c
        ${LH_LIB_SRC_PATH}/runtime/writer.c
        ${LH_LIB_SRC_PATH}/runtime/reader.c
        ${LH_LIB_SRC_PATH}/runtime/stream.c
        ${LH_LIB_SRC_PATH}/runtime/check.c
        ${LH_LIB_SRC_PATH}/exception.c
        ${LH_LIB_SRC_PATH}/exception/origin.c

        ${LH_LIB_SRC_PATH}/numeric/fixed/types.c
        ${LH_LIB_SRC_PATH}/numeric/fixed/limits.c
        ${LH_LIB_SRC_PATH}/numeric/parse/bytes.c
        ${LH_LIB_SRC_PATH}/numeric/big.c
        ${LH_LIB_SRC_PATH}/numeric/mul.c

        ${LH_LIB_SRC_PATH}/cpu/cache.c
        ${LH_LIB_SRC_PATH}/cpu/id.c
        ${LH_LIB_SRC_PATH}/cpu/simd.c
        ${LH_LIB_SRC_PATH}/cpu/vendor.c

        ${LH_LIB_SRC_PATH}/memory/allocator.c
        ${LH_LIB_SRC_PATH}/memory/sized/allocator.c
        ${LH_LIB_SRC_PATH}/memory/tree.c
        ${LH_LIB_SRC_PATH}/memory/tree/header.c
        ${LH_LIB_SRC_PATH}/memory/bounds/allocated.c
        ${LH_LIB_SRC_PATH}/memory/bounds.c
        ${LH_LIB_SRC_PATH}/memory/typed.c
        ${LH_LIB_SRC_PATH}/memory/typed/allocated.c
        ${LH_LIB_SRC_PATH}/memory/view.c
        ${LH_LIB_SRC_PATH}/memory/std.c
        ${LH_LIB_SRC_PATH}/memory.c
        ${LH_LIB_SRC_PATH}/spsc.c
        ${LH_LIB_SRC_PATH}/array.c
        ${LH_LIB_SRC_PATH}/event/entry.c
        ${LH_LIB_SRC_PATH}/event/subscription.c
        ${LH_LIB_SRC_PATH}/event/bus.c
        ${LH_LIB_SRC_PATH}/list.c
        ${LH_LIB_SRC_PATH}/value.c
        ${LH_LIB_SRC_PATH}/real/bits.c
        ${LH_LIB_SRC_PATH}/real/cached.c
        ${LH_LIB_SRC_PATH}/real/decimal.c
        ${LH_LIB_SRC_PATH}/real/diy.c
        ${LH_LIB_SRC_PATH}/real/grisu.c
        ${LH_LIB_SRC_PATH}/real/lemire.c
        ${LH_LIB_SRC_PATH}/real/power5.c
        ${LH_LIB_SRC_PATH}/pair.c
        ${LH_LIB_SRC_PATH}/ini/entry.c
        ${LH_LIB_SRC_PATH}/ini/error.c
        ${LH_LIB_SRC_PATH}/ini/parser.c
        ${LH_LIB_SRC_PATH}/http/char.c
        ${LH_LIB_SRC_PATH}/http/header.c
        ${LH_LIB_SRC_PATH}/http/parser.c
        ${LH_LIB_SRC_PATH}/http/request.c
        ${LH_LIB_SRC_PATH}/http/response.c
        ${LH_LIB_SRC_PATH}/http/version.c
        ${LH_LIB_SRC_PATH}/sql/db.c
        ${LH_LIB_SRC_PATH}/sql/driver.c
        ${LH_LIB_SRC_PATH}/sql/mysql/query.c
        ${LH_LIB_SRC_PATH}/sql/stmt.c
        ${LH_LIB_SRC_PATH}/json/error.c
        ${LH_LIB_SRC_PATH}/json/node.c
        ${LH_LIB_SRC_PATH}/json/minify.c
        ${LH_LIB_SRC_PATH}/json/reader.c
        ${LH_LIB_SRC_PATH}/json/writer.c
        ${LH_LIB_SRC_PATH}/list/node.c

        ${LH_LIB_SRC_PATH}/math/point.c
        ${LH_LIB_SRC_PATH}/math/size.c
        ${LH_LIB_SRC_PATH}/math/rect.c

        ${LH_LIB_SRC_PATH}/version.c
        ${LH_LIB_SRC_PATH}/date.c
        ${LH_LIB_SRC_PATH}/date/year.c
        ${LH_LIB_SRC_PATH}/date/month.c
        ${LH_LIB_SRC_PATH}/date/month/index.c
        ${LH_LIB_SRC_PATH}/date/day.c
        ${LH_LIB_SRC_PATH}/time.c
        ${LH_LIB_SRC_PATH}/time/second.c
        ${LH_LIB_SRC_PATH}/time/minute.c
        ${LH_LIB_SRC_PATH}/time/hour.c
        ${LH_LIB_SRC_PATH}/datetime.c
        ${LH_LIB_SRC_PATH}/timestamp.c
        ${LH_LIB_SRC_PATH}/timer.c
        ${LH_LIB_SRC_PATH}/timer/tick.c
        ${LH_LIB_SRC_PATH}/os/input/contacts.c
        ${LH_LIB_SRC_PATH}/os/input/event.c
        ${LH_LIB_SRC_PATH}/os/input/queue.c
        ${LH_LIB_SRC_PATH}/os/input/recognizer.c
        ${LH_LIB_SRC_PATH}/os/input/recognizer/config.c
        ${LH_LIB_SRC_PATH}/os/input/recognizer/pointer.c
        ${LH_LIB_SRC_PATH}/os/input/wheel.c
        ${LH_LIB_SRC_PATH}/os/controller.c
        ${LH_LIB_SRC_PATH}/os/controller/dispatch.c
        ${LH_LIB_SRC_PATH}/error.c
        ${LH_LIB_SRC_PATH}/werror.c
        ${LH_LIB_SRC_PATH}/runtime/error.c

        ${LH_LIB_SRC_PATH}/util/wchar/case/fold.c
        ${LH_LIB_SRC_PATH}/util/wstr/case/map.c
        ${LH_LIB_SRC_PATH}/util/wstr/ptr.c
        ${LH_LIB_SRC_PATH}/util/char/case/fold.c
        ${LH_LIB_SRC_PATH}/util/str/case/map.c
        ${LH_LIB_SRC_PATH}/util/str/ptr.c
        ${LH_LIB_SRC_PATH}/util/wchar.c
        ${LH_LIB_SRC_PATH}/str/view.c
        ${LH_LIB_SRC_PATH}/wstr/view.c
        ${LH_LIB_SRC_PATH}/str.c
        ${LH_LIB_SRC_PATH}/wstr.c
        ${LH_LIB_SRC_PATH}/str/format/uint.c
        ${LH_LIB_SRC_PATH}/str/format/ullong.c
        ${LH_LIB_SRC_PATH}/str/format/put.c
        ${LH_LIB_SRC_PATH}/str/format/float.c
        ${LH_LIB_SRC_PATH}/str/format/sint.c
        ${LH_LIB_SRC_PATH}/str/format/sllong.c
        ${LH_LIB_SRC_PATH}/str/format/real.c
        ${LH_LIB_SRC_PATH}/str/format/hex.c
        ${LH_LIB_SRC_PATH}/str/format/text.c
        ${LH_LIB_SRC_PATH}/str/format/bytes.c
        ${LH_LIB_SRC_PATH}/str/format/base64.c
        ${LH_LIB_SRC_PATH}/str/parse/text.c
        ${LH_LIB_SRC_PATH}/str/parse/uint.c
        ${LH_LIB_SRC_PATH}/str/parse/ullong.c
        ${LH_LIB_SRC_PATH}/str/parse/sllong.c
        ${LH_LIB_SRC_PATH}/str/parse/real.c
        ${LH_LIB_SRC_PATH}/str/parse/hex.c
        ${LH_LIB_SRC_PATH}/str/parse/radix.c
        ${LH_LIB_SRC_PATH}/str/split/next.c
        ${LH_LIB_SRC_PATH}/str/split/line.c
        ${LH_LIB_SRC_PATH}/str/split/once.c
        ${LH_LIB_SRC_PATH}/str/utf8.c
        ${LH_LIB_SRC_PATH}/str/line/reader.c
        ${LH_LIB_SRC_PATH}/str/scanf/spec.c
        ${LH_LIB_SRC_PATH}/str/scanf/next.c
        ${LH_LIB_SRC_PATH}/str/list.c
        ${LH_LIB_SRC_PATH}/wstr/list.c
        ${LH_LIB_SRC_PATH}/str/list/span.c

        ${LH_LIB_SRC_PATH}/fs/path.c
        ${LH_LIB_SRC_PATH}/fs/path/style.c
        ${LH_LIB_SRC_PATH}/fs/stat.c

        ${LH_LIB_SRC_PATH}/logger.c

        ${LH_LIB_SRC_PATH}/crypto/sha.c
        ${LH_LIB_SRC_PATH}/crypto/rijndael.c
        ${LH_LIB_SRC_PATH}/crypto/ed25519.c
        ${LH_LIB_SRC_PATH}/crypto/wipe.c

        ${LH_LIB_SRC_PATH}/hash/fnv.c

        ${LH_LIB_SRC_PATH}/net/ip.c
        ${LH_LIB_SRC_PATH}/net/port.c
        ${LH_LIB_SRC_PATH}/net/socket/addr/ip4.c
        ${LH_LIB_SRC_PATH}/net/socket/addr.c
        ${LH_LIB_SRC_PATH}/net/modbus.c
        ${LH_LIB_SRC_PATH}/net/modbus/link.c
        ${LH_LIB_SRC_PATH}/net/modbus/link/counters.c
        ${LH_LIB_SRC_PATH}/net/modbus/read.c
        ${LH_LIB_SRC_PATH}/net/modbus/want.c

        ${LH_LIB_SRC_PATH}/io/reader.c
        ${LH_LIB_SRC_PATH}/io/writer.c
        ${LH_LIB_SRC_PATH}/io/stream.c
        ${LH_LIB_SRC_PATH}/io/dgram/reader.c
        ${LH_LIB_SRC_PATH}/io/dgram/writer.c
        ${LH_LIB_SRC_PATH}/io/dgram.c

        ${LH_LIB_SRC_PATH}/lh.c
)

if (LH_LIBRARY_OPTION_OS)
    list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/os/worker.c)
    list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/os/worker/job.c)
    list(APPEND LH_LIB_SOURCES
            ${LH_LIB_SRC_PATH}/os.c
            ${LH_LIB_SRC_PATH}/os/error.c
            ${LH_LIB_SRC_PATH}/os/alloc.c
            ${LH_LIB_SRC_PATH}/os/system.c
            ${LH_LIB_SRC_PATH}/os/system/error.c
            ${LH_LIB_SRC_PATH}/os/system/error/capture.c
            ${LH_LIB_SRC_PATH}/os/system/str.c
            ${LH_LIB_SRC_PATH}/os/timestamp.c
            ${LH_LIB_SRC_PATH}/os/tick.c
            ${LH_LIB_SRC_PATH}/os/concurrency.c
            ${LH_LIB_SRC_PATH}/os/wake.c
            ${LH_LIB_SRC_PATH}/os/env.c
            ${LH_LIB_SRC_PATH}/os/memory.c
            ${LH_LIB_SRC_PATH}/os/input.c
            ${LH_LIB_SRC_PATH}/os/fs/path.c
            ${LH_LIB_SRC_PATH}/os/fs/stat.c
            ${LH_LIB_SRC_PATH}/os/fs/file.c
            ${LH_LIB_SRC_PATH}/os/fs/dir.c
            ${LH_LIB_SRC_PATH}/os/net.c
            ${LH_LIB_SRC_PATH}/os/net/socket.c
            ${LH_LIB_SRC_PATH}/os/net/interface.c
            ${LH_LIB_SRC_PATH}/os/serial.c
            ${LH_LIB_SRC_PATH}/os/shared.c
            ${LH_LIB_SRC_PATH}/os/module.c
            ${LH_LIB_SRC_PATH}/os/module/ops.c
            ${LH_LIB_SRC_PATH}/os/loader.c
            ${LH_LIB_SRC_PATH}/sql/mysql.c
            ${LH_LIB_SRC_PATH}/sql/mysql/api.c
            ${LH_LIB_SRC_PATH}/sql/mysql/conn.c
            ${LH_LIB_SRC_PATH}/sql/mysql/prepared.c
    )
    # Kernel backend: the only sources that call Win32 / POSIX directly.
    # Each backend also names the system libraries it links against.
    if (WIN32)
        set(LH_OS_SYSTEM_BACKEND win)
        set(LH_OS_SYSTEM_LINK_LIBS ws2_32 iphlpapi)
    else ()
        set(LH_OS_SYSTEM_BACKEND posix)
        set(LH_OS_SYSTEM_LINK_LIBS ${CMAKE_DL_LIBS})
    endif ()
    list(APPEND LH_LIB_SOURCES
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/error/capture.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/error/kind.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/timestamp.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/tick.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/concurrency.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/env.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/memory.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/fs/path.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/fs/file.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/fs/dir.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/fs/stat.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/net.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/net/socket.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/net/interface.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/serial.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/shared.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/str.c
    )
    # Unix kernel: what the input driver says, apart from any window system (X11 is
    # posix/). Built wherever the posix backend is; lh/os/input.c calls it on Linux
    # and macOS.
    if (NOT WIN32)
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/os/system/unix/input.c)
    endif ()
    if (LH_LIBRARY_OPTION_OS_WINDOW)
        list(APPEND LH_LIB_SOURCES
                ${LH_LIB_SRC_PATH}/os/app.c
                ${LH_LIB_SRC_PATH}/os/window.c
                ${LH_LIB_SRC_PATH}/os/window/corner.c
        )
        if (WIN32)
            list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/os/system/win/window.c)
            list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/os/system/win/key.c)
            list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/os/system/win/input.c)
            list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/os/system/win/surface.c)
            list(APPEND LH_OS_SYSTEM_LINK_LIBS user32 gdi32)
        else ()
            # Xlib, opened with dlopen when the first window is: nothing to link but
            # the loader, and no X11 headers or development package to build with.
            list(APPEND LH_LIB_SOURCES
                    ${LH_LIB_SRC_PATH}/os/system/posix/x11.c
                    ${LH_LIB_SRC_PATH}/os/system/posix/paint.c
                    ${LH_LIB_SRC_PATH}/os/system/posix/window.c
                    ${LH_LIB_SRC_PATH}/os/system/posix/key.c
                    ${LH_LIB_SRC_PATH}/os/system/posix/input.c
                    ${LH_LIB_SRC_PATH}/os/system/posix/surface.c
            )
            list(APPEND LH_OS_SYSTEM_LINK_LIBS ${CMAKE_DL_LIBS})
        endif ()
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/os/render/backend/gdi.c)
    endif ()
endif ()

# Monocypher, the arithmetic behind lh/crypto/ed25519.h. Someone else's code,
# like SQLite below: built without warnings, its header private to lh.
set(LH_MONOCYPHER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/monocypher)
list(APPEND LH_LIB_SOURCES
        ${LH_MONOCYPHER_PATH}/monocypher.c
        ${LH_MONOCYPHER_PATH}/monocypher-ed25519.c
)
set_source_files_properties(
        ${LH_MONOCYPHER_PATH}/monocypher.c
        ${LH_MONOCYPHER_PATH}/monocypher-ed25519.c
        PROPERTIES COMPILE_OPTIONS "$<IF:$<C_COMPILER_ID:MSVC>,/w,-w>"
)

# UI model (color, paint, style, canvas, entity tree). OFF leaves it out.
# SQLite and its lh/sql backend (LH_LIBRARY_OPTION_SQL_SQLITE). The amalgamation
# is someone else's code: built with its own defines and without warnings.
set(LH_SQLITE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/sqlite)
if (LH_LIBRARY_OPTION_SQL_SQLITE)
    list(APPEND LH_LIB_SOURCES
            ${LH_LIB_SRC_PATH}/sql/sqlite.c
            ${LH_SQLITE_PATH}/sqlite3.c
    )
    set_source_files_properties(${LH_SQLITE_PATH}/sqlite3.c PROPERTIES
            COMPILE_DEFINITIONS "SQLITE_OMIT_LOAD_EXTENSION;SQLITE_THREADSAFE=1"
            COMPILE_OPTIONS "$<IF:$<C_COMPILER_ID:MSVC>,/w,-w>"
    )
endif ()

if (LH_LIBRARY_OPTION_UI)
    list(APPEND LH_LIB_SOURCES
            ${LH_LIB_SRC_PATH}/ui/axis.c
            ${LH_LIB_SRC_PATH}/ui/frame.c
            ${LH_LIB_SRC_PATH}/ui/frame/stats.c
            ${LH_LIB_SRC_PATH}/ui/insets.c
            ${LH_LIB_SRC_PATH}/ui/key.c
            ${LH_LIB_SRC_PATH}/ui/layout/layout.c
            ${LH_LIB_SRC_PATH}/ui/layout/run.c
            ${LH_LIB_SRC_PATH}/ui/layout/cross.c
            ${LH_LIB_SRC_PATH}/ui/layout/place.c
            ${LH_LIB_SRC_PATH}/ui/color.c
            ${LH_LIB_SRC_PATH}/ui/paint.c
            ${LH_LIB_SRC_PATH}/ui/gradient.c
            ${LH_LIB_SRC_PATH}/ui/gradient/sampler.c
            ${LH_LIB_SRC_PATH}/ui/gradient/sampler/lerp.c
            ${LH_LIB_SRC_PATH}/ui/gradient/sine.c
            ${LH_LIB_SRC_PATH}/ui/style.c
            ${LH_LIB_SRC_PATH}/ui/easing.c
            ${LH_LIB_SRC_PATH}/ui/transition.c
            ${LH_LIB_SRC_PATH}/ui/radius.c
            ${LH_LIB_SRC_PATH}/ui/radii.c
            ${LH_LIB_SRC_PATH}/ui/rects.c
            ${LH_LIB_SRC_PATH}/ui/scalar.c
            ${LH_LIB_SRC_PATH}/ui/shadow.c
            ${LH_LIB_SRC_PATH}/ui/shadow/ramp.c
            ${LH_LIB_SRC_PATH}/ui/shadow/shape.c
            ${LH_LIB_SRC_PATH}/ui/blur.c
            ${LH_LIB_SRC_PATH}/ui/blur/pass.c
            ${LH_LIB_SRC_PATH}/ui/blur/window.c
            ${LH_LIB_SRC_PATH}/ui/range.c
            ${LH_LIB_SRC_PATH}/ui/canvas.c
            ${LH_LIB_SRC_PATH}/ui/canvas/backend.c
            ${LH_LIB_SRC_PATH}/ui/canvas/state.c
            ${LH_LIB_SRC_PATH}/ui/canvas/round.c
            ${LH_LIB_SRC_PATH}/ui/canvas/box.c
            ${LH_LIB_SRC_PATH}/ui/canvas/clip.c
            ${LH_LIB_SRC_PATH}/ui/canvas/clip/round.c
            ${LH_LIB_SRC_PATH}/ui/pen.c
            ${LH_LIB_SRC_PATH}/ui/entity.c
            ${LH_LIB_SRC_PATH}/ui/entity/class.c
            ${LH_LIB_SRC_PATH}/ui/entity/event.c
            ${LH_LIB_SRC_PATH}/ui/entity/transform.c
            ${LH_LIB_SRC_PATH}/ui/container.c
            ${LH_LIB_SRC_PATH}/ui/decl.c
            ${LH_LIB_SRC_PATH}/ui/image.c
            ${LH_LIB_SRC_PATH}/ui/label.c
            ${LH_LIB_SRC_PATH}/ui/input.c
            ${LH_LIB_SRC_PATH}/ui/scrollbar.c
            ${LH_LIB_SRC_PATH}/ui/button.c
             ${LH_LIB_SRC_PATH}/ui/toggle.c
            ${LH_LIB_SRC_PATH}/ui/view.c
            ${LH_LIB_SRC_PATH}/ui/view/controller.c
            ${LH_LIB_SRC_PATH}/ui/view/focus/walk.c
            ${LH_LIB_SRC_PATH}/ui/view/pointer.c
            ${LH_LIB_SRC_PATH}/ui/mask.c
            ${LH_LIB_SRC_PATH}/ui/font.c
             ${LH_LIB_SRC_PATH}/ui/font/range.c
            ${LH_LIB_SRC_PATH}/ui/text.c
            ${LH_LIB_SRC_PATH}/ui/text/align.c
            ${LH_LIB_SRC_PATH}/ui/text/line.c
            ${LH_LIB_SRC_PATH}/ui/text/block.c
            ${LH_LIB_SRC_PATH}/ui/canvas/mask.c
            ${LH_LIB_SRC_PATH}/ui/pixmap.c
            ${LH_LIB_SRC_PATH}/ui/canvas/sw.c
            ${LH_LIB_SRC_PATH}/ui/canvas/sw/shadow/pass.c
            ${LH_LIB_SRC_PATH}/ui/canvas/web.c
            ${LH_LIB_SRC_PATH}/ui/canvas/web/glyphs.c
            ${LH_LIB_SRC_PATH}/ui/canvas/web/atlas.c
            ${LH_LIB_SRC_PATH}/ui/present.c
            ${LH_LIB_SRC_PATH}/ui/present/sink.c
            ${LH_LIB_SRC_PATH}/ui/display.c
            ${LH_LIB_SRC_PATH}/ui/viewport3d.c
            ${LH_LIB_SRC_PATH}/ui/web/ws.c
            ${LH_LIB_SRC_PATH}/ui/web/buf.c
            ${LH_LIB_SRC_PATH}/ui/web/js.c
            ${LH_LIB_SRC_PATH}/ui/web/js/x.c
            ${LH_LIB_SRC_PATH}/ui/web/x.c
            ${LH_LIB_SRC_PATH}/ui/web/html.c
            ${LH_LIB_SRC_PATH}/ui/web/html/attr.c
            ${LH_LIB_SRC_PATH}/ui/web/page.c
    )
    if (LH_LIBRARY_OPTION_MATH_FPU)
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/ui/renderer3d.c
             ${LH_LIB_SRC_PATH}/ui/renderer3d/camera.c)
    endif ()
    if (LH_LIBRARY_OPTION_OS)
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/ui/web/host.c)
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/ui/web/host/client.c)
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/ui/web/host/frame.c)
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/ui/web/host/hooks.c)
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/ui/web/host/stream.c)
    endif ()
    if (LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW)
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/ui/surface.c)
    endif ()
    if (LH_LIBRARY_OPTION_UI_FONT_ROBOTO)
        list(APPEND LH_LIB_SOURCES ${LH_LIB_SRC_PATH}/ui/font/roboto.c)
    endif ()
endif ()

# Font-only: Roboto + mask/font without the widget/canvas tree (STM TFT, вЂ¦).
if (LH_LIBRARY_OPTION_UI_FONT_ROBOTO AND NOT LH_LIBRARY_OPTION_UI)
    list(APPEND LH_LIB_SOURCES
            ${LH_LIB_SRC_PATH}/ui/mask.c
            ${LH_LIB_SRC_PATH}/ui/font.c
            ${LH_LIB_SRC_PATH}/ui/font/range.c
            ${LH_LIB_SRC_PATH}/ui/font/roboto.c
    )
endif ()

# Float math (vec, quat, mat4) is the FPU. OFF leaves it out of the library.
#
# lh/math/float (sqrt, sin_cos, tan, round, finite) is part of it: built only with the option ON,
# like vec* and quat that call it, and point.c includes lh/math/float/round.h only
# under the same option. With it OFF nothing in the library asks for a float
# operation, so neither an FPU nor libm is needed.
if (LH_LIBRARY_OPTION_MATH_FPU)
    list(APPEND LH_LIB_SOURCES
            ${LH_LIB_SRC_PATH}/math/float/sqrt.c
            ${LH_LIB_SRC_PATH}/math/float/sin_cos.c
            ${LH_LIB_SRC_PATH}/math/float/round.c
            ${LH_LIB_SRC_PATH}/math/float/finite.c
            ${LH_LIB_SRC_PATH}/math/float/tan.c
            ${LH_LIB_SRC_PATH}/math/slice.c
            ${LH_LIB_SRC_PATH}/math/view.c
            ${LH_LIB_SRC_PATH}/math/fpoint.c
            ${LH_LIB_SRC_PATH}/math/fsize.c
            ${LH_LIB_SRC_PATH}/math/frect.c
            ${LH_LIB_SRC_PATH}/math/vec2.c
            ${LH_LIB_SRC_PATH}/math/vec3.c
            ${LH_LIB_SRC_PATH}/math/vec4.c
            ${LH_LIB_SRC_PATH}/math/quat.c
            ${LH_LIB_SRC_PATH}/math/mat4.c
    )
    # Square roots go through __builtin_sqrtf (src/lh/math/float/sqrt.c). Without
    # -fno-math-errno GCC/Clang keep a call to libm's sqrtf next to the CPU
    # instruction, only to set errno on negative input: a dependency a
    # freestanding build cannot satisfy, for a value lh never reads. The flag is
    # a property of these sources only -- never of the target's PUBLIC/INTERFACE
    # options -- so code that includes lh/math/float/*.h builds with its own flags;
    # vec*.c call lh_math_float_sqrt and no longer need it.
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
        set_source_files_properties(
                ${LH_LIB_SRC_PATH}/math/float/sqrt.c
                ${LH_LIB_SRC_PATH}/math/float/sin_cos.c
                ${LH_LIB_SRC_PATH}/math/float/round.c
                PROPERTIES COMPILE_OPTIONS -fno-math-errno
        )
    endif ()
endif ()

# lh is a static library only: it is linked into its host, never loaded as a
# DLL / shared object.
add_library(${PROJECT_NAME} STATIC ${LH_LIB_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
        POSITION_INDEPENDENT_CODE ${LH_POSITION_INDEPENDENT_CODE})

# Under -fPIC GCC treats every exported function as interposable and so never
# inlines one, not even into its own file. lh is a static archive, nothing
# interposes it: let the API calls between the bricks inline as without PIC
# (Clang assumes this already).
if (LH_POSITION_INDEPENDENT_CODE AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-semantic-interposition)
endif ()

# No identical code folding. lh has many one-line accessors that compile to the
# same instructions on different types вЂ” lh_list_node_get_prev and
# lh_memory_tree_header_get_parent both load a pointer at offset 4. GCC's IPA-ICF
# merges such functions; GCC 11 with LTO then inlines the merged body with the
# other type's alias set, so a list-node access reads as a tree-header access
# that "cannot" overlap the list. In lh_memory_tree_realloc that dropped the loop
# re-parenting the children: they kept pointing at the freed block (seen with
# GCC 11.4 -O3 + LTO, i386; not with GCC 16). Known class of GCC bugs: PR 113907,
# PR 106921, PR 118924. The flag is read per function at compile time, so it
# also holds when lh is folded into a consumer's LTO link.
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-ipa-icf)
endif ()

# Expose generated and public include directories to consumers.
# The generated one comes FIRST on purpose: `#include <lh/config.h>` has to find
# this build tree's header, not one that happens to sit in the source tree.
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/include
        ${LH_LIB_INCLUDE_PATH}
        # Library-private headers under src/lh/, included as <lh/...>.
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Kernel backend's system libraries (chosen with LH_OS_SYSTEM_BACKEND above).
target_include_directories(${PROJECT_NAME} PRIVATE ${LH_MONOCYPHER_PATH})

if (LH_LIBRARY_OPTION_OS AND LH_OS_SYSTEM_LINK_LIBS)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LH_OS_SYSTEM_LINK_LIBS})
endif ()

# Both OS workers and SQLite need the POSIX thread runtime independently.
if (NOT WIN32 AND (LH_LIBRARY_OPTION_OS OR LH_LIBRARY_OPTION_SQL_SQLITE))
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
endif ()

if (LH_LIBRARY_OPTION_SQL_SQLITE)
    target_include_directories(${PROJECT_NAME} PRIVATE ${LH_SQLITE_PATH})
endif ()

# ---------------------------------------------------------------------------
# Option: LH_STATIC_LIBGCC
#
# Hosts that ship an older libgcc_s (HLDS is the usual case) reject a module
# built against a newer GCC with `version GCC_7.0.0 not found`. Linking the
# runtime into the dependent with -static-libgcc / -static-libstdc++ avoids
# that. lh itself is a static archive; INTERFACE forwards the flags to whoever
# links it (e.g. authum_mm.so). No effect on MSVC.
# ---------------------------------------------------------------------------
option(LH_STATIC_LIBGCC
        "Pass -static-libgcc and -static-libstdc++ to dependents (GCC/Clang). For hosts with an older libgcc_s."
        OFF)
if (LH_STATIC_LIBGCC)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_link_options(${PROJECT_NAME} INTERFACE -static-libgcc -static-libstdc++)
        message(STATUS "LH_STATIC_LIBGCC=ON: dependents get -static-libgcc -static-libstdc++")
    else ()
        message(STATUS "LH_STATIC_LIBGCC=ON ignored on ${CMAKE_C_COMPILER_ID}")
    endif ()
endif ()

# Strong release optimizations for the core library (optional; Release only).
if (LH_ENABLE_RELEASE_MAX_OPT)
    if (MSVC)
        # /GL and /LTCG come from LH_LIBRARY_OPTION_LTO below, not from here.
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:/O2 /Oi /Ot /Gy>
        )
    else ()
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:-O3 -DNDEBUG -ffunction-sections -fdata-sections>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:-Wl,--gc-sections>
        )
    endif ()
endif ()

# Link-time optimization (LH_LIBRARY_OPTION_LTO, cmake/library_options.cmake).
# Decided before test/ and bench/ are added: when the archive holds compiler
# IR, its consumers link with IPO too (clang only loads its LTO plugin under
# -flto; GCC's driver loads its own either way).
if (NOT LH_LIBRARY_OPTION_LTO MATCHES "^(OFF|ON|FAT)$")
    message(FATAL_ERROR "LH_LIBRARY_OPTION_LTO must be OFF, ON or FAT (got '${LH_LIBRARY_OPTION_LTO}')")
endif ()
set(LH_IPO_SUPPORTED FALSE)
if (NOT LH_LIBRARY_OPTION_LTO STREQUAL "OFF")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LH_IPO_SUPPORTED LANGUAGES C)
    if (LH_IPO_SUPPORTED)
        set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    else ()
        message(STATUS "LH_LIBRARY_OPTION_LTO=${LH_LIBRARY_OPTION_LTO}: the C toolchain has no LTO; building without it.")
    endif ()
endif ()
if (LH_IPO_SUPPORTED AND LH_LIBRARY_OPTION_LTO STREQUAL "FAT")
    set(LH_LTO_FAT FALSE)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(LH_LTO_FAT TRUE)
    elseif (CMAKE_C_COMPILER_ID MATCHES "Clang" AND NOT WIN32 AND NOT APPLE
            AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 17)
        include(CheckCCompilerFlag)
        check_c_compiler_flag(-ffat-lto-objects LH_C_HAS_FAT_LTO_OBJECTS)
        set(LH_LTO_FAT ${LH_C_HAS_FAT_LTO_OBJECTS})
    endif ()
    if (LH_LTO_FAT)
        # After CMake's own IPO flags (GCC gets -fno-fat-lto-objects from them),
        # so this one wins.
        target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-ffat-lto-objects>)
        message(STATUS "LH_LIBRARY_OPTION_LTO=FAT: archive holds IR plus machine code.")
    else ()
        message(STATUS "LH_LIBRARY_OPTION_LTO=FAT: no fat LTO objects on ${CMAKE_C_COMPILER_ID} "
                "${CMAKE_C_COMPILER_VERSION}; using ON (IR only, hosts must link with LTO).")
    endif ()
endif ()

# Tests (after ${PROJECT_NAME} exists for target_link_libraries).
if (LH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()

# Benchmarks (after ${PROJECT_NAME} exists for target_link_libraries).
if (LH_BUILD_BENCH)
    add_subdirectory(bench)
endif ()

if (LH_BUILD_EXAMPLES)
    add_subdirectory(example)
endif ()
