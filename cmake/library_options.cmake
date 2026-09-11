# -----------------------------------------------------------------------------
# lh — library preprocessor options (LH_LIBRARY_OPTION_*)
#
# Values are written to include/lh/config.h via config.h.in (cmake/config.cmake).
# Non-CMake: copy include/lh/config.h.in to config.h and substitute @...@ tokens.
# library_option_fallback.h reads config.h for the effective values.
# Numeric LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX: CACHE STRING here.
#
# C symbols: lh/library_option.h (LH_LIBRARY_OPTION_ON / OFF).
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# LH_LIBRARY_OPTION_SHARED_BUILD
#
# Written to include/lh/config.h (0 or 1) from LH_BUILD_SHARED in
# CMakeLists.txt. Dependents inherit the same config via PUBLIC include path.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Option: LH_LIBRARY_OPTION_THREAD_LOCAL
#
# Controls whether LH_ATTRIBUTE_THREAD_LOCAL expands to real thread-local
# storage in library sources (see lh/attribute/thread_local.h).
#
#   ON  — per-thread instances of decorated statics (compiler TLS support required).
#   OFF — macro expands to nothing; same address space for all threads.
#
# Use ON if lh internals use TLS (e.g. runtime state); 
# keep OFF for minimal environments or when the app serializes access itself.
# -----------------------------------------------------------------------------
option(LH_LIBRARY_OPTION_THREAD_LOCAL
        "LH_ATTRIBUTE_THREAD_LOCAL uses real TLS in lh sources."
        ON)

# -----------------------------------------------------------------------------
# Option: LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB
#
# Default handler for lh_runtime_terminate() (lh/runtime_terminate.c).
#
#   ON  — initial callback is abort() from <stdlib.h>.
#   OFF — initial callback is null; call lh_runtime_terminate_set() before use
#         or rely on your own first install.
#
# Runtime override: lh_runtime_terminate_set() replaces the handler regardless.
# -----------------------------------------------------------------------------
option(LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB
        "Default lh_runtime_terminate handler is abort(); OFF leaves it unset."
        ON)

# -----------------------------------------------------------------------------
# Option: LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB
#
# Initial default for runtime allocator callbacks in lh/runtime/allocator.c.
#
#   ON  — runtime allocator is initialized with malloc/free from <stdlib.h>.
#   OFF — runtime allocator starts as null and must be installed by user code.
# -----------------------------------------------------------------------------
option(LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB
        "Initialize runtime allocator with malloc/free by default."
        ON)

# -----------------------------------------------------------------------------
# LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_{ALLOC,DEALLOC,INCLUDE}
#
# Function names (and the header declaring them) used to seed the runtime
# allocator when LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB is ON. Default
# to malloc/free/<stdlib.h>; override to bind a different pair at compile
# time (e.g. pvPortMalloc/vPortFree on FreeRTOS) without writing runtime
# lh_memory_allocator_set() init code.
#
# CMake: -DLH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC=pvPortMalloc or ccmake.
# Manual build: set in include/lh/config.h or -D on the compiler command line.
# -----------------------------------------------------------------------------
set(LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC "malloc" CACHE STRING
        "Function bound as the default runtime allocator's alloc_cb.")
set(LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC "free" CACHE STRING
        "Function bound as the default runtime allocator's dealloc_cb.")
set(LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE "<stdlib.h>" CACHE STRING
        "Header providing the default alloc/dealloc functions above.")

# -----------------------------------------------------------------------------
# Option: LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED
#
# Controls zero-initialization for memory returned by lh_memory_allocator_alloc().
#
#   ON  — newly allocated blocks are filled with 0 before return.
#   OFF — memory is returned as-is from allocator callback.
# -----------------------------------------------------------------------------
option(LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED
        "Zero-initialize allocated memory in lh_memory_allocator_alloc."
        ON)

# -----------------------------------------------------------------------------
# LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX
#
# Maximum active catch frames for lh_runtime_exception_catch_stack_* (array size
# in runtime/exception/catch/stack.c). Must be a positive decimal integer.
#
# CMake: -DLH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX=64 or ccmake.
# Manual build: set in include/lh/config.h or -D on the compiler command line.
# -----------------------------------------------------------------------------
set(LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX "16" CACHE STRING
        "Maximum depth of lh runtime exception catch stack (must be > 0).")

# -----------------------------------------------------------------------------
# LH_LIBRARY_OPTION_VECTOR_INITIAL_CAPACITY
#
# Capacity lh_vector_t grows to from empty on its first push_back/insert
# (src/lh/vector.c). Must be a positive decimal integer.
#
# CMake: -DLH_LIBRARY_OPTION_VECTOR_INITIAL_CAPACITY=4 or ccmake.
# Manual build: set in include/lh/config.h or -D on the compiler command line.
# -----------------------------------------------------------------------------
set(LH_LIBRARY_OPTION_VECTOR_INITIAL_CAPACITY "1" CACHE STRING
        "Capacity lh_vector_t grows to from empty on first insertion (must be > 0).")

