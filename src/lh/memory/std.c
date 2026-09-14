#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/cast/static.h>
#include <lh/compiler/type.h>
#include <lh/compiler/arch.h>
#include <lh/compiler/arch/family.h>
#include <lh/config.h>
#include <lh/cpu/simd.h>
#include <lh/numeric/fixed/types.h>
#include <lh/util/bit.h>
#include <lh/util/bit/bswap.h>
#include <lh/util/bit/scan.h>
#include <lh/util/ptr.h>

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

/* Shared by lh_memory_std_copy/copy_rev/rcopy's own dispatch (not the tiers
 * themselves): below this size, call the SSE2 tier directly (x86-64 guarantees
 * SSE2, so no runtime check is needed to do that); at or above it, go through the
 * indirect, AVX2-capable dispatch instead, since only then does that indirection
 * reliably pay for itself. A measured crossover, not a correctness fact — see
 * cmake/library_options.cmake for the full rationale and how to override it. */
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 && (LH_COMPILER_ARCH == LH_COMPILER_ARCH_64)
#    define LH_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD                                           \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD)
#endif

/* lh_memory_std_copy's plain while(n--) *d++ = *s++; loop (still used as-is under
 * every other compiler) measured ~9x slower under MSVC /O2 /Oi /Ot than under GCC on
 * the same data — checked directly with dumpbin /DISASM: MSVC emits it as a literal
 * byte-at-a-time movzx+mov loop, no vectorization, no recognized-memcpy substitution.
 * REP MOVSB is a baseline x86 string instruction, present and correct on every
 * x86/x86-64 CPU — no CPU feature detection needed (unlike the AVX2 path above) — and
 * under MSVC it wins at every size that was measured against MSVC's own scalar loop
 * (3x-15x, from 64 bytes up), so it remains the fallback when SIMD intrinsics are not
 * compilable. Gated on x86 specifically: REP MOVSB has no equivalent on other
 * architectures MSVC targets (e.g. ARM64), which fall through to the portable scalar
 * path below instead. When SSE2/AVX2 *are* compilable, lh_memory_std_copy prefers that
 * hand-written SIMD path under MSVC too (same Zen2 ERMSB lesson as the GCC side). */
#if (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC) && LH_COMPILER_ARCH_FAMILY_IS_X86
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 1
#    include <intrin.h>
#else
#    define LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB 0
#endif

/* Under GCC/Clang, the scalar copy loop is not the same story as MSVC's: GCC's own
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
#    define LH_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD                                                  \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD)
#else
#    define LH_MEMORY_STD_HAVE_GCC_REP_MOVSB 0
#endif

/* Scalar kernels owned by this file. SIMD tiers (and the public entry points
 * below their dispatch thresholds) force-inline these for heads/tails — they
 * are not a second public API. Callers elsewhere go through lh_memory_std_*.
 *
 * Block size for the branchless scalar compare / find scan: same CMake knob as
 * before (LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK). */
#define LH_MEMORY_STD_SCAN_BLOCK ((lh_usize_t)LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK)

#if LH_COMPILER_ARCH_FAMILY_IS_X86
/* Overlapping word/halfword/byte ladder for copies below one SSE register (16
 * bytes). Same technique CRT memcpy uses for the tiniest sizes: for any n in
 * [8, 15] two overlapping 8-byte moves cover the whole span with no loop.
 * Critical under MSVC, whose plain byte loop is a literal movzx+mov (Release
 * bench: 64B was ~44ns vs CRT ~5ns before SIMD took over at 512). On
 * x86/x86-64 unaligned integer loads/stores are architecturally defined.
 * Precondition: n < 16. */
LH_ATTRIBUTE_FORCE_INLINE
void
lh_memory_std_copy_tiny(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    if (n >= 8U)
    {
        const lh_u64_t first = *lh_ptr_rcast(const lh_u64_t, src);
        const lh_u64_t last = *lh_ptr_rcast(const lh_u64_t, src + n - 8U);
        *lh_ptr_rcast(lh_u64_t, dst) = first;
        *lh_ptr_rcast(lh_u64_t, dst + n - 8U) = last;
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = *lh_ptr_rcast(const lh_u32_t, src);
        const lh_u32_t last = *lh_ptr_rcast(const lh_u32_t, src + n - 4U);
        *lh_ptr_rcast(lh_u32_t, dst) = first;
        *lh_ptr_rcast(lh_u32_t, dst + n - 4U) = last;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first = *lh_ptr_rcast(const lh_u16_t, src);
        const lh_u16_t last = *lh_ptr_rcast(const lh_u16_t, src + n - 2U);
        *lh_ptr_rcast(lh_u16_t, dst) = first;
        *lh_ptr_rcast(lh_u16_t, dst + n - 2U) = last;
        return;
    }

    if (n != 0U)
    {
        *dst = *src;
    }
}
#    define LH_MEMORY_STD_HAVE_COPY_TINY 1
#else
#    define LH_MEMORY_STD_HAVE_COPY_TINY 0
#endif

LH_ATTRIBUTE_FORCE_INLINE
void
lh_memory_std_copy_bytes(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
#if LH_MEMORY_STD_HAVE_COPY_TINY
    if (n < 16U)
    {
        lh_memory_std_copy_tiny(dst, src, n);
        return;
    }

    do
    {
        *lh_ptr_rcast(lh_u64_t, dst) = *lh_ptr_rcast(const lh_u64_t, src);
        dst += 8;
        src += 8;
        n -= 8U;
    } while (n >= 8U);

    lh_memory_std_copy_tiny(dst, src, n);
#else
    while (n--)
    {
        *dst++ = *src++;
    }
#endif
}

#if LH_COMPILER_ARCH_FAMILY_IS_X86
/* dst[i] = src[n-1-i] for n < 16. Both ends loaded before any store so an
 * in-place reverse is defined. Precondition: n < 16. */
LH_ATTRIBUTE_FORCE_INLINE
void
lh_memory_std_copy_rev_tiny(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    if (n >= 8U)
    {
        const lh_u64_t first = lh_bit_bswap_u64(*lh_ptr_rcast(const lh_u64_t, src));
        const lh_u64_t last = lh_bit_bswap_u64(*lh_ptr_rcast(const lh_u64_t, src + n - 8U));
        *lh_ptr_rcast(lh_u64_t, dst + n - 8U) = first;
        *lh_ptr_rcast(lh_u64_t, dst) = last;
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = lh_bit_bswap_u32(*lh_ptr_rcast(const lh_u32_t, src));
        const lh_u32_t last = lh_bit_bswap_u32(*lh_ptr_rcast(const lh_u32_t, src + n - 4U));
        *lh_ptr_rcast(lh_u32_t, dst + n - 4U) = first;
        *lh_ptr_rcast(lh_u32_t, dst) = last;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first_rev = lh_bit_bswap_u16(*lh_ptr_rcast(const lh_u16_t, src));
        const lh_u16_t last_rev = lh_bit_bswap_u16(*lh_ptr_rcast(const lh_u16_t, src + n - 2U));
        *lh_ptr_rcast(lh_u16_t, dst + n - 2U) = first_rev;
        *lh_ptr_rcast(lh_u16_t, dst) = last_rev;
        return;
    }

    if (n != 0U)
    {
        *dst = *src;
    }
}
#endif

LH_ATTRIBUTE_FORCE_INLINE
void
lh_memory_std_copy_rev_bytes(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
#if LH_COMPILER_ARCH_FAMILY_IS_X86
    if (n >= 8U)
    {
        lh_uchar_t *d_end = dst + n;

        do
        {
            d_end -= 8;
            *lh_ptr_rcast(lh_u64_t, d_end) = lh_bit_bswap_u64(*lh_ptr_rcast(const lh_u64_t, src));
            src += 8;
            n -= 8U;
        } while (n >= 8U);

        lh_memory_std_copy_rev_tiny(dst, src, n);
        return;
    }

    lh_memory_std_copy_rev_tiny(dst, src, n);
#else
    dst += n;
    while (n--)
    {
        *--dst = *src++;
    }
#endif
}

#if LH_COMPILER_ARCH_FAMILY_IS_X86
/* Overlapping-word reverse copy for n < 16. Both ends are loaded before any
 * store, then the high end is written first — safe for memmove-backward overlap
 * (dst inside the source span). Precondition: n < 16. */
LH_ATTRIBUTE_FORCE_INLINE
void
lh_memory_std_rcopy_tiny(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    if (n >= 8U)
    {
        const lh_u64_t first = *lh_ptr_rcast(const lh_u64_t, src);
        const lh_u64_t last = *lh_ptr_rcast(const lh_u64_t, src + n - 8U);
        *lh_ptr_rcast(lh_u64_t, dst + n - 8U) = last;
        *lh_ptr_rcast(lh_u64_t, dst) = first;
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t first = *lh_ptr_rcast(const lh_u32_t, src);
        const lh_u32_t last = *lh_ptr_rcast(const lh_u32_t, src + n - 4U);
        *lh_ptr_rcast(lh_u32_t, dst + n - 4U) = last;
        *lh_ptr_rcast(lh_u32_t, dst) = first;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t first = *lh_ptr_rcast(const lh_u16_t, src);
        const lh_u16_t last = *lh_ptr_rcast(const lh_u16_t, src + n - 2U);
        *lh_ptr_rcast(lh_u16_t, dst + n - 2U) = last;
        *lh_ptr_rcast(lh_u16_t, dst) = first;
        return;
    }

    if (n != 0U)
    {
        *dst = *src;
    }
}
#endif

LH_ATTRIBUTE_FORCE_INLINE
void
lh_memory_std_rcopy_bytes(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
#if LH_COMPILER_ARCH_FAMILY_IS_X86
    if (n >= 8U)
    {
        lh_uchar_t *d = dst + n;
        const lh_uchar_t *s = src + n;

        do
        {
            d -= 8;
            s -= 8;
            *lh_ptr_rcast(lh_u64_t, d) = *lh_ptr_rcast(const lh_u64_t, s);
            n -= 8U;
        } while (n >= 8U);

        lh_memory_std_rcopy_tiny(dst, src, n);
        return;
    }

    lh_memory_std_rcopy_tiny(dst, src, n);
#else
    dst += n;
    src += n;
    while (n--)
    {
        *--dst = *--src;
    }
#endif
}

LH_ATTRIBUTE_FORCE_INLINE
void
lh_memory_std_set_bytes(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
#if LH_COMPILER_ARCH_FAMILY_IS_X86
    if (n >= 8U)
    {
        const lh_u64_t v = (lh_u64_t)val * 0x0101010101010101ULL;
        do
        {
            *lh_ptr_rcast(lh_u64_t, dst) = v;
            dst += 8;
            n -= 8U;
        } while (n >= 8U);

        if (n != 0U)
        {
            *lh_ptr_rcast(lh_u64_t, dst + n - 8U) = v;
        }
        return;
    }

    if (n >= 4U)
    {
        const lh_u32_t v = (lh_u32_t)val * 0x01010101U;
        *lh_ptr_rcast(lh_u32_t, dst) = v;
        *lh_ptr_rcast(lh_u32_t, dst + n - 4U) = v;
        return;
    }

    if (n >= 2U)
    {
        const lh_u16_t v = (lh_u16_t)((lh_u16_t)val * 0x0101U);
        *lh_ptr_rcast(lh_u16_t, dst) = v;
        *lh_ptr_rcast(lh_u16_t, dst + n - 2U) = v;
        return;
    }

    if (n != 0U)
    {
        *dst = val;
    }
#else
    while (n--)
    {
        *dst++ = val;
    }
#endif
}

static const lh_ptr
lh_memory_std_compare_bytes(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs);

    while (n >= LH_MEMORY_STD_SCAN_BLOCK)
    {
        lh_bool_t block_diff = lh_bool_false;
        lh_usize_t block_i;
        for (block_i = 0; block_i < LH_MEMORY_STD_SCAN_BLOCK; ++block_i)
        {
            block_diff = (lh_bool_t)(block_diff | (l[block_i] != r[block_i]));
        }
        if (block_diff)
        {
            break;
        }
        l += LH_MEMORY_STD_SCAN_BLOCK;
        r += LH_MEMORY_STD_SCAN_BLOCK;
        n -= LH_MEMORY_STD_SCAN_BLOCK;
    }

    while (n--)
    {
        if (*l != *r)
        {
            return l;
        }
        ++l;
        ++r;
    }
    return lh_null;
}

static const lh_ptr
lh_memory_std_rcompare_bytes(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs) + (n - 1U);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs) + (n - 1U);

    while (n--)
    {
        if (*l != *r)
        {
            return l;
        }
        --l;
        --r;
    }
    return lh_null;
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* Both are measured crossovers, not correctness facts — see
 * cmake/library_options.cmake for the full rationale and how to override them. */
#    define LH_MEMORY_STD_PREFETCH_TRIGGER                                                         \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_TRIGGER)
#    define LH_MEMORY_STD_PREFETCH_DISTANCE                                                        \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_DISTANCE)

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("sse2")
void
lh_memory_std_copy64_storeu(lh_uchar_t *dst, const lh_uchar_t *src)
{
    const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
    const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
    const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
    const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
    _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
    _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
    _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
    _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
}

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("sse2")
void
lh_memory_std_copy128_store_sse2(lh_uchar_t *dst, const lh_uchar_t *src)
{
    const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
    const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
    const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
    const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
    const __m128i v4 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 64));
    const __m128i v5 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 80));
    const __m128i v6 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 96));
    const __m128i v7 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 112));
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 64), v4);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 80), v5);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 96), v6);
    _mm_store_si128(lh_ptr_rcast(__m128i, dst + 112), v7);
}

