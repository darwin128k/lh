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
#include <lh/util/bit.h>
#include <lh/util/bit/scan.h>

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
#    define LH_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD                                             \
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
#    include <stdlib.h> /* _byteswap_uint64 for lh_memory_std_copy_rev's tiny path */
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
#    define LH_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD                                                    \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD)
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

/* Both are measured crossovers, not correctness facts — see
 * cmake/library_options.cmake for the full rationale and how to override them. */
#        define LH_MEMORY_STD_PREFETCH_TRIGGER ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_TRIGGER)
#        define LH_MEMORY_STD_PREFETCH_DISTANCE ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_PREFETCH_DISTANCE)

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
    /* Align the destination to 16 bytes so the bulk loop can use aligned stores
     * (movdqa / _mm_store_si128). Unaligned loads from src stay on loadu — src
     * alignment is independent. Aligned stores are what CRT memcpy leans on for
     * mid-size in-cache copies; the previous storeu-only loop left ~1.5x-2x on
     * the table against CRT at 512B-256KB on this project's SSE2-only target. */
    {
        lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)16);
        lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

        if (head > n)
        {
            head = n;
        }

        if (head != 0U)
        {
            lh_usize_t head_n = head;
            lh_algorithm_copy(lh_uchar_t, dst, src, head_n);
            dst += head;
            src += head;
            n -= head;
        }
    }

    /* Unrolled 4-wide, same shape/reason as lh_memory_std_copy_avx2's own 128-byte
     * loop below: a single load+store per iteration serializes on that one register's
     * load-to-store latency, leaving the CPU's other load/store ports idle. Measured
     * 1.7x-2.4x faster than the single-register loop across 512B-8KB on this
     * project's own GCC/MinGW toolchain. Prefetch the source a few lines ahead once
     * the remaining span is past a couple of cache lines — helps the mid-size band
     * where CRT was still winning after the 4-wide unroll alone. */
    while (n >= 64U)
    {
        if (n >= LH_MEMORY_STD_PREFETCH_TRIGGER)
        {
            _mm_prefetch(lh_ptr_ccast(char, src + LH_MEMORY_STD_PREFETCH_DISTANCE), _MM_HINT_T0);
        }

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

    while (n >= 16U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), _mm_loadu_si128(lh_ptr_rcast(const __m128i, src)));
        dst += 16;
        src += 16;
        n -= 16U;
    }

    lh_algorithm_copy(lh_uchar_t, dst, src, n);
}

/* Non-temporal SSE2 twin of lh_memory_std_copy_avx2_stream below — same RFO/cache-
 * pollution reason, same 2MB crossover (LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD), but
 * for CPUs that have SSE2 and not AVX2. Without this tier the multi-MB copies on an
 * SSE2-only machine stay on regular _mm_storeu_si128 and keep losing to the platform
 * CRT's memcpy (the gap the 4-wide unroll in lh_memory_std_copy_sse2 closed for mid
 * sizes, left open at multi-MB). MOVNTDQ (_mm_stream_si128) needs a 16-byte-aligned
 * destination; the short unaligned head is copied with regular stores first, and
 * _mm_sfence() fences the weakly-ordered NT stores before the scalar tail runs. */
LH_MEMORY_STD_SIMD_TARGET("sse2") static void
lh_memory_std_copy_sse2_stream(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    {
        lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)16);
        lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

        if (head > n)
        {
            head = n;
        }

        /* lh_algorithm_copy's while(n--) clobbers its count argument — copy into a
         * temporary so the post-adjust of dst/src/n below still sees the original head. */
        {
            lh_usize_t head_n = head;
            lh_algorithm_copy(lh_uchar_t, dst, src, head_n);
        }
        dst += head;
        src += head;
        n -= head;
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

    while (n >= 16U)
    {
        _mm_stream_si128(lh_ptr_rcast(__m128i, dst), _mm_loadu_si128(lh_ptr_rcast(const __m128i, src)));
        dst += 16;
        src += 16;
        n -= 16U;
    }

    _mm_sfence();

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

        /* lh_algorithm_copy's while(n--) clobbers its count argument — copy into a
         * temporary so the post-adjust of dst/src/n below still sees the original head. */
        {
            lh_usize_t head_n = head;
            lh_algorithm_copy(lh_uchar_t, dst, src, head_n);
        }
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
 * lh_null (never called) whenever neither AVX2 nor SSE2 is both compiled in and usable
 * on this CPU (lh_memory_std_copy_avx2_stream / lh_memory_std_copy_sse2_stream are the
 * only implementations of this tier). */
