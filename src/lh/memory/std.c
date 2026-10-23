/**
 * @file std.c
 * @brief ::lh_memory_std_* — public entry points; folders under std/ are the namespaces.
 *
 * One translation unit: pieces share static path-choice state. CMake lists only
 * this file; words and x86 pieces below are included, not separate link units.
 */
#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/arch/family.h>
#include <lh/config.h>
#include <lh/cpu/cache.h>
#include <lh/cpu/simd.h>
#include <lh/cpu/vendor.h>
#include <lh/bit/bswap.h>
#include <lh/bit/ops.h>
#include <lh/bit/scan.h>
#include <lh/bit/swar.h>
#include <lh/memory/word.h>
#include <lh/util/addr.h>
#include <lh/util/memory.h>

#if LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC
#    include <string.h>
#endif

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
#    include <immintrin.h>
#    if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#        include <intrin.h>
#    endif
#endif

/** @brief Compile one function for instruction set @p isa (GCC / Clang `target`). */
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

/** @brief 1 when MSVC's `__movsb` intrinsic (`REP MOVSB`) is available. */
#if (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC) && LH_COMPILER_ARCH_FAMILY_IS_X86
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 1
#    include <intrin.h>
#else
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 0
#endif

/** @brief 1 when `REP MOVSB` is reachable through GCC / Clang inline assembly. */
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

#include "std/words.c"
#include "std/x86/cpuid.c"
#include "std/x86/sse2.c"
#include "std/x86/avx2.c"

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static lh_byte_t m_simd_kind;
static lh_byte_t m_copy_rev_kind;

#    if !LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC

typedef lh_void (*lh_memory_std_copy_simd_fn)(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n);
typedef lh_void (*lh_memory_std_copy_rev_simd_fn)(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n);
typedef lh_void (*lh_memory_std_rcopy_simd_fn)(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n);
typedef lh_void (*lh_memory_std_set_simd_fn)(lh_byte_ptr dst, lh_byte_t val, lh_usize_t n);
typedef const lh_ptr (*lh_memory_std_compare_fn)(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);
typedef const lh_ptr (*lh_memory_std_rcompare_fn)(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);

static lh_void
lh_memory_std_copy_simd_dispatch(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n);
static lh_void
lh_memory_std_copy_rev_simd_dispatch(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n);
static lh_void
lh_memory_std_rcopy_simd_dispatch(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n);
static lh_void
lh_memory_std_set_simd_dispatch(lh_byte_ptr dst, lh_byte_t val, lh_usize_t n);
static const lh_ptr
lh_memory_std_compare_dispatch(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);
static const lh_ptr
lh_memory_std_rcompare_dispatch(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);

static lh_memory_std_copy_simd_fn m_copy_simd_impl = lh_memory_std_copy_simd_dispatch;
static lh_memory_std_copy_simd_fn m_copy_stream_impl = lh_null;
static lh_memory_std_copy_rev_simd_fn m_copy_rev_simd_impl = lh_memory_std_copy_rev_simd_dispatch;
static lh_memory_std_rcopy_simd_fn m_rcopy_simd_impl = lh_memory_std_rcopy_simd_dispatch;
static lh_memory_std_set_simd_fn m_set_simd_impl = lh_memory_std_set_simd_dispatch;
static lh_memory_std_set_simd_fn m_set_stream_impl = lh_null;
static lh_memory_std_compare_fn m_compare_impl = lh_memory_std_compare_dispatch;
static lh_memory_std_rcompare_fn m_rcompare_impl = lh_memory_std_rcompare_dispatch;

#    endif /* !LIBC */
#endif     /* SSE2 || AVX2 */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static lh_void
lh_memory_std_copy_simd_dispatch(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_copy_simd_impl = lh_memory_std_copy_avx2;
        m_copy_stream_impl = lh_memory_std_copy_avx2_stream;
        m_simd_kind = LH_MEMORY_STD_KIND_AVX2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_copy_simd_impl = lh_memory_std_copy_sse2;
        m_copy_stream_impl = lh_memory_std_copy_sse2_stream;
        m_simd_kind = LH_MEMORY_STD_KIND_SSE2;
    }
    else
