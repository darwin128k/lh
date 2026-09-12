#include <lh/memory/std.h>
#include <lh/util/algorithm.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/compiler/type.h>
#include <lh/compiler/arch.h>
#include <lh/compiler/arch/family.h>
#include <lh/config.h>
#include <lh/cpu/simd.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>
#include <lh/util/bit.h>

/* Real SIMD, runtime-dispatched, for both GCC/Clang and MSVC: whether a tier's
 * intrinsics + its runtime CPU-feature check are even compilable by this toolchain
 * for this target is decided once, at CMake configure time, by a compile-only probe
 * (cmake/check_simd.cmake — never executed, so it stays correct under cross-
 * compilation) and recorded as LH_LIBRARY_OPTION_SIMD_HAVE_{SSE2,AVX2} in config.h.
 * Which tier a given CPU can actually *run* is a separate, runtime-only question
 * (lh_cpu_simd_has_sse2/avx2, lh/cpu/simd.h), because the machine that configured the
 * build is not necessarily the machine that runs the binary.
 *
 * See lh_memory_std_compare below for why this is worth doing at all (a 7x+
 * throughput gap measured for the SSE2 tier alone against the portable scalar
 * block loop, on the same data, on this project's own GCC/MinGW toolchain — GCC's
 * auto-vectorizer was tried and rejected for this job first: at -O3 it either
 * declines to vectorize the scalar loop at all, or (once nudged into trying, via a
 * target attribute) produces a bloated multi-versioned loop that measured no
 * faster than the plain scalar path. Hand-written intrinsics, not autovectorization,
 * are what actually pays off here). Everywhere no tier is compilable (non-x86, or
 * an old toolchain without these intrinsics) falls back to the portable scalar
 * path, unchanged, with none of the dispatch machinery below even compiled in. */
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
#    include <immintrin.h>
#    if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#        include <intrin.h>
#    endif
#endif

/* GCC/Clang ask for a specific ISA's code generation per function via a target
 * attribute (no effect on any other function, no need for a global -msse2/-mavx2);
 * MSVC has no such mechanism at all — its intrinsics simply compile to the matching
 * instruction regardless of /arch, so the macro expands to nothing there. */
#if LH_COMPILER_TYPE_IS_GCC_LIKE
#    define LH_MEMORY_STD_SIMD_TARGET(isa) __attribute__((target(isa)))
#else
#    define LH_MEMORY_STD_SIMD_TARGET(isa)
#endif

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* Portable "index of lowest/highest set bit" for the movemask results below —
 * __builtin_ctz/clz (GCC/Clang) vs _BitScanForward/Reverse (MSVC), same operation. */
static lh_usize_t
lh_memory_std_bit_scan_forward(lh_u32_t x)
{
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
    return lh_cast_static(lh_usize_t, __builtin_ctz(x));
#    elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    lh_ulong_t index;
    _BitScanForward(&index, x);
    return lh_cast_static(lh_usize_t, index);
#    endif
}

static lh_usize_t
lh_memory_std_bit_scan_reverse(lh_u32_t x)
{
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
    return lh_cast_static(lh_usize_t, 31 - __builtin_clz(x));
#    elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    lh_ulong_t index;
    _BitScanReverse(&index, x);
    return lh_cast_static(lh_usize_t, index);
#    endif
}

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

/* lh_memory_std_copy's plain while(n--) *d++ = *s++; loop (still used as-is under
 * every other compiler) measured ~9x slower under MSVC /O2 /Oi /Ot than under GCC on
 * the same data — checked directly with dumpbin /DISASM: MSVC emits it as a literal
 * byte-at-a-time movzx+mov loop, no vectorization, no recognized-memcpy substitution.
 * REP MOVSB is a baseline x86 string instruction, present and correct on every
 * x86/x86-64 CPU — no CPU feature detection needed (unlike the AVX2 path above) — and
 * under MSVC it wins at every size that was measured (3x-15x over MSVC's own scalar
 * loop, from 64 bytes up), so it is used unconditionally there. Gated on x86
 * specifically: REP MOVSB has no equivalent on other architectures MSVC targets (e.g.
 * ARM64), which fall through to the portable scalar path below instead. */
#if (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC) && LH_COMPILER_ARCH_FAMILY_IS_X86
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 1
#    include <intrin.h>
#else
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 0
#endif