# -----------------------------------------------------------------------------
# LH_LIBRARY_OPTION_VECTOR_GROWTH_FACTOR
#
# Factor by which lh_vector_t capacity grows once full (src/lh/vector.c).
# Must be a decimal integer greater than 1.
#
# CMake: -DLH_LIBRARY_OPTION_VECTOR_GROWTH_FACTOR=3 or ccmake.
# Manual build: set in include/lh/config.h or -D on the compiler command line.
# -----------------------------------------------------------------------------
set(LH_LIBRARY_OPTION_VECTOR_GROWTH_FACTOR "2" CACHE STRING
        "Factor by which lh_vector_t capacity grows when full (must be > 1).")

# -----------------------------------------------------------------------------
# LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK
#
# Element block size lh_algorithm_compare (include/lh/util/algorithm.h) scans
# branchlessly before falling back to a precise, element-by-element scan of
# the block that turned out to differ — same block used by the single-element
# fast path in lh_memory_find_step (src/lh/memory.c). Larger values give the
# compiler more to auto-vectorize per block at the cost of a longer worst-case
# precise rescan when a block does contain a mismatch/hit; smaller values do
# the opposite. Must be a positive decimal integer.
#
# CMake: -DLH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK=32 or ccmake.
# Manual build: set in include/lh/config.h or -D on the compiler command line.
# -----------------------------------------------------------------------------
set(LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK "16" CACHE STRING
        "Block size for lh_algorithm_compare / lh_memory_find_step's branchless scan (must be > 0).")

# -----------------------------------------------------------------------------
# Option: LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
#
# Implementation lh_str_ptr_to_lower / lh_str_ptr_to_upper (src/lh/util/str/case/map.c)
# use for single-byte (0..255) simple case mapping:
#
#   ON  — direct 256-entry lookup table per direction (512 bytes total flash/ROM).
#         O(1) per character, no branch: table[(unsigned char)c] is always the
#         mapped byte (identity for characters with no case mapping).
#   OFF — binary search over the existing sorted sparse pair table (~112 bytes
#         total). O(log n) per character (~6 comparisons for the current ~56
#         entries per direction) — smaller, slower.
#
# Pure speed/flash trade-off, same shape as LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK:
# no way to know which matters more for a given target without measuring on it.
# -----------------------------------------------------------------------------
option(LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE
        "lh_str_ptr_to_lower/_to_upper use a dense 256-entry table (ON, faster, +~400B) instead of binary search over the sparse pair table (OFF, smaller)."
        ON)

# -----------------------------------------------------------------------------
# Option: LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE
#
# Implementation lh_wstr_ptr_to_lower / lh_wstr_ptr_to_upper (src/lh/util/wstr/case/map.c)
# use for the full-Unicode simple case mapping (~1.4k pairs across the whole UCD, vs ~56
# for the narrow LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE — too many, and too sparse across
# too wide a range, for a single dense table to be sane at any code point width):
#
#   ON  — two-level (block) table for the BMP (code points 0x0000..0xFFFF, i.e. every
#         lh_wchar_t value on a 16-bit wchar_t platform, and effectively all real-world
#         text on any platform): a 256-entry stage-1 index (one byte per 256-code-point
#         block; ~239 of 256 blocks have no case mapping at all and collapse to a single
#         sentinel) into a stage-2 table holding only the ~17-20 blocks that actually have
#         mappings. O(1) per character. Falls back to a small binary search (~282 entries,
#         supplementary-plane scripts with case pairs — Deseret, Osage, Adlam, ...) only for
#         code points above the BMP, which requires a 32-bit wchar_t to even reach.
#   OFF — binary search over the full sorted sparse pair table (~1.4k entries per
#         direction). O(log n) per character (~11 comparisons).
#
# Unlike LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE, this is not really a size/speed
# trade-off: each OFF pair entry is two lh_uchar32_t (8 bytes) — the existing sparse
# table alone already costs ~23KB (both directions); the ON stage-1/stage-2 tables plus
# their much-smaller supplementary-plane fallback cost about the same, ~23.5KB. ON is
# close to a strict win here; kept as an option mainly for code-size (less lookup logic)
# on the very tightest targets, and for symmetry with the narrow option above.
# -----------------------------------------------------------------------------
option(LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE
        "lh_wstr_ptr_to_lower/_to_upper use a two-level BMP block table (ON, faster, ~same size) instead of binary search over the full sparse pair table (OFF, simpler code)."
        ON)