#    endif
    {
        m_copy_simd_impl = lh_memory_std_copy_simd_scalar;
        m_simd_kind = LH_MEMORY_STD_KIND_SCALAR;
    }

    /* Honour the stream threshold on this first call too — otherwise a first-ever
     * multi-MB copy would resolve the pointers and then run the plain SIMD tier. */
    if (lh_memory_std_is_stream_size(n, LH_MEMORY_STD_STREAM_COPY_TRAFFIC) &&
        m_copy_stream_impl != lh_null)
    {
        m_copy_stream_impl(dst, src, n);
    }
    else
    {
        m_copy_simd_impl(dst, src, n);
    }
}

#endif /* SIMD */

lh_ptr
lh_memory_std_copy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);

#if LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC
    (void)memcpy(dst, src, n);
#elif LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    /* See lh_memory_std_copy_simd_dispatch's doc comment above for why this tier —
     * not REP MOVSB — is the default whenever SIMD is compilable, for both GCC/Clang
     * and MSVC. Under MSVC, __movsb (below) was originally used unconditionally because
     * it beat MSVC's own scalar byte loop by 3x-15x; once the hand-written SIMD path
     * existed, the same Zen2 measurement that retired REP MOVSB under GCC also applies
     * here (ERMSB is not equally fast on every x86 vendor), so MSVC takes this branch
     * too whenever the intrinsics compiled in. */
    if (n >= LH_MEMORY_STD_SIMD_COPY_THRESHOLD)
    {
        lh_byte_ptr d = lh_ptr_cast(lh_byte_t, dst);
        lh_byte_cptr s = lh_ptr_ccast(lh_byte_t, src);

        if (lh_memory_std_is_stream_size(n, LH_MEMORY_STD_STREAM_COPY_TRAFFIC))
        {
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
            if (m_simd_kind == LH_MEMORY_STD_KIND_AVX2)
            {
                lh_memory_std_copy_avx2_stream(d, s, n);
            }
            else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
                if (m_simd_kind == LH_MEMORY_STD_KIND_SSE2)
            {
                lh_memory_std_copy_sse2_stream(d, s, n);
            }
            else
#    endif
            {
                m_copy_simd_impl(d, s, n);
            }
        }
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 && (LH_COMPILER_ARCH == LH_COMPILER_ARCH_64)
        /* 16-255: SSE2 overlapping stores, no AVX2 prologue. Measured faster than
         * routing this band through copy_avx2 (extra size-class branches, and on
         * this CPU two ymm overlapping 64-byte copies lost to four xmm). */
        else if (n < LH_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD)
        {
            lh_memory_std_copy_sse2_small(d, s, n);
        }
#    endif
#    if LH_LIBRARY_OPTION_MEMORY_STD_USE_ERMS &&                                                   \
        (LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB || LH_MEMORY_STD_HAVE_GCC_REP_MOVSB)
        /* Mid-size on Intel+ERMS: OWN `rep movsb` inside this same function — not
         * CRT memcpy, not a second copy entry. Beats SSE2 on Ivy Bridge at 64KB;
         * skipped on AMD (Zen2 SIMD won). Below DIRECT_DISPATCH and past the
         * stream floor the vector/NT tiers keep ownership. */
        else if (n >= LH_MEMORY_STD_ERMS_COPY_THRESHOLD && lh_memory_std_use_erms_copy())
        {
#        if LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB
            __movsb(lh_ptr_cast(unsigned char, d), lh_ptr_ccast(unsigned char, s), n);
#        else
            __asm__ volatile("rep movsb" : "+D"(d), "+S"(s), "+c"(n) : : "memory");
#        endif
        }
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
        else if (m_simd_kind == LH_MEMORY_STD_KIND_AVX2)
        {
            lh_memory_std_copy_avx2(d, s, n);
        }
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        else if (m_simd_kind == LH_MEMORY_STD_KIND_SSE2)
        {
            lh_memory_std_copy_sse2(d, s, n);
        }
#    endif
        else if (m_simd_kind == LH_MEMORY_STD_KIND_SCALAR)
        {
            lh_memory_std_copy_bytes(d, s, n);
        }
        else
        {
            m_copy_simd_impl(d, s, n);
        }
    }
    else
    {
        lh_memory_std_copy_bytes(lh_ptr_cast(lh_byte_t, dst), lh_ptr_ccast(lh_byte_t, src), n);
    }