/* Under GCC/Clang, lh_algorithm_copy's plain loop is not the same story: GCC's own
 * auto-vectorizer already turns it into a 16-byte movdqu/movups loop (checked via
 * objdump — unlike lh_memory_std_compare's block loop, this one *does* get
 * vectorized), so REP MOVSB is competing against real SIMD here, not a scalar loop.
 * Measured on this project's own GCC/MinGW toolchain: REP MOVSB (via inline asm — GCC
 * has no __movsb-style intrinsic) loses to the compiler's own vectorized loop below
 * ~400-500 bytes (0.46x-0.82x: slower) and wins above it (1.24x-1.62x at 512-4096
 * bytes), matching the well-known "Enhanced REP MOVSB" (ERMSB) crossover behavior on
 * modern x86: fixed microcode setup cost that is not worth paying for small copies.
 * LH_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD picks a size at that measured crossover. */
#if LH_COMPILER_TYPE_IS_GCC_LIKE && LH_COMPILER_ARCH_FAMILY_IS_X86
#    define LH_MEMORY_STD_HAVE_GCC_REP_MOVSB 1
#    define LH_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD ((lh_usize_t)512)
#else
#    define LH_MEMORY_STD_HAVE_GCC_REP_MOVSB 0
#endif

lh_ptr
lh_memory_std_copy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);

#if LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB
    /* __movsb's own declared signature is unsigned char* / const unsigned char* (fixed by
     * <intrin.h>, not ours to change) — lh_uchar_t is a plain typedef of unsigned char
     * (lh/char.h), so this cast is the same reinterpretation either way, just spelled with
     * this file's own type alias instead of the raw C one, same as lh_algorithm_copy below. */
    __movsb(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
#elif LH_MEMORY_STD_HAVE_GCC_REP_MOVSB
    if (n >= LH_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD)
    {
        lh_uchar_t *d = lh_ptr_cast(lh_uchar_t, dst);
        const lh_uchar_t *s = lh_ptr_ccast(lh_uchar_t, src);

        /* No GCC/Clang builtin for REP MOVSB (unlike MSVC's __movsb) — inline asm is
         * the only way to ask for this instruction specifically. The three registers
         * it reads/advances (destination, source, count) are both inputs and outputs
         * (each is left pointing/counting past the copied range), and the compiler is
         * told the whole addressable memory may have changed ("memory" clobber),
         * since REP MOVSB writes through dst without that being visible from the
         * constraint list alone. */
        __asm__ volatile("rep movsb" : "+D"(d), "+S"(s), "+c"(n) : : "memory");
    }
    else
    {
        lh_algorithm_copy(lh_uchar_t, dst, src, n);
    }
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

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static const lh_ptr
lh_memory_std_compare_scalar(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_algorithm_compare(lh_uchar_t, lhs, rhs, n);
    return lh_null;
}

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

/* 16 bytes/compare via a single packed-byte-equal + movemask, instead of the
 * scalar block's 16 elements/compare via ILP alone — see lh_algorithm_compare's own
 * doc comment for that technique. GCC's own auto-vectorizer was tried for this exact
 * loop first (see the file-level comment above) and rejected: either it declines to
 * vectorize at all, or it produces a bloated multi-versioned loop that measured no
 * faster than the scalar path. This hand-written version is what actually measured
 * a 7x+ win on this project's own benchmark data. Falls back to the scalar tail
 * below the block width, and to plain scalar via the dispatch if not even SSE2 is
 * available at runtime (32-bit x86 only — SSE2 is baseline on x86-64). */
LH_MEMORY_STD_SIMD_TARGET("sse2") static const lh_ptr
lh_memory_std_compare_sse2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs);

    while (n >= 16U)
    {
        const __m128i va = _mm_loadu_si128(lh_ptr_rcast(const __m128i, l));
        const __m128i vb = _mm_loadu_si128(lh_ptr_rcast(const __m128i, r));
        const lh_u32_t eq_mask = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFU)
        {
            return l + lh_memory_std_bit_scan_forward(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
        }

        l += 16;
        r += 16;
        n -= 16U;
    }

    return lh_memory_std_compare_scalar(l, r, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* Same technique as the SSE2 tier above, twice the width. Falls back to plain
 * scalar (not the SSE2 tier) for its <32-byte tail: mixing legacy (non-VEX) SSE
 * encoding into an AVX2-attributed function risks an SSE/AVX transition penalty
 * on older microarchitectures, so this stays on VEX-encoded/scalar code only. */
LH_MEMORY_STD_SIMD_TARGET("avx2") static const lh_ptr
lh_memory_std_compare_avx2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs);

    while (n >= 32U)
    {
        const __m256i va = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l));
        const __m256i vb = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFFFFFU)
        {
            return l + lh_memory_std_bit_scan_forward(lh_bit_not(eq_mask));
        }

        l += 32;
        r += 32;
        n -= 32U;
    }

    return lh_memory_std_compare_scalar(l, r, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

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
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_compare_impl = lh_memory_std_compare_avx2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_compare_impl = lh_memory_std_compare_sse2;
    }
    else
