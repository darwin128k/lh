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

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* lh_memory_std_copy's own SIMD tier, same technique/dispatch shape as
 * lh_memory_std_compare's below (plain 16/32-byte load+store, no movemask needed
 * since there is nothing to compare) — added after REP MOVSB (above) measured a 6x-27x
 * *loss* against the platform CRT's own memcpy on an AMD Zen2 target, across roughly
 * the same 500B-256KB range the file comment above says REP MOVSB was originally
 * measured to win on (an Intel target): REP MOVSB's ERMSB fast path is not equally
 * fast on every x86 vendor/generation, unlike a plain vector load/store loop, which
 * has no microcoded setup cost to lose on. This tier is what closes that gap without
 * taking a libc dependency (the project's other memory_std_* primitives are hand-
 * rolled precisely to avoid one). Falls back to lh_algorithm_copy for the tail below
 * its block width, same as lh_memory_std_compare's tiers. */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

/* Parameters are named dst/src, not the shorter d/s, deliberately: lh_algorithm_copy
 * (and friends) declare their own internal T *d/const T *s locals from whatever
 * identifiers are passed in — calling it as lh_algorithm_copy(lh_uchar_t, d, s, n)
 * from inside a function whose own parameters are already named d/s would expand to
 * `lh_uchar_t *d = lh_ptr_cast(lh_uchar_t, d);`, initializing the new d from itself
 * (already in scope at its own initializer, per C's declarator scope rules) instead
 * of from the caller's d — silently reading garbage, not a diagnosed error. */
LH_MEMORY_STD_SIMD_TARGET("sse2") static void
lh_memory_std_copy_sse2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    while (n >= 16U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), _mm_loadu_si128(lh_ptr_rcast(const __m128i, src)));
        dst += 16;
        src += 16;
        n -= 16U;
    }

    lh_algorithm_copy(lh_uchar_t, dst, src, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* See lh_memory_std_compare_avx2 for why the tail falls back to scalar, not SSE2:
 * mixing legacy (non-VEX) SSE encoding into an AVX2-attributed function risks an
 * SSE/AVX transition penalty on older microarchitectures. */
LH_MEMORY_STD_SIMD_TARGET("avx2") static void
lh_memory_std_copy_avx2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    while (n >= 128U)
    {
        const __m256i v0 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0));
        const __m256i v1 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32));
        const __m256i v2 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64));
        const __m256i v3 = _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96));
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 0), v0);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 32), v1);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 64), v2);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 96), v3);
        dst += 128;
        src += 128;
        n -= 128U;
    }

    while (n >= 32U)
    {
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src)));
        dst += 32;
        src += 32;
        n -= 32U;
    }

    lh_algorithm_copy(lh_uchar_t, dst, src, n);
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
LH_MEMORY_STD_SIMD_TARGET("avx2") static void
lh_memory_std_copy_avx2_stream(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    {
        lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)32);
        lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

        if (head > n)
        {
            head = n;
        }

        lh_algorithm_copy(lh_uchar_t, dst, src, head);
        dst += head;
        src += head;
        n -= head;
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

    while (n >= 32U)
    {
        _mm256_stream_si256(lh_ptr_rcast(__m256i, dst), _mm256_loadu_si256(lh_ptr_rcast(const __m256i, src)));
        dst += 32;
        src += 32;
        n -= 32U;
    }

    _mm_sfence();

    lh_algorithm_copy(lh_uchar_t, dst, src, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

static void
lh_memory_std_copy_simd_scalar(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_algorithm_copy(lh_uchar_t, dst, src, n);
}

typedef void (*lh_memory_std_copy_simd_fn)(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n);

static void
lh_memory_std_copy_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n);

/* Rewrites itself to the resolved implementation on the first real call, same
 * one-time-detection shape as m_compare_impl below. */
static lh_memory_std_copy_simd_fn m_copy_simd_impl = lh_memory_std_copy_simd_dispatch;