static lh_memory_std_copy_simd_fn m_copy_stream_impl = lh_null;

/* Below this, the overlapping-word tiny ladder (lh_memory_std_copy_tiny) handles the
 * copy — small enough that an indirect SIMD call cannot pay for itself, and on MSVC
 * the plain lh_algorithm_copy byte loop is catastrophically bad (see the Release
 * bench: 64B was ~9x behind CRT before the ladder). At/above this size the SIMD
 * tier is used directly; 16 matches one SSE register so the first vector iteration
 * always does real work. */
#    define LH_MEMORY_STD_SIMD_COPY_THRESHOLD ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD)

/* Above this, the non-temporal stream tier (lh_memory_std_copy_avx2_stream, or
 * lh_memory_std_copy_sse2_stream when AVX2 isn't available) takes over from the plain
 * SIMD copy above — see those functions' own doc comments for why; the crossover was
 * measured on this project's own Zen2 benchmark target somewhere between 1MB (the
 * plain AVX2 tier still wins there) and 4MB (it loses clearly). */
#    define LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD                                                  \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD)

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
        m_copy_stream_impl = lh_memory_std_copy_sse2_stream;
    }
    else
#    endif
    {
        m_copy_simd_impl = lh_memory_std_copy_simd_scalar;
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

/* Overlapping word/halfword/byte ladder for copies below one SSE register (16 bytes).
 * Same technique CRT memcpy uses for the tiniest sizes: for any n in [8, 15] two
 * overlapping 8-byte moves cover the whole span with no loop; likewise for 4 and 2.
 * Critical under MSVC, whose lh_algorithm_copy codegen is a literal byte-at-a-time
 * movzx+mov loop (Release bench: 64B was ~44ns vs CRT ~5ns before SIMD took over
 * at 512 — this ladder closes the hole below the new 16-byte SIMD threshold).
 * On x86/x86-64 unaligned integer loads/stores are architecturally defined. */
#if LH_COMPILER_ARCH_FAMILY_IS_X86
static void
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
        if (n >= LH_MEMORY_STD_SIMD_COPY_STREAM_THRESHOLD && m_copy_stream_impl != lh_null)
        {
            m_copy_stream_impl(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
        }
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 && (LH_COMPILER_ARCH == LH_COMPILER_ARCH_64)
        /* x86-64 guarantees SSE2 — call the SSE2 kernel directly below the size where
         * an AVX2 upgrade (via the indirect m_copy_simd_impl) is worth the FP. Avoids
         * paying dispatch/indirect-call overhead on the 16-255 band, where Release
         * MinGW otherwise lost to CRT's specialized small-copy path (and to GCC's own
         * earlier auto-vectorized inline loop before the threshold dropped to 16). */
        else if (n < LH_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD)
        {
            lh_memory_std_copy_sse2(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
        }
#    endif
        else
        {
            m_copy_simd_impl(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
        }
    }
#    if LH_MEMORY_STD_HAVE_COPY_TINY
    else
    {
        lh_memory_std_copy_tiny(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
    }
#    else
    else
    {
        lh_algorithm_copy(lh_uchar_t, dst, src, n);
    }
#    endif
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
#    if LH_MEMORY_STD_HAVE_COPY_TINY
    else
    {
        lh_memory_std_copy_tiny(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
    }
#    else
    else
    {
        lh_algorithm_copy(lh_uchar_t, dst, src, n);
    }
#    endif
#elif LH_MEMORY_STD_HAVE_COPY_TINY
    lh_memory_std_copy_tiny(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
#else
    lh_algorithm_copy(lh_uchar_t, dst, src, n);
#endif

    return end;
}

#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* Reverse 16 bytes inside an SSE register using only SSE2 (no SSSE3 pshufb):
 * shuffle dwords end-to-end, swap 16-bit lanes inside each half, then swap the
 * two bytes of every u16. Verified against the scalar lh_algorithm_copy_rev path. */
LH_MEMORY_STD_SIMD_TARGET("sse2") static __m128i
lh_memory_std_reverse_epi8_sse2(__m128i v)
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
LH_MEMORY_STD_SIMD_TARGET("sse2") static void
lh_memory_std_copy_rev_sse2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_uchar_t *d_end = dst + n;

    while (n >= 64U)
    {
        const __m128i v0 = lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0)));
        const __m128i v1 = lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16)));
        const __m128i v2 = lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32)));
        const __m128i v3 = lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48)));
        /* After reversing each 16-byte chunk, the chunk that was read first (src+0)
         * must land at the highest addresses of this 64-byte dest window. */
        d_end -= 64;
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 48), v0);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 32), v1);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 16), v2);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 0), v3);
        src += 64;
        n -= 64U;
    }

    while (n >= 16U)
    {
        d_end -= 16;
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end),
                         lh_memory_std_reverse_epi8_sse2(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src))));
        src += 16;
        n -= 16U;
    }

    {
        lh_usize_t rem = n;
        lh_algorithm_copy_rev(lh_uchar_t, dst, src, rem);
    }
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSSE3