#    endif
    {
        m_compare_impl = lh_memory_std_compare_scalar;
    }

    return m_compare_impl(lhs, rhs, n);
}

const lh_ptr
lh_memory_std_compare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    return m_compare_impl(lhs, rhs, n);
}

#else /* !(LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2) */

const lh_ptr
lh_memory_std_compare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    lh_algorithm_compare(lh_uchar_t, lhs, rhs, n);
    return lh_null;
}

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static const lh_ptr
lh_memory_std_rcompare_scalar(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_algorithm_rcompare(lh_uchar_t, lhs, rhs, n);
    return lh_null;
}

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

/* Mirrors lh_memory_std_compare_sse2, scanning from the end: the 16-byte block
 * ending at (and including) the current position is loaded starting 15 bytes
 * before it, so a mismatch is found with a *highest*-set-bit scan instead of a
 * lowest-set-bit one — the mismatch closest to the end of the block is the first
 * one this direction of scan should report. */
LH_MEMORY_STD_SIMD_TARGET("sse2") static const lh_ptr
lh_memory_std_rcompare_sse2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs) + (n - 1U);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs) + (n - 1U);

    while (n >= 16U)
    {
        const lh_uchar_t *lb = l - 15;
        const lh_uchar_t *rb = r - 15;

        const __m128i va = _mm_loadu_si128(lh_ptr_rcast(const __m128i, lb));
        const __m128i vb = _mm_loadu_si128(lh_ptr_rcast(const __m128i, rb));
        const lh_u32_t eq_mask = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFU)
        {
            return lb + lh_memory_std_bit_scan_reverse(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
        }

        l -= 16;
        r -= 16;
        n -= 16U;
    }

    /* l/r are the tail's *last*-byte pointer (this is a reverse scan), but
     * lh_memory_std_rcompare_scalar expects the tail's *base* pointer (it derives
     * its own last-byte pointer as base + (n-1) internally) — undo that offset once
     * here so the two don't compound. Matches lh_algorithm_rcompare's own tolerance
     * for an out-of-range-but-never-dereferenced base pointer when n is 0. */
    return lh_memory_std_rcompare_scalar(l - (n - 1U), r - (n - 1U), n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* Same technique as lh_memory_std_rcompare_sse2, twice the width; see
 * lh_memory_std_compare_avx2 for why the tail falls back to scalar, not SSE2. */
LH_MEMORY_STD_SIMD_TARGET("avx2") static const lh_ptr
lh_memory_std_rcompare_avx2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs) + (n - 1U);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs) + (n - 1U);

    while (n >= 32U)
    {
        const lh_uchar_t *lb = l - 31;
        const lh_uchar_t *rb = r - 31;

        const __m256i va = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lb));
        const __m256i vb = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rb));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFFFFFU)
        {
            return lb + lh_memory_std_bit_scan_reverse(lh_bit_not(eq_mask));
        }

        l -= 32;
        r -= 32;
        n -= 32U;
    }

    /* See the matching comment in lh_memory_std_rcompare_sse2 above. */
    return lh_memory_std_rcompare_scalar(l - (n - 1U), r - (n - 1U), n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

typedef const lh_ptr (*lh_memory_std_rcompare_fn)(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);

static const lh_ptr
lh_memory_std_rcompare_dispatch(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n);

static lh_memory_std_rcompare_fn m_rcompare_impl = lh_memory_std_rcompare_dispatch;

static const lh_ptr
lh_memory_std_rcompare_dispatch(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_rcompare_impl = lh_memory_std_rcompare_avx2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_rcompare_impl = lh_memory_std_rcompare_sse2;
    }
    else
#    endif
    {
        m_rcompare_impl = lh_memory_std_rcompare_scalar;
    }

    return m_rcompare_impl(lhs, rhs, n);
}

const lh_ptr
lh_memory_std_rcompare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    return m_rcompare_impl(lhs, rhs, n);
}

#else /* !(LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2) */

const lh_ptr
lh_memory_std_rcompare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    lh_algorithm_rcompare(lh_uchar_t, lhs, rhs, n);
    return lh_null;
}

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */