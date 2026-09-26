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

    if (LH_LIBRARY_OPTION_THREAD_LOCAL)
        set(ValThreadLocal 1)
    else ()
        set(ValThreadLocal 0)
    endif ()
    if (LH_LIBRARY_OPTION_OS)
        set(ValOs 1)
    else ()
        set(ValOs 0)
    endif ()
    if (LH_LIBRARY_OPTION_OS_WERROR)
        set(ValOsWerror 1)
    else ()
        set(ValOsWerror 0)
    endif ()
    if (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB)
        set(ValRtStdlib 1)
    else ()
        set(ValRtStdlib 0)
    endif ()
    if (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT STREQUAL "NONE")
        set(ValRtCheckReport 0)
    elseif (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT STREQUAL "LOCATION")
        set(ValRtCheckReport 1)
    elseif (LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT STREQUAL "FULL")
        set(ValRtCheckReport 2)
    else ()
        message(FATAL_ERROR "LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT must be NONE, LOCATION or FULL "
                "(got '${LH_LIBRARY_OPTION_RUNTIME_CHECK_REPORT}')")
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
    set(ValAllocatorDefaultRealloc "${LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC}")
    set(ValAllocatorDefaultInclude "${LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE}")

    set(ValCatchStackMax "${LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX}")

    set(ValVectorInitialCapacity "${LH_LIBRARY_OPTION_VECTOR_INITIAL_CAPACITY}")
    set(ValVectorGrowthFactor "${LH_LIBRARY_OPTION_VECTOR_GROWTH_FACTOR}")

    set(ValAlgorithmCompareBlock "${LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK}")

    if (LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE)
        set(ValStrCaseMapUseTable 1)
    else ()
        set(ValStrCaseMapUseTable 0)
    endif ()

    if (LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE)
        set(ValWstrCaseMapUseTable 1)
    else ()
        set(ValWstrCaseMapUseTable 0)
    endif ()

    if (LH_SIMD_HAVE_SSE2)
        set(ValSimdHaveSse2 1)
    else ()
        set(ValSimdHaveSse2 0)
    endif ()

    if (LH_SIMD_HAVE_SSSE3)
        set(ValSimdHaveSsse3 1)
    else ()
        set(ValSimdHaveSsse3 0)
    endif ()

    if (LH_SIMD_HAVE_AVX2)
        set(ValSimdHaveAvx2 1)
    else ()
        set(ValSimdHaveAvx2 0)
    endif ()

    set(ValMemoryStdSimdMinThreshold "${LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD}")
    set(ValMemoryStdSimdSetThreshold "${LH_LIBRARY_OPTION_MEMORY_STD_SIMD_SET_THRESHOLD}")
    set(ValMemoryStdSimdXorThreshold "${LH_LIBRARY_OPTION_MEMORY_STD_SIMD_XOR_THRESHOLD}")
    set(ValMemoryStdSimdDirectDispatchThreshold "${LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD}")
    set(ValMemoryStdSimdStreamThreshold "${LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD}")
    set(ValMemoryStdGccRepMovsbThreshold "${LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD}")
    set(ValMemoryStdPrefetchTrigger "${LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_TRIGGER}")
    set(ValMemoryStdPrefetchDistance "${LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_DISTANCE}")

    set(_in "${CMAKE_CURRENT_SOURCE_DIR}/include/lh/config.h.in")
    set(_out "${CMAKE_CURRENT_SOURCE_DIR}/include/lh/config.h")
    configure_file("${_in}" "${_out}" @ONLY)
endfunction()