LH_MEMORY_STD_SIMD_TARGET("sse2")
static void
lh_memory_std_copy_sse2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    /* CRT-style overlapping vector stores: two (or four) unaligned 16-byte moves
     * cover any length in [16, 63] with no loop, no alignment prologue, no scalar
     * tail. */
    if (n < 64U)
    {
        if (n >= 32U)
        {
            const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
            const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
            const __m128i c = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 32));
            const __m128i d = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), b);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 32), c);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), d);
            return;
        }

        if (n >= 16U)
        {
            const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
            const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), b);
            return;
        }

        lh_memory_std_copy_bytes(dst, src, n);
        return;
    }

    /* Unaligned 64-byte loop + one overlapping last block. Aligned 128-byte
     * movdqa still wins for bigger in-cache copies on SSE2-only targets, but
     * only after the setup cost is amortized — keep that path for n >= 512. */
    if (n < 512U)
    {
        lh_uchar_t *dst_end = dst + n;
        const lh_uchar_t *src_end = src + n;

        while (n >= 64U)
        {
            lh_memory_std_copy64_storeu(dst, src);
            dst += 64;
            src += 64;
            n -= 64U;
        }

        if (n != 0U)
        {
            lh_memory_std_copy64_storeu(dst_end - 64, src_end - 64);
        }
        return;
    }

    {
        lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)16);
        lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));
        lh_uchar_t *dst_end = dst + n;
        const lh_uchar_t *src_end = src + n;

        if (head > n)
        {
            head = n;
        }

        if (head != 0U)
        {
            lh_memory_std_copy_bytes(dst, src, head);
            dst += head;
            src += head;
            n -= head;
        }

        while (n >= 128U)
        {
            if (n >= LH_MEMORY_STD_PREFETCH_TRIGGER)
            {
                _mm_prefetch(lh_ptr_ccast(char, src + LH_MEMORY_STD_PREFETCH_DISTANCE),
                             _MM_HINT_T0);
            }

            lh_memory_std_copy128_store_sse2(dst, src);
            dst += 128;
            src += 128;
            n -= 128U;
        }

        if (n >= 64U)
        {
            const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
            const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
            const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
            const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
            _mm_store_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
            _mm_store_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
            _mm_store_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
            _mm_store_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
            dst += 64;
            src += 64;
            n -= 64U;
        }

        if (n != 0U)
        {
            lh_memory_std_copy64_storeu(dst_end - 64, src_end - 64);
        }
    }
}

/* Non-temporal SSE2 twin of lh_memory_std_copy_avx2_stream below — same RFO/cache-
 * pollution reason, same 2MB crossover (LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD), but
 * for CPUs that have SSE2 and not AVX2. Without this tier the multi-MB copies on an
 * SSE2-only machine stay on regular _mm_storeu_si128 and keep losing to the platform
 * CRT's memcpy (the gap the 4-wide unroll in lh_memory_std_copy_sse2 closed for mid
 * sizes, left open at multi-MB). MOVNTDQ (_mm_stream_si128) needs a 16-byte-aligned
 * destination; the short unaligned head is copied with regular stores first, and
 * _mm_sfence() fences the weakly-ordered NT stores before the scalar tail runs. */
LH_MEMORY_STD_SIMD_TARGET("sse2")
static void
lh_memory_std_copy_sse2_stream(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_uchar_t *dst_end = dst + n;
    const lh_uchar_t *src_end = src + n;
    lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)16);
    lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

    if (head > n)
    {
        head = n;
    }

    lh_memory_std_copy_bytes(dst, src, head);
    dst += head;
    src += head;
    n -= head;

    while (n >= 128U)
    {
        const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
        const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
        const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
        const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
        const __m128i v4 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 64));
        const __m128i v5 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 80));
        const __m128i v6 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 96));
        const __m128i v7 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 112));
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 64), v4);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 80), v5);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 96), v6);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 112), v7);
        dst += 128;
        src += 128;
        n -= 128U;
    }

    while (n >= 64U)
    {
        const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0));
        const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
        const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32));
        const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48));
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 0), v0);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 16), v1);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 32), v2);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 48), v3);
        dst += 64;
        src += 64;
        n -= 64U;
    }

    _mm_sfence();

    if (n != 0U)
    {
        lh_memory_std_copy64_storeu(dst_end - 64, src_end - 64);
    }
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* See lh_memory_std_compare_avx2 for why the tail falls back to scalar, not SSE2:
 * mixing legacy (non-VEX) SSE encoding into an AVX2-attributed function risks an
 * SSE/AVX transition penalty on older microarchitectures. */
LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("avx2")
void
lh_memory_std_copy128_storeu(lh_uchar_t *dst, const lh_uchar_t *src)
{
    const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
    const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
    const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
    const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
    _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
    _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
    _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
    _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
}

LH_ATTRIBUTE_FORCE_INLINE
LH_MEMORY_STD_SIMD_TARGET("avx2")
void
lh_memory_std_copy256_store(lh_uchar_t *dst, const lh_uchar_t *src)
{
    const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
    const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
    const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
    const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
    const __m256i v4 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 128));
    const __m256i v5 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 160));
    const __m256i v6 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 192));
    const __m256i v7 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 224));
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 128), v4);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 160), v5);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 192), v6);
    _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 224), v7);
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static void
lh_memory_std_copy_avx2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    /* 16-31: overlapping XMM. Compiled under target("avx2") so GCC/Clang emit
     * VEX-encoded vmovdqu, not a legacy-SSE mix. */
    if (n < 32U)
    {
        if (n >= 16U)
        {
            const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
            const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), b);
            return;
        }

        lh_memory_std_copy_bytes(dst, src, n);
        return;
    }

    if (n < 128U)
    {
        const __m256i a = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src));
        const __m256i b = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + n - 32));
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), a);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), b);
        return;
    }

    {
        lh_uchar_t *dst_end = dst + n;
        const lh_uchar_t *src_end = src + n;

        if (n >= 512U)
        {
            lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)32);
            lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

            if (head > n)
            {
                head = n;
            }

            if (head != 0U)
            {
                lh_memory_std_copy_bytes(dst, src, head);
                dst += head;
                src += head;
                n -= head;
            }

            while (n >= 256U)
            {
                if (n >= LH_MEMORY_STD_PREFETCH_TRIGGER)
                {
                    _mm_prefetch(lh_ptr_ccast(char, src + LH_MEMORY_STD_PREFETCH_DISTANCE),
                                 _MM_HINT_T0);
                }

                lh_memory_std_copy256_store(dst, src);
                dst += 256;
                src += 256;
                n -= 256U;
            }

            if (n >= 128U)
            {
                const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
                const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
                const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
                const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
                dst += 128;
                src += 128;
                n -= 128U;
            }
        }
        else
        {
            while (n >= 128U)
            {
                lh_memory_std_copy128_storeu(dst, src);
                dst += 128;
                src += 128;
                n -= 128U;
            }
        }

        if (n != 0U)
        {
            lh_memory_std_copy128_storeu(dst_end - 128, src_end - 128);
        }
    }
}

/* Non-temporal ("streaming") store tier for copies past LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD
 * — past the point where lh_memory_std_copy_avx2 above starts losing to the platform CRT's own
 * memcpy on this project's Zen2 benchmark target (regular stores measured ~1.7x-2.2x slower than
 * the CRT at 4-16MB, despite winning by a wide margin at every smaller size already covered by
 * the tier above). A regular store first pulls the destination cache line in before writing it
 * (read-for-ownership) even though every byte of it is about to be overwritten — wasted memory
 * bandwidth for a copy this large, which is also unlikely to re-read the destination soon enough
 * for that cache residency to pay for itself. MOVNTDQ (_mm256_stream_si256) writes around the
 * cache instead, at the cost of two obligations a regular store doesn't have: the destination
 * address must be 32-byte aligned (unlike _mm256_storeu_si256, this is undefined behaviour
 * otherwise, not just slow), handled by copying a short unaligned head with regular stores
 * first; and the stores are weakly ordered against everything after them until a fence, handled
 * by _mm_sfence() before the tail below runs. */