/* Resolved alongside m_copy_simd_impl above, on that same first real call — stays
 * lh_null (never called) whenever AVX2 isn't both compiled in and usable on this CPU,
 * since lh_memory_std_copy_avx2_stream is the only implementation of this tier. */
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
static lh_memory_std_copy_simd_fn m_copy_stream_impl = lh_null;
#    endif

static void
lh_memory_std_copy_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_copy_simd_impl = lh_memory_std_copy_avx2;
        m_copy_stream_impl = lh_memory_std_copy_avx2_stream;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_copy_simd_impl = lh_memory_std_copy_sse2;
    }
    else
#    endif
    {
        m_copy_simd_impl = lh_memory_std_copy_simd_scalar;
    }

    m_copy_simd_impl(dst, src, n);
}

/* Below this, lh_algorithm_copy's own inline scalar/auto-vectorized loop stays in use
 * (measured faster than going through this tier's indirect function-pointer call for
 * anything under ~512 bytes — the call overhead is not worth paying yet at that size,
 * same crossover point REP MOVSB used above before this tier replaced it). */
#    define LH_MEMORY_STD_SIMD_COPY_THRESHOLD ((lh_usize_t)512)

/* Above this, lh_memory_std_copy_avx2_stream's non-temporal tier takes over from
 * lh_memory_std_copy_avx2 above — see that function's own doc comment for why; the
 * crossover was measured on this project's own Zen2 benchmark target somewhere
 * between 1MB (the plain AVX2 tier still wins there) and 4MB (it loses clearly). */
#    define LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD ((lh_usize_t)2 * 1024 * 1024)

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

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
#elif LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    /* See lh_memory_std_copy_simd_dispatch's doc comment above for why this tier —
     * not REP MOVSB — is the default under GCC/Clang now. */
    if (n >= LH_MEMORY_STD_SIMD_COPY_THRESHOLD)
    {
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
        if (n >= LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD && m_copy_stream_impl != lh_null)
        {
            m_copy_stream_impl(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
        }
        else
#    endif
        m_copy_simd_impl(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
    }
    else
    {
        lh_algorithm_copy(lh_uchar_t, dst, src, n);
    }
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

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* lh_memory_std_rcopy's own SIMD tier: lh_algorithm_rcopy's plain reverse (decrementing
 * pointer) loop turned out not to get the same auto-vectorization treatment GCC gives
 * lh_algorithm_copy's forward loop (see lh_memory_std_copy_avx2's own doc comment
 * above) — measured up to ~14x slower than lh_memory_std_copy at the same size on
 * this project's Zen2 benchmark target. Same technique as that tier, mirrored for the
 * reverse direction (walks from the end of the range down to its start, same shape as
 * lh_memory_std_rcompare_avx2's block walk), falling back to lh_algorithm_rcopy for
 * the remaining head below its block width. */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

LH_MEMORY_STD_SIMD_TARGET("sse2") static void
lh_memory_std_rcopy_sse2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_uchar_t *d = dst + n;
    const lh_uchar_t *s = src + n;

    while (n >= 16U)
    {
        d -= 16;
        s -= 16;
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d), _mm_loadu_si128(lh_ptr_rcast(const __m128i, s)));
        n -= 16U;
    }

    lh_algorithm_rcopy(lh_uchar_t, dst, src, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* See lh_memory_std_compare_avx2 for why the tail falls back to scalar, not SSE2. */
LH_MEMORY_STD_SIMD_TARGET("avx2") static void
lh_memory_std_rcopy_avx2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_uchar_t *d = dst + n;
    const lh_uchar_t *s = src + n;

    while (n >= 32U)
    {
        d -= 32;
        s -= 32;
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, d), _mm256_loadu_si256(lh_ptr_rcast(const __m256i, s)));
        n -= 32U;
    }

    lh_algorithm_rcopy(lh_uchar_t, dst, src, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

static void
lh_memory_std_rcopy_simd_scalar(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_algorithm_rcopy(lh_uchar_t, dst, src, n);
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
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_rcopy_simd_impl = lh_memory_std_rcopy_sse2;
    }
    else
#    endif
    {
        m_rcopy_simd_impl = lh_memory_std_rcopy_simd_scalar;
    }

    m_rcopy_simd_impl(dst, src, n);
}

#    define LH_MEMORY_STD_SIMD_RCOPY_THRESHOLD ((lh_usize_t)32)

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

lh_ptr
lh_memory_std_rcopy(lh_ptr dst, const lh_ptr src, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);
    lh_assert_runtime_ref(src);

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (n >= LH_MEMORY_STD_SIMD_RCOPY_THRESHOLD)
    {
        m_rcopy_simd_impl(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
    }
    else
    {
        lh_algorithm_rcopy(lh_uchar_t, dst, src, n);
    }
#else
    lh_algorithm_rcopy(lh_uchar_t, dst, src, n);
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
 * project's Zen2 benchmark target, lh_algorithm_set's plain scalar/auto-vectorized
 * loop (still used below LH_MEMORY_STD_SIMD_SET_THRESHOLD) lost by ~1.5x-2x from 64
 * bytes up to a few KB. A broadcast-and-store loop closes most of that gap without a
 * libc dependency. Falls back to lh_algorithm_set for the tail below its block width. */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2

/* Parameter named dst, not the shorter d: see lh_memory_std_copy_sse2's doc comment
 * above for why — lh_algorithm_set declares its own internal T *d local the same way
 * lh_algorithm_copy does. */
LH_MEMORY_STD_SIMD_TARGET("sse2") static void
lh_memory_std_set_sse2(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
    const __m128i v = _mm_set1_epi8(lh_cast_static(char, val));

    while (n >= 16U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), v);
        dst += 16;
        n -= 16U;
    }

    lh_algorithm_set(lh_uchar_t, dst, val, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* See lh_memory_std_compare_avx2 for why the tail falls back to scalar, not SSE2. */
LH_MEMORY_STD_SIMD_TARGET("avx2") static void
lh_memory_std_set_avx2(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
    const __m256i v = _mm256_set1_epi8(lh_cast_static(char, val));

    while (n >= 128U)
    {
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 0), v);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 32), v);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 64), v);
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst + 96), v);
        dst += 128;
        n -= 128U;
    }

    while (n >= 32U)
    {
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), v);
        dst += 32;
        n -= 32U;
    }

    lh_algorithm_set(lh_uchar_t, dst, val, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

static void
lh_memory_std_set_simd_scalar(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
    lh_algorithm_set(lh_uchar_t, dst, val, n);
}

typedef void (*lh_memory_std_set_simd_fn)(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n);

static void
lh_memory_std_set_simd_dispatch(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n);

static lh_memory_std_set_simd_fn m_set_simd_impl = lh_memory_std_set_simd_dispatch;

static void
lh_memory_std_set_simd_dispatch(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_set_simd_impl = lh_memory_std_set_avx2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_set_simd_impl = lh_memory_std_set_sse2;
    }
    else
#    endif
    {
        m_set_simd_impl = lh_memory_std_set_simd_scalar;
    }

    m_set_simd_impl(dst, val, n);
}

/* Measured lower than lh_memory_std_copy's own threshold: lh_algorithm_set has only
 * one memory stream to drive (no read side), so the indirect call here pays for
 * itself sooner. */
#    define LH_MEMORY_STD_SIMD_SET_THRESHOLD ((lh_usize_t)32)

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

lh_ptr
lh_memory_std_set(lh_ptr dst, lh_uchar_t val, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (n >= LH_MEMORY_STD_SIMD_SET_THRESHOLD)
    {
        m_set_simd_impl(lh_ptr_cast(lh_uchar_t, dst), val, n);
    }
    else
    {
        lh_algorithm_set(lh_uchar_t, dst, val, n);
    }
#else
    lh_algorithm_set(lh_uchar_t, dst, val, n);
#endif
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