/* Reverse 16 bytes via a single pshufb — measured 1.6x-2.6x faster than the 4-
 * instruction SSE2-only shuffle sequence above across 64B-64KB on this project's
 * own GCC/MinGW toolchain (the mid-size band, ~1KB-4KB, saw the largest win: 2.6x
 * and 2.1x respectively). SSSE3 (2006+ on Intel, Bulldozer+ on AMD) is not part of
 * any baseline ISA, unlike SSE2 on x86-64, so this tier is still runtime-checked. */
LH_MEMORY_STD_SIMD_TARGET("ssse3") static __m128i
lh_memory_std_reverse_epi8_ssse3(__m128i v)
{
    const __m128i mask = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    return _mm_shuffle_epi8(v, mask);
}

/* Same structure as lh_memory_std_copy_rev_sse2 — only the in-register reverse
 * differs. See that function's own comments for the block/chunk-order reasoning. */
LH_MEMORY_STD_SIMD_TARGET("ssse3") static void
lh_memory_std_copy_rev_ssse3(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_uchar_t *d_end = dst + n;

    while (n >= 64U)
    {
        const __m128i v0 = lh_memory_std_reverse_epi8_ssse3(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 0)));
        const __m128i v1 = lh_memory_std_reverse_epi8_ssse3(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 16)));
        const __m128i v2 = lh_memory_std_reverse_epi8_ssse3(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 32)));
        const __m128i v3 = lh_memory_std_reverse_epi8_ssse3(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src + 48)));
        d_end -= 64;
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 48), v0);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 32), v1);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 16), v2);
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end + 0), v3);
        src += 64;
        n -= 64U;
    }

    while (n >= 16U)
    {
        d_end -= 16;
        _mm_storeu_si128(lh_ptr_rcast(__m128i, d_end),
                         lh_memory_std_reverse_epi8_ssse3(_mm_loadu_si128(lh_ptr_rcast(const __m128i, src))));
        src += 16;
        n -= 16U;
    }

    {
        lh_usize_t rem = n;
        lh_algorithm_copy_rev(lh_uchar_t, dst, src, rem);
    }
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSSE3 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