LH_MEMORY_STD_SIMD_TARGET("avx2")
static void
lh_memory_std_copy_avx2_stream(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_uchar_t *dst_end = dst + n;
    const lh_uchar_t *src_end = src + n;
    lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)32);
    lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

    if (head > n)
    {
        head = n;
    }

    lh_memory_std_copy_bytes(dst, src, head);
    dst += head;
    src += head;
    n -= head;

    while (n >= 256U)
    {
        const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
        const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
        const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
        const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
        const __m256i v4 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 128));
        const __m256i v5 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 160));
        const __m256i v6 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 192));
        const __m256i v7 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 224));
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 128), v4);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 160), v5);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 192), v6);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 224), v7);
        dst += 256;
        src += 256;
        n -= 256U;
    }

    while (n >= 128U)
    {
        const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
        const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
        const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
        const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
        dst += 128;
        src += 128;
        n -= 128U;
    }

    _mm_sfence();

    if (n != 0U)
    {
        lh_memory_std_copy128_storeu(dst_end - 128, src_end - 128);
    }
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

static void
lh_memory_std_copy_simd_scalar(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_memory_std_copy_bytes(dst, src, n);
}

typedef void (*lh_memory_std_copy_simd_fn)(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n);

static void
lh_memory_std_copy_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n);

/* Rewrites itself to the resolved implementation on the first real call, same
 * one-time-detection shape as m_compare_impl below. */
static lh_memory_std_copy_simd_fn m_copy_simd_impl = lh_memory_std_copy_simd_dispatch;

/* Resolved alongside m_copy_simd_impl above, on that same first real call — stays
 * lh_null (never called) whenever neither AVX2 nor SSE2 is both compiled in and usable
 * on this CPU (lh_memory_std_copy_avx2_stream / lh_memory_std_copy_sse2_stream are the
 * only implementations of this tier). */
static lh_memory_std_copy_simd_fn m_copy_stream_impl = lh_null;

/* 0 = not yet resolved. Shared by copy/set/rcopy/compare so the first std call
 * on this thread-of-control pays CPUID once, and later calls of any of them
 * can take a predicted direct jump instead of an indirect function pointer —
 * that pointer was the 64-255 tax when DIRECT_DISPATCH_THRESHOLD forced SSE2. */
#    define LH_MEMORY_STD_KIND_AVX2 1U
#    define LH_MEMORY_STD_KIND_SSE2 2U
#    define LH_MEMORY_STD_KIND_SCALAR 3U
#    define LH_MEMORY_STD_KIND_SSSE3 4U
static unsigned char m_simd_kind;
static unsigned char m_copy_rev_kind;

/* Below this, the overlapping-word tiny ladder (lh_memory_std_copy_tiny) handles the
 * copy — small enough that an indirect SIMD call cannot pay for itself, and on MSVC
 * the plain scalar byte loop is catastrophically bad (see the Release
 * bench: 64B was ~9x behind CRT before the ladder). At/above this size the SIMD
 * tier is used directly; 16 matches one SSE register so the first vector iteration
 * always does real work. */
#    define LH_MEMORY_STD_SIMD_COPY_THRESHOLD                                                      \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD)

/* Above this, the non-temporal stream tier (lh_memory_std_copy_avx2_stream, or
 * lh_memory_std_copy_sse2_stream when AVX2 isn't available) takes over from the plain
 * SIMD copy above — see those functions' own doc comments for why; the crossover was
 * measured on this project's own Zen2 benchmark target somewhere between 1MB (the
 * plain AVX2 tier still wins there) and 4MB (it loses clearly). */
#    define LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD                                               \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD)

static void
lh_memory_std_copy_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
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
    if (n >= LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD && m_copy_stream_impl != lh_null)
    {
        m_copy_stream_impl(dst, src, n);
    }
    else
    {
        m_copy_simd_impl(dst, src, n);
    }
}

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