#elif LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB
    /* Fallback for an MSVC x86 build where SIMD intrinsics turned out not to be
     * compilable at all (see cmake/check_simd.cmake) — the SIMD branch above wins
     * whenever it is available. __movsb's declared signature is unsigned char* /
     * const unsigned char* (fixed by <intrin.h>); lh_byte_t is a plain typedef of
     * unsigned char (lh/char.h), so this cast is the same reinterpretation either way. */
    __movsb(lh_ptr_cast(unsigned char, dst), lh_ptr_ccast(unsigned char, src), n);
#elif LH_MEMORY_STD_HAVE_GCC_REP_MOVSB
    if (n >= LH_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD)
    {
        lh_byte_ptr d = lh_ptr_cast(lh_byte_t, dst);
        lh_byte_cptr s = lh_ptr_ccast(lh_byte_t, src);

        /* No GCC/Clang builtin for REP MOVSB (unlike MSVC's __movsb) — inline asm is
         * the only way to ask for this instruction specifically. The three registers
         * it reads/advances (destination, source, count) are both inputs and outputs
         * (each is left pointing/counting past the copied range), and the compiler is
         * told the whole addressable memory may have changed ("memory" clobber),
         * since REP MOVSB writes through dst without that being visible from the
         * constraint list alone. Kept only as the fallback for a GCC-like x86 build
         * where SIMD intrinsics turned out not to be compilable at all (see
         * cmake/check_simd.cmake) — the branch above wins whenever it is available. */
        __asm__ volatile("rep movsb" : "+D"(d), "+S"(s), "+c"(n) : : "memory");
    }
    else
    {
        lh_memory_std_copy_bytes(lh_ptr_cast(lh_byte_t, dst), lh_ptr_ccast(lh_byte_t, src), n);
    }
#else
    /* No vector unit and no REP MOVSB on this target — the portable word kernel
       is the floor, not a byte loop. It is the same code the vector tiers are
       measured against, so a non-x86 build is not left an order of magnitude
       behind simply by lacking intrinsics. */
    (void)lh_memory_std_copy_words(dst, src, n);
#endif

    return end;
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static lh_void
lh_memory_std_copy_rev_simd_dispatch(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_copy_rev_simd_impl = lh_memory_std_copy_rev_avx2;
        m_copy_rev_kind = LH_MEMORY_STD_KIND_AVX2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSSE3
        if (lh_cpu_simd_has_ssse3())
    {
        m_copy_rev_simd_impl = lh_memory_std_copy_rev_ssse3;
        m_copy_rev_kind = LH_MEMORY_STD_KIND_SSSE3;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_copy_rev_simd_impl = lh_memory_std_copy_rev_sse2;
        m_copy_rev_kind = LH_MEMORY_STD_KIND_SSE2;
    }
    else
#    endif
    {
        m_copy_rev_simd_impl = lh_memory_std_copy_rev_simd_scalar;
        m_copy_rev_kind = LH_MEMORY_STD_KIND_SCALAR;
    }

    m_copy_rev_simd_impl(dst, src, n);
}

#endif /* SIMD */

lh_ptr
lh_memory_std_copy_rev(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (n >= LH_MEMORY_STD_SIMD_COPY_REV_THRESHOLD)
    {
        lh_byte_ptr d = lh_ptr_cast(lh_byte_t, dst);
        lh_byte_cptr s = lh_ptr_ccast(lh_byte_t, src);

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
        if (m_copy_rev_kind == LH_MEMORY_STD_KIND_AVX2)
        {
            lh_memory_std_copy_rev_avx2(d, s, n);
        }
        else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSSE3
            if (m_copy_rev_kind == LH_MEMORY_STD_KIND_SSSE3)
        {
            lh_memory_std_copy_rev_ssse3(d, s, n);
        }
        else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
            if (m_copy_rev_kind == LH_MEMORY_STD_KIND_SSE2)
        {
            lh_memory_std_copy_rev_sse2(d, s, n);
        }
        else
#    endif
            if (m_copy_rev_kind == LH_MEMORY_STD_KIND_SCALAR)
        {
            lh_memory_std_copy_rev_bytes(d, s, n);
        }
        else
        {
            m_copy_rev_simd_impl(d, s, n);
        }
    }
    else
#endif
    {
        lh_memory_std_copy_rev_bytes(dst, src, n);
    }

    return end;
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static lh_void
lh_memory_std_rcopy_simd_dispatch(lh_byte_ptr dst, lh_byte_cptr src, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_rcopy_simd_impl = lh_memory_std_rcopy_avx2;
        m_simd_kind = LH_MEMORY_STD_KIND_AVX2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_rcopy_simd_impl = lh_memory_std_rcopy_sse2;
        m_simd_kind = LH_MEMORY_STD_KIND_SSE2;
    }
    else
#    endif
    {
        m_rcopy_simd_impl = lh_memory_std_rcopy_simd_scalar;
        m_simd_kind = LH_MEMORY_STD_KIND_SCALAR;
    }

    m_rcopy_simd_impl(dst, src, n);
}