LH_MEMORY_STD_SIMD_TARGET("avx2") static void
lh_memory_std_copy_rev_avx2(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    /* AVX2 vpshufb only reverses within each 128-bit lane — swap the lanes
     * afterward (permute2x128) to get a full 32-byte reverse. */
    const __m256i lane_rev =
        _mm256_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11,
                         10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    lh_uchar_t *d_end = dst + n;

    while (n >= 128U)
    {
        __m256i v0 = _mm256_shuffle_epi8(_mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 0)), lane_rev);
        __m256i v1 = _mm256_shuffle_epi8(_mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 32)), lane_rev);
        __m256i v2 = _mm256_shuffle_epi8(_mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 64)), lane_rev);
        __m256i v3 = _mm256_shuffle_epi8(_mm256_loadu_si256(lh_ptr_rcast(const __m256i, src + 96)), lane_rev);
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

    while (n >= 32U)
    {
        __m256i v = _mm256_shuffle_epi8(_mm256_loadu_si256(lh_ptr_rcast(const __m256i, src)), lane_rev);
        v = _mm256_permute2x128_si256(v, v, 0x01);
        d_end -= 32;
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, d_end), v);
        src += 32;
        n -= 32U;
    }

    {
        lh_usize_t rem = n;
        lh_algorithm_copy_rev(lh_uchar_t, dst, src, rem);
    }
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

static void
lh_memory_std_copy_rev_simd_scalar(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
    lh_algorithm_copy_rev(lh_uchar_t, dst, src, n);
}

typedef void (*lh_memory_std_copy_rev_simd_fn)(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n);

static void
lh_memory_std_copy_rev_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n);

static lh_memory_std_copy_rev_simd_fn m_copy_rev_simd_impl = lh_memory_std_copy_rev_simd_dispatch;

#    define LH_MEMORY_STD_SIMD_COPY_REV_THRESHOLD                                                     \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD)