lh_ptr
lh_memory_std_copy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    /* See lh_memory_std_copy_simd_dispatch's doc comment above for why this tier —
     * not REP MOVSB — is the default whenever SIMD is compilable, for both GCC/Clang
     * and MSVC. Under MSVC, __movsb (below) was originally used unconditionally because
     * it beat MSVC's own scalar byte loop by 3x-15x; once the hand-written SIMD path
     * existed, the same Zen2 measurement that retired REP MOVSB under GCC also applies
     * here (ERMSB is not equally fast on every x86 vendor), so MSVC takes this branch
     * too whenever the intrinsics compiled in. */
    if (n >= LH_MEMORY_STD_SIMD_COPY_THRESHOLD)
    {
        lh_uchar_t *d = lh_ptr_cast(lh_uchar_t, dst);
        const lh_uchar_t *s = lh_ptr_ccast(lh_uchar_t, src);

        if (n >= LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD)
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
            lh_memory_std_copy_sse2(d, s, n);
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
        lh_memory_std_copy_bytes(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
    }
#elif LH_MEMORY_STD_HAVE_MSVC_REP_MOVSB
    /* Fallback for an MSVC x86 build where SIMD intrinsics turned out not to be
     * compilable at all (see cmake/check_simd.cmake) — the SIMD branch above wins
     * whenever it is available. __movsb's declared signature is unsigned char* /
     * const unsigned char* (fixed by <intrin.h>); lh_uchar_t is a plain typedef of
     * unsigned char (lh/char.h), so this cast is the same reinterpretation either way. */
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
         * constraint list alone. Kept only as the fallback for a GCC-like x86 build
         * where SIMD intrinsics turned out not to be compilable at all (see
         * cmake/check_simd.cmake) — the branch above wins whenever it is available. */
        __asm__ volatile("rep movsb" : "+D"(d), "+S"(s), "+c"(n) : : "memory");
    }
    else
    {
        lh_memory_std_copy_bytes(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
    }
#else
    lh_memory_std_copy_bytes(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
#endif

    return end;
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* Reverse 16 bytes inside an SSE register using only SSE2 (no SSSE3 pshufb):
 * shuffle dwords end-to-end, swap 16-bit lanes inside each half, then swap the
 * two bytes of every u16. Verified against the scalar reverse-copy path. */
LH_MEMORY_STD_SIMD_TARGET("sse2") static __m128i lh_memory_std_reverse_epi8_sse2(__m128i v)
{
    v = _mm_shuffle_epi32(v, _MM_SHUFFLE(0, 1, 2, 3));
    v = _mm_shufflelo_epi16(v, _MM_SHUFFLE(2, 3, 0, 1));
    v = _mm_shufflehi_epi16(v, _MM_SHUFFLE(2, 3, 0, 1));
    return _mm_or_si128(_mm_slli_epi16(v, 8), _mm_srli_epi16(v, 8));
}

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

/* lh_memory_std_copy_rev: src walks low-to-high, dst is filled high-to-low so
 * dst[i] = src[n-1-i]. Load a forward block from src, byte-reverse it in-register,
 * store it at the descending destination cursor. */
LH_MEMORY_STD_SIMD_TARGET("sse2")
static void
lh_memory_std_copy_rev_sse2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_uchar_t *d_end = dst + n;

    if (n < 32U)
    {
        if (n >= 16U)
        {
            const __m128i a =
                lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src)));
            const __m128i b = lh_memory_std_reverse_epi8_sse2(
                _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16)));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), a);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), b);
            return;
        }

        lh_memory_std_copy_rev_bytes(dst, src, n);
        return;
    }

    while (n >= 64U)
    {
        const __m128i v0 =
            lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0)));
        const __m128i v1 =
            lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16)));
        const __m128i v2 =
            lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32)));
        const __m128i v3 =
            lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48)));
        d_end -= 64;
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 48), v0);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 32), v1);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 16), v2);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 0), v3);
        src += 64;
        n -= 64U;
    }

    lh_memory_std_copy_rev_bytes(dst, src, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSSE3

/* Reverse 16 bytes via a single pshufb — measured 1.6x-2.6x faster than the 4-
 * instruction SSE2-only shuffle sequence above across 64B-64KB on this project's
 * own GCC/MinGW toolchain (the mid-size band, ~1KB-4KB, saw the largest win: 2.6x
 * and 2.1x respectively). SSSE3 (2006+ on Intel, Bulldozer+ on AMD) is not part of
 * any baseline ISA, unlike SSE2 on x86-64, so this tier is still runtime-checked. */
LH_MEMORY_STD_SIMD_TARGET("ssse3") static __m128i lh_memory_std_reverse_epi8_ssse3(__m128i v)
{
    const __m128i mask = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    return _mm_shuffle_epi8(v, mask);
}

/* Same structure as lh_memory_std_copy_rev_sse2 — only the in-register reverse
 * differs. See that function's own comments for the block/chunk-order reasoning. */
LH_MEMORY_STD_SIMD_TARGET("ssse3")
static void
lh_memory_std_copy_rev_ssse3(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_uchar_t *d_end = dst + n;

    if (n < 32U)
    {
        if (n >= 16U)
        {
            const __m128i a =
                lh_memory_std_reverse_epi8_ssse3(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src)));
            const __m128i b = lh_memory_std_reverse_epi8_ssse3(
                _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16)));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), a);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), b);
            return;
        }

        lh_memory_std_copy_rev_bytes(dst, src, n);
        return;
    }

    while (n >= 64U)
    {
        const __m128i v0 =
            lh_memory_std_reverse_epi8_ssse3(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0)));
        const __m128i v1 = lh_memory_std_reverse_epi8_ssse3(
            _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16)));
        const __m128i v2 = lh_memory_std_reverse_epi8_ssse3(
            _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32)));
        const __m128i v3 = lh_memory_std_reverse_epi8_ssse3(
            _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48)));
        d_end -= 64;
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 48), v0);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 32), v1);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 16), v2);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 0), v3);
        src += 64;
        n -= 64U;
    }

    lh_memory_std_copy_rev_bytes(dst, src, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSSE3 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

LH_MEMORY_STD_SIMD_TARGET("avx2")
static void
lh_memory_std_copy_rev_avx2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    const __m256i lane_rev = _mm256_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                              15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);

    if (n < 64U)
    {
        if (n >= 32U)
        {
            __m256i a =
                _mm256_shuffle_epi8(_mm256_loadu_si256(lh_ptr_rcast(const __m256i, src)), lane_rev);
            __m256i b = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + n - 32)), lane_rev);
            a = _mm256_permute2x128_si256(a, a, 0x01);
            b = _mm256_permute2x128_si256(b, b, 0x01);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), a);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), b);
            return;
        }

        lh_memory_std_copy_rev_bytes(dst, src, n);
        return;
    }

    {
        lh_uchar_t *d_end = dst + n;

        while (n >= 256U)
        {
            __m256i v0 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0)), lane_rev);
            __m256i v1 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32)), lane_rev);
            __m256i v2 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64)), lane_rev);
            __m256i v3 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96)), lane_rev);
            __m256i v4 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 128)), lane_rev);
            __m256i v5 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 160)), lane_rev);
            __m256i v6 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 192)), lane_rev);
            __m256i v7 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 224)), lane_rev);
            v0 = _mm256_permute2x128_si256(v0, v0, 0x01);
            v1 = _mm256_permute2x128_si256(v1, v1, 0x01);
            v2 = _mm256_permute2x128_si256(v2, v2, 0x01);
            v3 = _mm256_permute2x128_si256(v3, v3, 0x01);
            v4 = _mm256_permute2x128_si256(v4, v4, 0x01);
            v5 = _mm256_permute2x128_si256(v5, v5, 0x01);
            v6 = _mm256_permute2x128_si256(v6, v6, 0x01);
            v7 = _mm256_permute2x128_si256(v7, v7, 0x01);
            d_end -= 256;
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 224), v0);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 192), v1);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 160), v2);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 128), v3);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 96), v4);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 64), v5);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 32), v6);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 0), v7);
            src += 256;
            n -= 256U;
        }

        while (n >= 128U)
        {
            __m256i v0 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0)), lane_rev);
            __m256i v1 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32)), lane_rev);
            __m256i v2 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64)), lane_rev);
            __m256i v3 = _mm256_shuffle_epi8(
                _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96)), lane_rev);
            v0 = _mm256_permute2x128_si256(v0, v0, 0x01);
            v1 = _mm256_permute2x128_si256(v1, v1, 0x01);
            v2 = _mm256_permute2x128_si256(v2, v2, 0x01);
            v3 = _mm256_permute2x128_si256(v3, v3, 0x01);
            d_end -= 128;
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 96), v0);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 64), v1);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 32), v2);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end + 0), v3);
            src += 128;
            n -= 128U;
        }

        lh_memory_std_copy_rev_bytes(dst, src, n);
    }
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

static void
lh_memory_std_copy_rev_simd_scalar(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_memory_std_copy_rev_bytes(dst, src, n);
}

typedef void (*lh_memory_std_copy_rev_simd_fn)(lh_uchar_t *dst, const lh_uchar_t *src,
                                               lh_usize_t n);

static void
lh_memory_std_copy_rev_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n);

static lh_memory_std_copy_rev_simd_fn m_copy_rev_simd_impl = lh_memory_std_copy_rev_simd_dispatch;

#    define LH_MEMORY_STD_SIMD_COPY_REV_THRESHOLD                                                  \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD)

static void
lh_memory_std_copy_rev_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
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

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

