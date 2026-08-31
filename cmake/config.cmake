# ---------------------------------------------------------------------------
# lh_generate_config()
#
# Runs configure_file on include/lh/config.h.in -> include/lh/config.h
# so #include <lh/config.h> sees a single path in the source tree.
#
# Non-CMake: copy config.h.in to config.h and substitute placeholders
# (see comments in the .in file). Placeholder names must match set() names below.
# ---------------------------------------------------------------------------
function(lh_generate_config)
    if (DEFINED ENV{USER} AND NOT "$ENV{USER}" STREQUAL "")
        set(LH_COMPILE_BY "$ENV{USER}")
    elseif (DEFINED ENV{USERNAME} AND NOT "$ENV{USERNAME}" STREQUAL "")
        set(LH_COMPILE_BY "$ENV{USERNAME}")
    else ()
        set(LH_COMPILE_BY "unknown")
    endif ()

    execute_process(
            COMMAND hostname
            OUTPUT_VARIABLE LH_COMPILE_HOST
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )
    if ("${LH_COMPILE_HOST}" STREQUAL "")
        set(LH_COMPILE_HOST "unknown-host")
    endif ()

    string(REPLACE "\\" "\\\\" LH_COMPILE_BY "${LH_COMPILE_BY}")
    string(REPLACE "\"" "\\\"" LH_COMPILE_BY "${LH_COMPILE_BY}")
    string(REPLACE "\\" "\\\\" LH_COMPILE_HOST "${LH_COMPILE_HOST}")
    string(REPLACE "\"" "\\\"" LH_COMPILE_HOST "${LH_COMPILE_HOST}")

    if (LH_BUILD_SHARED)
        set(ValShared 1)
    else ()
        set(ValShared 0)
    endif ()
    if (LH_LIBRARY_OPTION_THREAD_LOCAL)
        set(ValThreadLocal 1)
    else ()
        set(ValThreadLocal 0)
    endif ()
    if (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB)
        set(ValRtStdlib 1)
    else ()
        set(ValRtStdlib 0)
    endif ()
    if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB)
        set(ValAllocatorStdlib 1)
    else ()
        set(ValAllocatorStdlib 0)
    endif ()
    if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED)
        set(ValAllocatorInitAllocated 1)
    else ()
        set(ValAllocatorInitAllocated 0)
    endif ()

    set(ValAllocatorDefaultAlloc "${LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC}")
    set(ValAllocatorDefaultDealloc "${LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC}")
    set(ValAllocatorDefaultInclude "${LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE}")

    set(ValCatchStackMax "${LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX}")

    set(ValVectorInitialCapacity "${LH_LIBRARY_OPTION_VECTOR_INITIAL_CAPACITY}")
    set(ValVectorGrowthFactor "${LH_LIBRARY_OPTION_VECTOR_GROWTH_FACTOR}")

    set(_in "${CMAKE_CURRENT_SOURCE_DIR}/include/lh/config.h.in")
    set(_out "${CMAKE_CURRENT_SOURCE_DIR}/include/lh/config.h")
    configure_file("${_in}" "${_out}" @ONLY)
endfunction()