static void
lh_memory_std_copy_rev_simd_dispatch(lh_uchar_t *dst, const lh_uchar_t *src, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_copy_rev_simd_impl = lh_memory_std_copy_rev_avx2;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSSE3
        if (lh_cpu_simd_has_ssse3())
    {
        m_copy_rev_simd_impl = lh_memory_std_copy_rev_ssse3;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_copy_rev_simd_impl = lh_memory_std_copy_rev_sse2;
    }
    else
#    endif
    {
        m_copy_rev_simd_impl = lh_memory_std_copy_rev_simd_scalar;
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
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 && (LH_COMPILER_ARCH == LH_COMPILER_ARCH_64)
        /* Stays on plain SSE2 rather than checking lh_cpu_simd_has_ssse3() here too:
         * SSE2 is unconditional on x86-64 (no branch needed to call it directly),
         * which is the whole point of this fast path over the indirect dispatch
         * below. The measured SSSE3 win is real at this size too, but the biggest
         * gains (2.1x-2.6x) were at 1KB-4KB, already past this threshold and served
         * by m_copy_rev_simd_impl, which does resolve to the SSSE3 tier when usable. */
        if (n < LH_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD)
        {
            lh_memory_std_copy_rev_sse2(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
        }
        else
#    endif
        {
            m_copy_rev_simd_impl(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
        }
    }
    else
#endif
#if LH_COMPILER_ARCH_FAMILY_IS_X86
    if (n >= 8U)
    {
        /* Tiny reverse via bswap — same idea as lh_memory_std_copy_tiny.
         * Parameters named dst_b/src_b: lh_algorithm_copy_rev declares its own
         * T *d / const T *s locals from whatever identifiers are passed in (see
         * lh_memory_std_copy_sse2's doc comment for the same trap). */
        lh_uchar_t *dst_b = lh_ptr_cast(lh_uchar_t, dst);
        const lh_uchar_t *src_b = lh_ptr_ccast(lh_uchar_t, src);
        lh_uchar_t *d_end = dst_b + n;

        while (n >= 8U)
        {
            d_end -= 8;
#    if LH_COMPILER_TYPE_IS_GCC_LIKE
            const lh_u64_t v = lh_cast_static(lh_u64_t, __builtin_bswap64(*lh_ptr_rcast(const lh_u64_t, src_b)));
#    elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
            const lh_u64_t v = lh_cast_static(lh_u64_t, _byteswap_uint64(*lh_ptr_rcast(const lh_u64_t, src_b)));
#    else
            const lh_u64_t raw = *lh_ptr_rcast(const lh_u64_t, src_b);
            const lh_u64_t v =
                ((raw & 0x00000000000000FFULL) << 56) | ((raw & 0x000000000000FF00ULL) << 40) |
                ((raw & 0x0000000000FF0000ULL) << 24) | ((raw & 0x00000000FF000000ULL) << 8) |
                ((raw & 0x000000FF00000000ULL) >> 8) | ((raw & 0x0000FF0000000000ULL) >> 24) |
                ((raw & 0x00FF000000000000ULL) >> 40) | ((raw & 0xFF00000000000000ULL) >> 56);
#    endif
            *lh_ptr_rcast(lh_u64_t, d_end) = v;
            src_b += 8;
            n -= 8U;
        }

        {
            lh_usize_t rem = n;
            lh_algorithm_copy_rev(lh_uchar_t, dst_b, src_b, rem);
        }
    }
    else
#endif
    {
        lh_algorithm_copy_rev(lh_uchar_t, dst, src, n);
    }

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

    /* Unrolled 4-wide — see lh_memory_std_copy_sse2's own doc comment for why. Each
     * block's own four loads/stores stay in forward (low-to-high) order within
     * themselves; only the block-to-block walk runs high-to-low, same as the
     * single-register version this replaces. */
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

#    define LH_MEMORY_STD_SIMD_RCOPY_THRESHOLD                                                        \
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
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 && (LH_COMPILER_ARCH == LH_COMPILER_ARCH_64)
        if (n < LH_MEMORY_STD_SIMD_DIRECT_DISPATCH_THRESHOLD)
        {
            lh_memory_std_rcopy_sse2(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
        }
        else
#    endif
        {
            m_rcopy_simd_impl(lh_ptr_cast(lh_uchar_t, dst), lh_ptr_ccast(lh_uchar_t, src), n);
        }
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

    /* Align dst for movdqa stores — same reason as lh_memory_std_copy_sse2. */
    {
        lh_uchar_t *aligned_dst = lh_ptr_align_up(lh_uchar_t, dst, (lh_uaddr_t)16);
        lh_usize_t head = lh_cast_static(lh_usize_t, lh_ptr_udiff(aligned_dst, dst));

        if (head > n)
        {
            head = n;
        }

        if (head != 0U)
        {
            lh_usize_t head_n = head;
            lh_algorithm_set(lh_uchar_t, dst, val, head_n);
            dst += head;
            n -= head;
        }
    }

    /* Unrolled 4-wide — see lh_memory_std_copy_sse2's own doc comment for why (this
     * tier has only one memory stream to drive, not two, but is still four
     * independent stores per iteration instead of one, for the same reason). */
    while (n >= 64U)
    {
        _mm_store_si128(lh_ptr_rcast(__m128i, dst + 0), v);
        _mm_store_si128(lh_ptr_rcast(__m128i, dst + 16), v);
        _mm_store_si128(lh_ptr_rcast(__m128i, dst + 32), v);
        _mm_store_si128(lh_ptr_rcast(__m128i, dst + 48), v);
        dst += 64;
        n -= 64U;
    }

    while (n >= 16U)
    {
        _mm_storeu_si128(lh_ptr_rcast(__m128i, dst), v);
        dst += 16;
        n -= 16U;
    }

    lh_algorithm_set(lh_uchar_t, dst, val, n);
}

/* Non-temporal SSE2 fill — twin of lh_memory_std_copy_sse2_stream. CRT memset uses
 * NT stores past a few MiB; without this tier large sets keep paying RFO on every
 * destination line (Release MSVC bench: 16MB set was ~2x behind CRT). */
LH_MEMORY_STD_SIMD_TARGET("sse2") static void
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
            lh_usize_t head_n = head;
            lh_algorithm_set(lh_uchar_t, dst, val, head_n);
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

    lh_algorithm_set(lh_uchar_t, dst, val, n);
}

#    endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 */

#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2

/* See lh_memory_std_compare_avx2 for why the tail falls back to scalar, not SSE2. */
LH_MEMORY_STD_SIMD_TARGET("avx2") static void
lh_memory_std_set_avx2(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
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
            lh_usize_t head_n = head;
            lh_algorithm_set(lh_uchar_t, dst, val, head_n);
            dst += head;
            n -= head;
        }
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

    while (n >= 32U)
    {
        _mm256_storeu_si256(lh_ptr_rcast(__m256i, dst), v);
        dst += 32;
        n -= 32U;
    }

    lh_algorithm_set(lh_uchar_t, dst, val, n);
}

LH_MEMORY_STD_SIMD_TARGET("avx2") static void
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
            lh_usize_t head_n = head;
            lh_algorithm_set(lh_uchar_t, dst, val, head_n);
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
static lh_memory_std_set_simd_fn m_set_stream_impl = lh_null;

/* Same crossover as the copy stream tier — past this, RFO on every destination line
 * dominates, and NT stores win. */
#    define LH_MEMORY_STD_SIMD_SET_STREAM_THRESHOLD                                                   \
        ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_STREAM_THRESHOLD)