#endif /* SIMD */

lh_ptr
lh_memory_std_rcopy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

#if LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC
    /* rcopy is memmove's backward half: for dst above an overlapping src (its
     * documented use) and for disjoint ranges the result is the same. */
    (void)memmove(dst, src, n);
#elif LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (n >= LH_MEMORY_STD_SIMD_RCOPY_THRESHOLD)
    {
        lh_byte_ptr d = lh_ptr_cast(lh_byte_t, dst);
        lh_byte_cptr s = lh_ptr_ccast(lh_byte_t, src);

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
        if (m_simd_kind == LH_MEMORY_STD_KIND_AVX2)
        {
            lh_memory_std_rcopy_avx2(d, s, n);
        }
        else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
            if (m_simd_kind == LH_MEMORY_STD_KIND_SSE2)
        {
            lh_memory_std_rcopy_sse2(d, s, n);
        }
        else
#    endif
            if (m_simd_kind == LH_MEMORY_STD_KIND_SCALAR)
        {
            lh_memory_std_rcopy_bytes(d, s, n);
        }
        else
        {
            m_rcopy_simd_impl(d, s, n);
        }
    }
    else
    {
        lh_memory_std_rcopy_bytes(dst, src, n);
    }
#else
    lh_memory_std_rcopy_bytes(dst, src, n);
#endif
    return dst;
}

lh_ptr
lh_memory_std_move(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
#if LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    (void)memmove(dst, src, n);
    return lh_ptr_add_by_offset(lh_void, dst, n);
#else
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

#    if LH_MEMORY_STD_HAVE_MOVE_SMALL
    if (n < 16U)
    {
        lh_memory_std_copy_tiny(lh_ptr_cast(lh_byte_t, dst), lh_ptr_ccast(lh_byte_t, src), n);
        return lh_ptr_add_by_offset(lh_void, dst, n);
    }
    if (n <= 128U && lh_memory_std_has_sse2())
    {
        lh_memory_std_move_small_sse2(lh_ptr_cast(lh_byte_t, dst), lh_ptr_ccast(lh_byte_t, src), n);
        return lh_ptr_add_by_offset(lh_void, dst, n);
    }
#    endif

    const lh_ptr src_end = lh_ptr_add_by_offset(lh_void, src, n);
    if (lh_ptr_is_backward_copy(dst, src, src_end))
    {
        lh_memory_std_rcopy(dst, src, n);
        return lh_ptr_add_by_offset(lh_void, dst, n);
    }

    /* Forward, and dst inside [src - n, src): overlapping, so not the copy tiers
     * (see lh_memory_std_move_forward_sse2). lh_memory_std_copy_bytes walks
     * forward and reads its tail before writing it, so it is safe too. */
    if (lh_ptr_to_uaddr(dst) < lh_ptr_to_uaddr(src) &&
        lh_ptr_to_uaddr(src) - lh_ptr_to_uaddr(dst) < n)
    {
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (n > 16U && lh_memory_std_has_sse2())
        {
            lh_memory_std_move_forward_sse2(lh_ptr_cast(lh_byte_t, dst),
                                            lh_ptr_ccast(lh_byte_t, src), n);
            return lh_ptr_add_by_offset(lh_void, dst, n);
        }
#    endif
        lh_memory_std_copy_bytes(lh_ptr_cast(lh_byte_t, dst), lh_ptr_ccast(lh_byte_t, src), n);
        return lh_ptr_add_by_offset(lh_void, dst, n);
    }
    return lh_memory_std_copy(dst, src, n);
#endif
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static lh_void
lh_memory_std_set_simd_dispatch(lh_byte_ptr dst, lh_byte_t val, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_set_simd_impl = lh_memory_std_set_avx2;
        m_set_stream_impl = lh_memory_std_set_avx2_stream;
        m_simd_kind = LH_MEMORY_STD_KIND_AVX2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_set_simd_impl = lh_memory_std_set_sse2;
        m_set_stream_impl = lh_memory_std_set_sse2_stream;
        m_simd_kind = LH_MEMORY_STD_KIND_SSE2;
    }
    else
#    endif
    {
        m_set_simd_impl = lh_memory_std_set_simd_scalar;
        m_simd_kind = LH_MEMORY_STD_KIND_SCALAR;
    }

    if (lh_memory_std_is_stream_size(n, LH_MEMORY_STD_STREAM_SET_TRAFFIC) &&
        m_set_stream_impl != lh_null)
    {
        m_set_stream_impl(dst, val, n);
    }
    else
    {
        m_set_simd_impl(dst, val, n);
    }
}

