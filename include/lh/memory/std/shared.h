/**
 * @file shared.h
 * @brief Shared includes/macros for ::lh_memory_std_* (src/lh/memory/std/).
 */
#ifndef LH_MEMORY_STD_SHARED_H
#define LH_MEMORY_STD_SHARED_H

#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/byte.h>
#include <lh/byte/ptr.h>
#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/compiler/type.h>
#include <lh/compiler/arch.h>
#include <lh/compiler/arch/family.h>
#include <lh/config.h>
#include <lh/cpu/cache.h>
#include <lh/cpu/simd.h>
#include <lh/cpu/vendor.h>
#include <lh/null.h>
#include <lh/numeric/fixed/types.h>
#include <lh/bit.h>
#include <lh/bit/bswap.h>
#include <lh/bit/scan.h>
#include <lh/util/memory.h>
#include <lh/util/ptr.h>

#if LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC
#    include <string.h>
#endif

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
#    include <immintrin.h>
#    if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#        include <intrin.h>
#    endif
#endif

#if LH_COMPILER_TYPE_IS_GCC_LIKE
#    define LH_MEMORY_STD_SIMD_TARGET(isa) __attribute__((target(isa)))
#else
#    define LH_MEMORY_STD_SIMD_TARGET(isa)
#endif

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 && (LH_COMPILER_ARCH == LH_COMPILER_ARCH_64)
#    define LH_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD                                           \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD))
#    if LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD > 512
#        error "LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD must be <= 512"
#    endif
#endif

#if (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC) && LH_COMPILER_ARCH_FAMILY_IS_X86
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 1
#    include <intrin.h>
#else
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 0
#endif

#if LH_COMPILER_TYPE_IS_GCC_LIKE && LH_COMPILER_ARCH_FAMILY_IS_X86
#    define LH_MEMORY_STD_HAVE_GCC_REP_MOVSB 1
#    define LH_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD                                                  \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD))
#else
#    define LH_MEMORY_STD_HAVE_GCC_REP_MOVSB 0
#endif

#if LH_COMPILER_ARCH_FAMILY_IS_X86
#    define LH_MEMORY_STD_ERMS_COPY_THRESHOLD                                                      \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_ERMS_COPY_THRESHOLD))
#endif

#define LH_MEMORY_STD_SCAN_BLOCK (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK))

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
#    define LH_MEMORY_STD_KIND_AVX2 1U
#    define LH_MEMORY_STD_KIND_SSE2 2U
#    define LH_MEMORY_STD_KIND_SCALAR 3U
#    define LH_MEMORY_STD_KIND_SSSE3 4U
#    define LH_MEMORY_STD_PREFETCH_TRIGGER                                                         \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_TRIGGER))
#    define LH_MEMORY_STD_PREFETCH_DISTANCE                                                        \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_DISTANCE))
#    define LH_MEMORY_STD_SIMD_COPY_THRESHOLD                                                      \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD))
#    define LH_MEMORY_STD_SIMD_COPY_REV_THRESHOLD                                                  \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD))
#    define LH_MEMORY_STD_SIMD_RCOPY_THRESHOLD                                                     \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD))
#    define LH_MEMORY_STD_SIMD_SET_THRESHOLD                                                       \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_SIMD_SET_THRESHOLD))
#    define LH_MEMORY_STD_SIMD_XOR_THRESHOLD                                                       \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_SIMD_XOR_THRESHOLD))
#    define LH_MEMORY_STD_STREAM_FLOOR                                                             \
        (lh_cast_static(lh_usize_t, LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD))
#    define LH_MEMORY_STD_STREAM_COPY_TRAFFIC 2U
#    define LH_MEMORY_STD_STREAM_SET_TRAFFIC 1U
#endif

#endif /* LH_MEMORY_STD_SHARED_H */