static void
lh_memory_std_set_simd_dispatch(lh_uchar_t *dst, lh_uchar_t val, lh_usize_t n)
{
#    if LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (lh_cpu_simd_has_avx2())
    {
        m_set_simd_impl = lh_memory_std_set_avx2;
        m_set_stream_impl = lh_memory_std_set_avx2_stream;
    }
    else
#    endif
#    if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2
        if (lh_cpu_simd_has_sse2())
    {
        m_set_simd_impl = lh_memory_std_set_sse2;
        m_set_stream_impl = lh_memory_std_set_sse2_stream;
    }
    else
#    endif
    {
        m_set_simd_impl = lh_memory_std_set_simd_scalar;
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

/* Measured lower than lh_memory_std_copy's own threshold: lh_algorithm_set has only
 * one memory stream to drive (no read side), so the indirect call here pays for
 * itself sooner. */
#    define LH_MEMORY_STD_SIMD_SET_THRESHOLD ((lh_usize_t)LH_LIBRARY_OPTION_MEMORY_STD_SIMD_SET_THRESHOLD)

#endif /* LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2 */

lh_ptr
lh_memory_std_set(lh_ptr dst, lh_uchar_t val, lh_usize_t n)
{
    lh_assert_runtime_ref(dst);

    lh_ptr end = lh_ptr_add_unsafe(lh_void, dst, n);
#if LH_LIBRARY_OPTION_SIMD_HAVE_SSE2 || LH_LIBRARY_OPTION_SIMD_HAVE_AVX2
    if (n >= LH_MEMORY_STD_SIMD_SET_THRESHOLD)
    {
        if (n >= LH_MEMORY_STD_SIMD_SET_STREAM_THRESHOLD && m_set_stream_impl != lh_null)
        {
            m_set_stream_impl(lh_ptr_cast(lh_uchar_t, dst), val, n);
        }
        else
        {
            m_set_simd_impl(lh_ptr_cast(lh_uchar_t, dst), val, n);
        }
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
            return l + lh_bit_scan_forward(lh_bit_and(lh_bit_not(eq0), 0xFFFFU));
        }

        if (eq1 != 0xFFFFU)
        {
            return l + 16 + lh_bit_scan_forward(lh_bit_and(lh_bit_not(eq1), 0xFFFFU));
        }

        l += 32;
        r += 32;
        n -= 32U;
    }

    while (n >= 16U)
    {
        const __m128i va = _mm_loadu_si128(lh_ptr_rcast(const __m128i, l));
        const __m128i vb = _mm_loadu_si128(lh_ptr_rcast(const __m128i, r));
        const lh_u32_t eq_mask = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFU)
        {
            return l + lh_bit_scan_forward(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
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
            return l + lh_bit_scan_forward(lh_bit_not(eq_mask));
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
            return lb1 + lh_bit_scan_reverse(lh_bit_and(lh_bit_not(eq1), 0xFFFFU));
        }

        if (eq0 != 0xFFFFU)
        {
            return lb0 + lh_bit_scan_reverse(lh_bit_and(lh_bit_not(eq0), 0xFFFFU));
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
        const lh_u32_t eq_mask = lh_cast_static(lh_u32_t, _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb)));

        if (eq_mask != 0xFFFFU)
        {
            return lb + lh_bit_scan_reverse(lh_bit_and(lh_bit_not(eq_mask), 0xFFFFU));
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
            return lb + lh_bit_scan_reverse(lh_bit_not(eq_mask));
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