lh_ptr
lh_memory_std_copy_rev(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (n >= LH_MEMORY_STD_SIMD_COPY_REV_THRESHOLD)
    {
        lh_uchar_t *d = lh_ptr_cast(lh_uchar_t, dst);
        const lh_uchar_t *s = lh_ptr_ccast(lh_uchar_t, src);

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

/* lh_memory_std_rcopy's own SIMD tier: a plain reverse (decrementing
 * pointer) loop turned out not to get the same auto-vectorization treatment GCC gives
 * a forward copy loop (see lh_memory_std_copy_avx2's own doc comment
 * above) — measured up to ~14x slower than lh_memory_std_copy at the same size on
 * this project's Zen2 benchmark target. Same technique as that tier, mirrored for the
 * reverse direction (walks from the end of the range down to its start, same shape as
 * lh_memory_std_rcompare_avx2's block walk), falling back to lh_memory_std_rcopy_bytes
 * for the remaining head below its block width. */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

LH_MEMORY_STD_SIMD_TARGET("sse2")
static void
lh_memory_std_rcopy_sse2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    if (n < 64U)
    {
        if (n >= 32U)
        {
            const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
            const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16));
            const __m128i c = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 32));
            const __m128i d = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), d);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 32), c);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), b);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
            return;
        }

        if (n >= 16U)
        {
            const __m128i a = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src));
            const __m128i b = _mm_loadu_si128(lh_ptr_rcast(const __m128i, src + n - 16));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), b);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), a);
            return;
        }

        lh_memory_std_rcopy_bytes(dst, src, n);
        return;
    }

    {
        lh_uchar_t *d = dst + n;
        const lh_uchar_t *s = src + n;

        while (n >= 128U)
        {
            d -= 128;
            s -= 128;
            const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 0));
            const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 16));
            const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 32));
            const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 48));
            const __m128i v4 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 64));
            const __m128i v5 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 80));
            const __m128i v6 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 96));
            const __m128i v7 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 112));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 0), v0);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 16), v1);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 32), v2);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 48), v3);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 64), v4);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 80), v5);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 96), v6);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 112), v7);
            n -= 128U;
        }

        while (n >= 64U)
        {
            d -= 64;
            s -= 64;
            const __m128i v0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 0));
            const __m128i v1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 16));
            const __m128i v2 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 32));
            const __m128i v3 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, s + 48));
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 0), v0);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 16), v1);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 32), v2);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, d + 48), v3);
            n -= 64U;
        }

        lh_memory_std_rcopy_bytes(dst, src, n);
    }
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* See lh_memory_std_compare_avx2 for why the tail falls back to scalar, not SSE2. */
LH_MEMORY_STD_SIMD_TARGET("avx2")
static void
lh_memory_std_rcopy_avx2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    if (n < 64U)
    {
        if (n >= 32U)
        {
            const __m256i a = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src));
            const __m256i b = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + n - 32));
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), b);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), a);
            return;
        }

        lh_memory_std_rcopy_bytes(dst, src, n);
        return;
    }

    {
        lh_uchar_t *d = dst + n;
        const lh_uchar_t *s = src + n;

        while (n >= 256U)
        {
            d -= 256;
            s -= 256;
            const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 0));
            const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 32));
            const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 64));
            const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 96));
            const __m256i v4 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 128));
            const __m256i v5 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 160));
            const __m256i v6 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 192));
            const __m256i v7 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 224));
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 0), v0);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 32), v1);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 64), v2);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 96), v3);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 128), v4);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 160), v5);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 192), v6);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 224), v7);
            n -= 256U;
        }

        while (n >= 128U)
        {
            d -= 128;
            s -= 128;
            const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 0));
            const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 32));
            const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 64));
            const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s + 96));
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 0), v0);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 32), v1);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 64), v2);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, d + 96), v3);
            n -= 128U;
        }

        lh_memory_std_rcopy_bytes(dst, src, n);
    }
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

static void
lh_memory_std_rcopy_simd_scalar(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_memory_std_rcopy_bytes(dst, src, n);
}

typedef void (*lh_memory_std_rcopy_simd_fn)(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n);

static void
lh_memory_std_rcopy_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n);

static lh_memory_std_rcopy_simd_fn m_rcopy_simd_impl = lh_memory_std_rcopy_simd_dispatch;

static void
lh_memory_std_rcopy_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
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

#    define LH_MEMORY_STD_SIMD_RCOPY_THRESHOLD                                                     \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD)

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

lh_ptr
lh_memory_std_rcopy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (n >= LH_MEMORY_STD_SIMD_RCOPY_THRESHOLD)
    {
        lh_uchar_t *d = lh_ptr_cast(lh_uchar_t, dst);
        const lh_uchar_t *s = lh_ptr_ccast(lh_uchar_t, src);

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
    const lh_ptr src_end = lh_ptr_add_by_offset(lh_void, src, n);
    if (lh_ptr_is_backward_copy(dst, src, src_end))
    {
        lh_memory_std_rcopy(dst, src, n);
        return lh_ptr_add_by_offset(lh_void, dst, n);
    }
    return lh_memory_std_copy(dst, src, n);
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* lh_memory_std_set's own SIMD tier, same shape as lh_memory_std_copy's above — added
 * for the same reason: measured against the platform CRT's own memset on this
 * project's Zen2 benchmark target, the scalar fill (still used below
 * LH_MEMORY_STD_SIMD_SET_THRESHOLD) lost by ~1.5x-2x from 64 bytes up to a few KB.
 * A broadcast-and-store loop closes most of that gap without a libc dependency.
 * Falls back to lh_memory_std_set_bytes for the tail below its block width. */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

LH_MEMORY_STD_SIMD_TARGET("sse2")
static void
lh_memory_std_set_sse2(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
    const __m128i v = _mm_set1_epi8(lh_cast_static(char, val));

    if (n < 64U)
    {
        if (n >= 32U)
        {
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 32), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), v);
            return;
        }

        if (n >= 16U)
        {
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + n - 16), v);
            return;
        }

        lh_memory_std_set_bytes(dst, val, n);
        return;
    }

    {
        lh_uchar_t *dst_end = dst + n;

        if (n >= 512U)
        {
            lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)16);
            lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

            if (head > n)
            {
                head = n;
            }

            if (head != 0U)
            {
                lh_memory_std_set_bytes(dst, val, head);
                dst += head;
                n -= head;
            }

            while (n >= 64U)
            {
                _mm_store_si128(lh_ptr_rcast(__m128i, dst + 0), v);
                _mm_store_si128(lh_ptr_rcast(__m128i, dst + 16), v);
                _mm_store_si128(lh_ptr_rcast(__m128i, dst + 32), v);
                _mm_store_si128(lh_ptr_rcast(__m128i, dst + 48), v);
                dst += 64;
                n -= 64U;
            }
        }
        else
        {
            while (n >= 64U)
            {
                _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 0), v);
                _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 16), v);
                _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 32), v);
                _mm_storeu_si128(lh_ptr_rcast(__m128i, dst + 48), v);
                dst += 64;
                n -= 64U;
            }
        }

        if (n != 0U)
        {
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst_end - 64), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst_end - 48), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst_end - 32), v);
            _mm_storeu_si128(lh_ptr_rcast(__m128i, dst_end - 16), v);
        }
    }
}

/* Non-temporal SSE2 fill — twin of lh_memory_std_copy_sse2_stream. CRT memset uses
 * NT stores past a few MiB; without this tier large sets keep paying RFO on every
 * destination line (Release MSVC bench: 16MB set was ~2x behind CRT). */
LH_MEMORY_STD_SIMD_TARGET("sse2")
static void
lh_memory_std_set_sse2_stream(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
    const __m128i v = _mm_set1_epi8(lh_cast_static(char, val));

    {
        lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)16);
        lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

        if (head > n)
        {
            head = n;
        }

        if (head != 0U)
        {
            lh_memory_std_set_bytes(dst, val, head);
            dst += head;
            n -= head;
        }
    }

    while (n >= 64U)
    {
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 0), v);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 16), v);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 32), v);
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst + 48), v);
        dst += 64;
        n -= 64U;
    }

    while (n >= 16U)
    {
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst), v);
        dst += 16;
        n -= 16U;
    }

    _mm_sfence();

    lh_memory_std_set_bytes(dst, val, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* See lh_memory_std_compare_avx2 for why the tail falls back to scalar, not SSE2. */
LH_MEMORY_STD_SIMD_TARGET("avx2")
static void
lh_memory_std_set_avx2(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
    const __m256i v = _mm256_set1_epi8(lh_cast_static(char, val));

    if (n < 32U)
    {
        lh_memory_std_set_bytes(dst, val, n);
        return;
    }

    if (n < 128U)
    {
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), v);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + n - 32), v);
        return;
    }

    {
        lh_uchar_t *dst_end = dst + n;

        if (n >= 512U)
        {
            lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)32);
            lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

            if (head > n)
            {
                head = n;
            }

            if (head != 0U)
            {
                lh_memory_std_set_bytes(dst, val, head);
                dst += head;
                n -= head;
            }

            while (n >= 256U)
            {
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 0), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 32), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 64), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 96), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 128), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 160), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 192), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 224), v);
                dst += 256;
                n -= 256U;
            }

            while (n >= 128U)
            {
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 0), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 32), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 64), v);
                _mm256_store_si256(lh_ptr_rcast(__m256i, dst + 96), v);
                dst += 128;
                n -= 128U;
            }
        }
        else
        {
            while (n >= 128U)
            {
                _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 0), v);
                _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 32), v);
                _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 64), v);
                _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 96), v);
                dst += 128;
                n -= 128U;
            }
        }

        if (n != 0U)
        {
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst_end - 128), v);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst_end - 96), v);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst_end - 64), v);
            _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst_end - 32), v);
        }
    }
}

