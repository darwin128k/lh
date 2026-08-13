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