#endif /* SIMD */

lh_ptr
lh_memory_std_set(lh_ptr dst, lh_byte_t val, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);
#if LH_LIBRARY_OPTION_MEMORY_STD_BACKEND_LIBC
    (void)memset(dst, val, n);
#elif LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (n >= LH_MEMORY_STD_SIMD_SET_THRESHOLD)
    {
        lh_byte_ptr d = lh_ptr_cast(lh_byte_t, dst);

        if (lh_memory_std_is_stream_size(n, LH_MEMORY_STD_STREAM_SET_TRAFFIC))
        {
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
            if (m_simd_kind == LH_MEMORY_STD_KIND_AVX2)
            {
                lh_memory_std_set_avx2_stream(d, val, n);
            }
            else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
                if (m_simd_kind == LH_MEMORY_STD_KIND_SSE2)
            {
                lh_memory_std_set_sse2_stream(d, val, n);
            }
            else
#    endif
            {
                m_set_simd_impl(d, val, n);
            }
        }
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
        else if (m_simd_kind == LH_MEMORY_STD_KIND_AVX2)
        {
            lh_memory_std_set_avx2(d, val, n);
        }
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        else if (m_simd_kind == LH_MEMORY_STD_KIND_SSE2)
        {
            lh_memory_std_set_sse2(d, val, n);
        }
#    endif
        else if (m_simd_kind == LH_MEMORY_STD_KIND_SCALAR)
        {
            lh_memory_std_set_bytes(d, val, n);
        }
        else
        {
            m_set_simd_impl(d, val, n);
        }
    }
    else
    {
        lh_memory_std_set_bytes(dst, val, n);
    }
#else
    lh_memory_std_set_bytes(dst, val, n);
#endif
    return end;
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static lh_void
lh_memory_std_xor_resolve_kind(void)
{
    if (lh_cpu_simd_has_avx2())
    {
        m_simd_kind = LH_MEMORY_STD_KIND_AVX2;
    }
    else
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_simd_kind = LH_MEMORY_STD_KIND_SSE2;
    }
    else
#    endif
    {
        m_simd_kind = LH_MEMORY_STD_KIND_SCALAR;
    }
}

#endif /* SIMD */

lh_ptr
lh_memory_std_xor(lh_ptr dst, const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_byte_ptr d = lh_ptr_cast(lh_byte_t, dst);
    lh_byte_cptr l = lh_ptr_ccast(lh_byte_t, lhs);
    lh_byte_cptr r = lh_ptr_ccast(lh_byte_t, rhs);

    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

#if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_math_ge(n, LH_MEMORY_STD_SIMD_XOR_THRESHOLD))
    {
        if (lh_math_is_zero(m_simd_kind))
        {
            lh_memory_std_xor_resolve_kind();
        }
        if (m_simd_kind == LH_MEMORY_STD_KIND_AVX2)
        {
            lh_memory_std_xor_avx2(d, l, r, n);
            return lh_ptr_add_unsafe(lh_void, dst, n);
        }
    }
#endif
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (n >= 32U && lh_memory_std_has_sse2())
    {
        lh_memory_std_xor_sse2(d, l, r, n);
        return lh_ptr_add_unsafe(lh_void, dst, n);
    }
#endif
    lh_memory_std_xor_bytes(d, l, r, n);
    return lh_ptr_add_unsafe(lh_void, dst, n);
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static const lh_ptr
lh_memory_std_compare_dispatch(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_compare_impl = lh_memory_std_compare_avx2;
        m_simd_kind = LH_MEMORY_STD_KIND_AVX2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_compare_impl = lh_memory_std_compare_sse2;
        m_simd_kind = LH_MEMORY_STD_KIND_SSE2;
    }
    else
#    endif
    {
        m_compare_impl = lh_memory_std_compare_scalar;
        m_simd_kind = LH_MEMORY_STD_KIND_SCALAR;
    }

    return m_compare_impl(lhs, rhs, n);
}