LH_MEMORY_STD_SIMD_TARGET("avx2")
static void
lh_memory_std_set_avx2_stream(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
    const __m256i v = _mm256_set1_epi8(lh_cast_static(char, val));

    {
        lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)32);
        lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

        if (head > n)
        {
            head = n;
        }

        if (head != 0U)
        {
            lh_memory_std_set_bytes(dst, val, head);
            dst += head;
            n -= head;
        }
    }

    while (n >= 128U)
    {
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 0), v);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 32), v);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 64), v);
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst + 96), v);
        dst += 128;
        n -= 128U;
    }

    while (n >= 32U)
    {
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst), v);
        dst += 32;
        n -= 32U;
    }

    _mm_sfence();

    lh_memory_std_set_bytes(dst, val, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

static void
lh_memory_std_set_simd_scalar(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
    lh_memory_std_set_bytes(dst, val, n);
}

typedef void (*lh_memory_std_set_simd_fn)(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n);

static void
lh_memory_std_set_simd_dispatch(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n);

static lh_memory_std_set_simd_fn m_set_simd_impl = lh_memory_std_set_simd_dispatch;
static lh_memory_std_set_simd_fn m_set_stream_impl = lh_null;

/* Same crossover as the copy stream tier — past this, RFO on every destination line
 * dominates, and NT stores win. */
#    define LH_MEMORY_STD_SIMD_SET_STREAM_THRESHOLD                                                \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD)

static void
lh_memory_std_set_simd_dispatch(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
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

    if (n >= LH_MEMORY_STD_SIMD_SET_STREAM_THRESHOLD && m_set_stream_impl != lh_null)
    {
        m_set_stream_impl(dst, val, n);
    }
    else
    {
        m_set_simd_impl(dst, val, n);
    }
}

/* Measured lower than lh_memory_std_copy's own threshold: a fill has only
 * one memory stream to drive (no read side), so the indirect call here pays for
 * itself sooner. */
#    define LH_MEMORY_STD_SIMD_SET_THRESHOLD                                                       \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_SET_THRESHOLD)

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

lh_ptr
lh_memory_std_set(lh_ptr dst, lh_uchar_t val, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (n >= LH_MEMORY_STD_SIMD_SET_THRESHOLD)
    {
        lh_uchar_t *d = lh_ptr_cast(lh_uchar_t, dst);

        if (n >= LH_MEMORY_STD_SIMD_SET_STREAM_THRESHOLD)
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

static const lh_ptr
lh_memory_std_compare_scalar(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    return lh_memory_std_compare_bytes(lhs, rhs, n);
}

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

/* 16 bytes/compare via a single packed-byte-equal + movemask, instead of the
 * scalar block's 16 elements/compare via ILP alone. GCC's own auto-vectorizer was tried for this
 * exact loop first (see the file-level comment above) and rejected: either it declines to vectorize
 * at all, or it produces a bloated multi-versioned loop that measured no faster than the scalar
 * path. This hand-written version is what actually measured a 7x+ win on this project's own
 * benchmark data. Falls back to the scalar tail below the block width, and to plain scalar via the
 * dispatch if not even SSE2 is available at runtime (32-bit x86 only — SSE2 is baseline on x86-64).
 */
LH_MEMORY_STD_SIMD_TARGET("sse2")
static const lh_ptr
lh_memory_std_compare_sse2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs);

    /* 2-wide unroll on the equal-path hot loop — same ILP idea as the copy tiers;
     * early-exit still fires on the first mismatched register. */
    while (n >= 32U)
    {
        const __m128i va0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, l + 0));
        const __m128i vb0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, r + 0));
        const __m128i va1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, l + 16));
        const __m128i vb1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, r + 16));
        const lh_u32_t eq0 = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va0, vb0)));
        const lh_u32_t eq1 = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va1, vb1)));

        if (eq0 != 0xFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_and(lh_bit_not(eq0), 0xFFFFU));
        }

        if (eq1 != 0xFFFFU)
        {
            return l + 16 + lh_bit_scan_forward_u32(lh_bit_and(lh_bit_not(eq1), 0xFFFFU));
        }

        l += 32;
        r += 32;
        n -= 32U;
    }

    while (n >= 16U)
    {
        const __m128i va = _mm_loadu_si128(lh_ptr_rcast(const __m128i, l));
        const __m128i vb = _mm_loadu_si128(lh_ptr_rcast(const __m128i, r));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
        }

        l += 16;
        r += 16;
        n -= 16U;
    }

    if (n != 0U)
    {
        const lh_uchar_t *const l0 = lh_ptr_ccast(lh_uchar_t, lhs);
        if ((lh_usize_t)((l + n) - l0) >= 16U)
        {
            const lh_uchar_t *lt = l + n - 16U;
            const lh_uchar_t *rt = r + n - 16U;
            const __m128i va = _mm_loadu_si128(lh_ptr_rcast(const __m128i, lt));
            const __m128i vb = _mm_loadu_si128(lh_ptr_rcast(const __m128i, rt));
            const lh_u32_t eq_mask =
                lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

            if (eq_mask != 0xFFFFU)
            {
                return lt + lh_bit_scan_forward_u32(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
            }

            return lh_null;
        }
    }

    return lh_memory_std_compare_scalar(l, r, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* Same technique as the SSE2 tier above, twice the width. Falls back to plain
 * scalar (not the SSE2 tier) for its <32-byte tail: mixing legacy (non-VEX) SSE
 * encoding into an AVX2-attributed function risks an SSE/AVX transition penalty
 * on older microarchitectures, so this stays on VEX-encoded/scalar code only. */
LH_MEMORY_STD_SIMD_TARGET("avx2")
static const lh_ptr
lh_memory_std_compare_avx2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs);

    while (n >= 128U)
    {
        const __m256i va0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 0));
        const __m256i vb0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 0));
        const __m256i va1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 32));
        const __m256i vb1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 32));
        const __m256i va2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 64));
        const __m256i vb2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 64));
        const __m256i va3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 96));
        const __m256i vb3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 96));
        const lh_u32_t eq0 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va0, vb0)));
        const lh_u32_t eq1 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va1, vb1)));
        const lh_u32_t eq2 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va2, vb2)));
        const lh_u32_t eq3 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va3, vb3)));

        if (eq0 != 0xFFFFFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_not(eq0));
        }

        if (eq1 != 0xFFFFFFFFU)
        {
            return l + 32 + lh_bit_scan_forward_u32(lh_bit_not(eq1));
        }

        if (eq2 != 0xFFFFFFFFU)
        {
            return l + 64 + lh_bit_scan_forward_u32(lh_bit_not(eq2));
        }

        if (eq3 != 0xFFFFFFFFU)
        {
            return l + 96 + lh_bit_scan_forward_u32(lh_bit_not(eq3));
        }

        l += 128;
        r += 128;
        n -= 128U;
    }

    while (n >= 64U)
    {
        const __m256i va0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 0));
        const __m256i vb0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 0));
        const __m256i va1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l + 32));
        const __m256i vb1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r + 32));
        const lh_u32_t eq0 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va0, vb0)));
        const lh_u32_t eq1 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va1, vb1)));

        if (eq0 != 0xFFFFFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_not(eq0));
        }

        if (eq1 != 0xFFFFFFFFU)
        {
            return l + 32 + lh_bit_scan_forward_u32(lh_bit_not(eq1));
        }

        l += 64;
        r += 64;
        n -= 64U;
    }

    while (n >= 32U)
    {
        const __m256i va = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, l));
        const __m256i vb = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, r));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFFFFFU)
        {
            return l + lh_bit_scan_forward_u32(lh_bit_not(eq_mask));
        }

        l += 32;
        r += 32;
        n -= 32U;
    }

    if (n != 0U)
    {
        const lh_uchar_t *const l0 = lh_ptr_ccast(lh_uchar_t, lhs);
        if ((lh_usize_t)((l + n) - l0) >= 32U)
        {
            const lh_uchar_t *lt = l + n - 32U;
            const lh_uchar_t *rt = r + n - 32U;
            const __m256i va = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lt));
            const __m256i vb = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rt));
            const lh_u32_t eq_mask =
                lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)));

            if (eq_mask != 0xFFFFFFFFU)
            {
                return lt + lh_bit_scan_forward_u32(lh_bit_not(eq_mask));
            }

            return lh_null;
        }
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

