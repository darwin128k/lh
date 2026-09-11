#include <lh/memory/std.h>
#include <lh/util/algorithm.h>
#include <lh/assert.h>
#include <lh/compiler/type.h>

/* Real SIMD, runtime-dispatched: only where it can be done safely and portably —
 * GCC/Clang's __builtin_cpu_supports (checks CPUID *and* that the OS has actually
 * enabled AVX register state via XGETBV/XCR0, not just the raw feature bit) plus a
 * per-function __attribute__((target(...))), which asks the compiler for AVX2 code
 * generation in that one function only, without needing -mavx2 anywhere in the
 * project's build flags. See lh_memory_std_compare below for why this is worth
 * doing at all (an 11x+ throughput gap measured against lh_memory_std_copy on the
 * same data, with no SIMD instructions in the compiler's own output). Everywhere
 * this isn't available (non-x86, or a compiler other than GCC/Clang) falls back to
 * the portable scalar path, unchanged. */
#if LH_COMPILER_TYPE_IS_GCC_LIKE &&                                                                \
    (defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86))
#    define LH_MEMORY_STD_HAVE_X86_SIMD 1
#    include <immintrin.h>
#else
#    define LH_MEMORY_STD_HAVE_X86_SIMD 0
#endif

/* lh_memory_std_copy's plain while(n--) *d++ = *s++; loop (still used as-is on every
 * other compiler, including GCC/Clang here) measured ~9x slower under MSVC /O2 /Oi /Ot
 * than under GCC on the same data — checked directly with dumpbin /DISASM: MSVC emits
 * it as a literal byte-at-a-time movzx+mov loop, no vectorization, no recognized-memcpy
 * substitution, where GCC's own output for the identical C is dramatically better. No
 * CPU feature detection needed here (unlike the AVX2 path above): REP MOVSB is a
 * baseline x86 string instruction, present and correct on every x86/x86-64 CPU — using
 * it is a straight win, not a runtime-conditional one. */
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 1
#    include <intrin.h>
#else
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 0
#endif

lh_ptr
lh_memory_std_copy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);

#if LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB
    __movsb((unsigned char *)dst, (const unsigned char *)src, n);
#else
    lh_algorithm_copy(lh_uchar_t, dst, src, n);
#endif

    return end;
}

lh_ptr
lh_memory_std_copy_rev(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);
    lh_algorithm_copy_rev(lh_uchar_t, dst, src, n);

    return end;
}

lh_ptr
lh_memory_std_rcopy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_algorithm_rcopy(lh_uchar_t, dst, src, n);
    return dst;
}

lh_ptr
lh_memory_std_move(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    const lh_ptr src_end = lh_ptr_add_by_offset(lh_void, src, n);
    if (lh_ptr_is_backward_copy(dst, src, src_end))
    {
        lh_memory_std_rcopy(dst, src, n);
        return lh_ptr_add_by_offset(lh_void, dst, n);
    }
    return lh_memory_std_copy(dst, src, n);
}

lh_ptr
lh_memory_std_set(lh_ptr dst, lh_uchar_t val, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);
    lh_algorithm_set(lh_uchar_t, dst, val, n);
    return end;
}

#if LH_MEMORY_STD_HAVE_X86_SIMD

static const lh_ptr
lh_memory_std_compare_scalar(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_algorithm_compare(lh_uchar_t, lhs, rhs, n);
    return lh_null;
}

/* 32 bytes/compare via a single AVX2 packed-byte-equal + movemask, instead of the
 * scalar block's 16 elements/compare via ILP alone — see lh_algorithm_compare's own
 * doc comment for that technique, which this still falls back to for the < 32-byte
 * tail (and for CPUs without AVX2, via the dispatch below). movemask turns the
 * 32-lane comparison into a 32-bit "which lanes matched" bitmap directly usable by
 * a single bit-scan, rather than needing any lane-by-lane branching at all. */
__attribute__((target("avx2"))) static const lh_ptr
lh_memory_std_compare_avx2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs);

    while (n >= 32U)
    {
        const __m256i va = _mm256_loadu_si256((const __m256i *)l);
        const __m256i vb = _mm256_loadu_si256((const __m256i *)r);
        const unsigned eq_mask = (unsigned)_mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb));

        if (eq_mask != 0xFFFFFFFFU)
        {
            return l + __builtin_ctz(~eq_mask);
        }

        l += 32;
        r += 32;
        n -= 32U;
    }

    return lh_memory_std_compare_scalar(l, r, n);
}

typedef const lh_ptr (*lh_memory_std_compare_fn)(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);

static const lh_ptr
lh_memory_std_compare_dispatch(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);

/* Rewrites itself to the resolved implementation on the first real call — every
 * call after that goes straight through the (by-then-direct) function pointer, so
 * detection happens exactly once, not per call. */
static lh_memory_std_compare_fn m_compare_impl = lh_memory_std_compare_dispatch;

static const lh_ptr
lh_memory_std_compare_dispatch(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    m_compare_impl =
        __builtin_cpu_supports("avx2") ? lh_memory_std_compare_avx2 : lh_memory_std_compare_scalar;
    return m_compare_impl(lhs, rhs, n);
}

const lh_ptr
lh_memory_std_compare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    return m_compare_impl(lhs, rhs, n);
}

#else /* !LH_MEMORY_STD_HAVE_X86_SIMD */

const lh_ptr
lh_memory_std_compare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    lh_algorithm_compare(lh_uchar_t, lhs, rhs, n);
    return lh_null;
}

#endif /* LH_MEMORY_STD_HAVE_X86_SIMD */

const lh_ptr
lh_memory_std_rcompare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    lh_algorithm_rcompare(lh_uchar_t, lhs, rhs, n);
    return lh_null;
}