#endif /* SIMD */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

const lh_ptr
lh_memory_std_compare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (m_simd_kind == LH_MEMORY_STD_KIND_AVX2)
    {
        return lh_memory_std_compare_avx2(lhs, rhs, n);
    }
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (m_simd_kind == LH_MEMORY_STD_KIND_SSE2)
    {
        return lh_memory_std_compare_sse2(lhs, rhs, n);
    }
#    endif
    if (m_simd_kind == LH_MEMORY_STD_KIND_SCALAR)
    {
        return lh_memory_std_compare_bytes(lhs, rhs, n);
    }

    return m_compare_impl(lhs, rhs, n);
}

#else /* no SIMD */

const lh_ptr
lh_memory_std_compare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    return lh_memory_std_compare_bytes(lhs, rhs, n);
}

#endif /* SIMD */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static const lh_ptr
lh_memory_std_rcompare_dispatch(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_rcompare_impl = lh_memory_std_rcompare_avx2;
        m_simd_kind = LH_MEMORY_STD_KIND_AVX2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_rcompare_impl = lh_memory_std_rcompare_sse2;
        m_simd_kind = LH_MEMORY_STD_KIND_SSE2;
    }
    else
#    endif
    {
        m_rcompare_impl = lh_memory_std_rcompare_scalar;
        m_simd_kind = LH_MEMORY_STD_KIND_SCALAR;
    }

    return m_rcompare_impl(lhs, rhs, n);
}

#endif /* SIMD */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

const lh_ptr
lh_memory_std_rcompare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (m_simd_kind == LH_MEMORY_STD_KIND_AVX2)
    {
        return lh_memory_std_rcompare_avx2(lhs, rhs, n);
    }
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (m_simd_kind == LH_MEMORY_STD_KIND_SSE2)
    {
        return lh_memory_std_rcompare_sse2(lhs, rhs, n);
    }
#    endif
    if (m_simd_kind == LH_MEMORY_STD_KIND_SCALAR)
    {
        return lh_memory_std_rcompare_bytes(lhs, rhs, n);
    }

    return m_rcompare_impl(lhs, rhs, n);
}

#else /* no SIMD */

const lh_ptr
lh_memory_std_rcompare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    return lh_memory_std_rcompare_bytes(lhs, rhs, n);
}

#endif /* SIMD */

const lh_ptr
lh_memory_std_find(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size)
{
    lh_byte_cptr hay;
    lh_byte_cptr needle;

    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    if (rhs_size == 0U || lhs_size < rhs_size)
    {
        return lh_null;
    }
    hay = lh_ptr_ccast(lh_byte_t, lhs);
    needle = lh_ptr_ccast(lh_byte_t, rhs);
    if (rhs_size == 1U)
    {
        return lh_memory_std_find_scalar(hay, lhs_size, needle, 1U, 0U);
    }

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (lh_memory_std_has_sse2())
    {
        return lh_memory_std_find_sse2(hay, lhs_size, needle, rhs_size);
    }
#endif
    return lh_memory_std_find_scalar(hay, lhs_size, needle, rhs_size, 0U);
}

const lh_ptr
lh_memory_std_rfind(const lh_ptr lhs, lh_usize_t lhs_size, const lh_ptr rhs, lh_usize_t rhs_size)
{
    lh_byte_cptr hay;
    lh_byte_cptr needle;

    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    if (rhs_size == 0U || lhs_size < rhs_size)
    {
        return lh_null;
    }
    hay = lh_ptr_ccast(lh_byte_t, lhs);
    needle = lh_ptr_ccast(lh_byte_t, rhs);
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
    if (lh_memory_std_has_sse2())
    {
        return lh_memory_std_rfind_sse2(hay, lhs_size, needle, rhs_size);
    }
#endif
    return lh_memory_std_rfind_scalar(hay, lhs_size, needle, rhs_size);
}