#else /* !(LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2) */

const lh_ptr
lh_memory_std_compare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    return lh_memory_std_compare_bytes(lhs, rhs, n);
}

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

static const lh_ptr
lh_memory_std_rcompare_scalar(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    return lh_memory_std_rcompare_bytes(lhs, rhs, n);
}

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

/* Mirrors lh_memory_std_compare_sse2, scanning from the end: the 16-byte block
 * ending at (and including) the current position is loaded starting 15 bytes
 * before it, so a mismatch is found with a *highest*-set-bit scan instead of a
 * lowest-set-bit one — the mismatch closest to the end of the block is the first
 * one this direction of scan should report. */
LH_MEMORY_STD_SIMD_TARGET("sse2")
static const lh_ptr
lh_memory_std_rcompare_sse2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs) + (n - 1U);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs) + (n - 1U);

    while (n >= 32U)
    {
        const lh_uchar_t *lb1 = l - 15;
        const lh_uchar_t *rb1 = r - 15;
        const lh_uchar_t *lb0 = l - 31;
        const lh_uchar_t *rb0 = r - 31;

        const __m128i va1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, lb1));
        const __m128i vb1 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, rb1));
        const __m128i va0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, lb0));
        const __m128i vb0 = _mm_loadu_si128(lh_ptr_rcast(const __m128i, rb0));
        const lh_u32_t eq1 = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va1, vb1)));
        const lh_u32_t eq0 = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va0, vb0)));

        /* Higher-address block first — that's the first mismatch a reverse scan must report. */
        if (eq1 != 0xFFFFU)
        {
            return lb1 + lh_bit_scan_reverse_u32(lh_bit_and(lh_bit_not(eq1), 0xFFFFU));
        }

        if (eq0 != 0xFFFFU)
        {
            return lb0 + lh_bit_scan_reverse_u32(lh_bit_and(lh_bit_not(eq0), 0xFFFFU));
        }

        l -= 32;
        r -= 32;
        n -= 32U;
    }

    while (n >= 16U)
    {
        const lh_uchar_t *lb = l - 15;
        const lh_uchar_t *rb = r - 15;

        const __m128i va = _mm_loadu_si128(lh_ptr_rcast(const __m128i, lb));
        const __m128i vb = _mm_loadu_si128(lh_ptr_rcast(const __m128i, rb));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFU)
        {
            return lb + lh_bit_scan_reverse_u32(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
        }

        l -= 16;
        r -= 16;
        n -= 16U;
    }

    /* l/r are the tail's *last*-byte pointer (this is a reverse scan), but
     * lh_memory_std_rcompare_bytes expects the tail's *base* pointer (it derives
     * its own last-byte pointer as base + (n-1) internally) — undo that offset once
     * here so the two don't compound. Matches that function's own tolerance
     * for an out-of-range-but-never-dereferenced base pointer when n is 0. */
    return lh_memory_std_rcompare_scalar(l - (n - 1U), r - (n - 1U), n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* Same technique as lh_memory_std_rcompare_sse2, twice the width; see
 * lh_memory_std_compare_avx2 for why the tail falls back to scalar, not SSE2. */
LH_MEMORY_STD_SIMD_TARGET("avx2")
static const lh_ptr
lh_memory_std_rcompare_avx2(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    const lh_usize_t orig = n;
    const lh_uchar_t *l = lh_ptr_ccast(lh_uchar_t, lhs) + (n - 1U);
    const lh_uchar_t *r = lh_ptr_ccast(lh_uchar_t, rhs) + (n - 1U);

    while (n >= 64U)
    {
        const lh_uchar_t *lb1 = l - 31;
        const lh_uchar_t *rb1 = r - 31;
        const lh_uchar_t *lb0 = l - 63;
        const lh_uchar_t *rb0 = r - 63;

        const __m256i va1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lb1));
        const __m256i vb1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rb1));
        const __m256i va0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lb0));
        const __m256i vb0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rb0));
        const lh_u32_t eq1 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va1, vb1)));
        const lh_u32_t eq0 =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va0, vb0)));

        if (eq1 != 0xFFFFFFFFU)
        {
            return lb1 + lh_bit_scan_reverse_u32(lh_bit_not(eq1));
        }

        if (eq0 != 0xFFFFFFFFU)
        {
            return lb0 + lh_bit_scan_reverse_u32(lh_bit_not(eq0));
        }

        l -= 64;
        r -= 64;
        n -= 64U;
    }

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
            return lb + lh_bit_scan_reverse_u32(lh_bit_not(eq_mask));
        }

        l -= 32;
        r -= 32;
        n -= 32U;
    }

    if (n != 0U && orig >= 32U)
    {
        const lh_uchar_t *lt = lh_ptr_ccast(lh_uchar_t, lhs);
        const lh_uchar_t *rt = lh_ptr_ccast(lh_uchar_t, rhs);
        const __m256i va = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, lt));
        const __m256i vb = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, rt));
        const lh_u32_t eq_mask =
            lh_cast_static(lh_u32_t, _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFFFFFU)
        {
            return lt + lh_bit_scan_reverse_u32(lh_bit_not(eq_mask));
        }

        return lh_null;
    }

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

#else /* !(LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2) */

const lh_ptr
lh_memory_std_rcompare(const lh_ptr lhs, const lh_ptr rhs, lh_usize_t n)
{
    lh_assert_runtime_ref(lhs);
    lh_assert_runtime_ref(rhs);

    return lh_memory_std_rcompare_bytes(lhs, rhs, n);
}

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */