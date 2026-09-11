# -----------------------------------------------------------------------------
# lh_check_simd()
#
# Compile-only capability probes (CheckCSourceCompiles — never CheckCSourceRuns):
# each check asks "can THIS toolchain, targeting THIS target, compile code that
# uses tier X's intrinsics and its runtime CPU-feature-detection mechanism?".
# It never asks "does the machine running CMake have tier X" — that question is
# meaningless for cross-compilation and is answered at *runtime*, in the library
# itself, via __builtin_cpu_supports (GCC/Clang) or CPUID + XGETBV (MSVC).
#
# Sets (in the caller's scope):
#   LH_SIMD_HAVE_SSE2 — TRUE if the toolchain can compile the SSE2 tier
#                        (intrinsics + its "is SSE2 usable here" runtime check).
#   LH_SIMD_HAVE_AVX2 — same, for the AVX2 tier.
#
# Both are plain (non-cache) variables — this is a toolchain fact, not a user
# choice, same treatment as LH_IPO_SUPPORTED in the top-level CMakeLists.txt.
# -----------------------------------------------------------------------------
include(CheckCSourceCompiles)

function(lh_check_simd)
    if (MSVC)
        # MSVC: no per-function ISA attribute — intrinsics compile regardless of
        # /arch; runtime gating uses __cpuid/__cpuidex + _xgetbv from <intrin.h>.
        check_c_source_compiles("
            #include <intrin.h>
            #include <immintrin.h>
            static int f(const char *a, const char *b) {
                __m128i va = _mm_loadu_si128((const __m128i *)a);
                __m128i vb = _mm_loadu_si128((const __m128i *)b);
                return _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb));
            }
            static int cpu_has_sse2(void) {
                int info[4];
                __cpuid(info, 1);
                return (info[3] >> 26) & 1;
            }
            int main(void) { char a[16] = {0}, b[16] = {0}; return f(a, b) + cpu_has_sse2(); }
        " LH_SIMD_HAVE_SSE2)

        check_c_source_compiles("
            #include <intrin.h>
            #include <immintrin.h>
            static int f(const char *a, const char *b) {
                __m256i va = _mm256_loadu_si256((const __m256i *)a);
                __m256i vb = _mm256_loadu_si256((const __m256i *)b);
                return _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb));
            }
            static int cpu_has_avx2(void) {
                int info[4];
                __cpuid(info, 0);
                if (info[0] < 7) return 0;
                __cpuid(info, 1);
                if (!((info[2] >> 27) & 1) || !((info[2] >> 28) & 1)) return 0;
                if ((_xgetbv(0) & 0x6) != 0x6) return 0;
                __cpuidex(info, 7, 0);
                return (info[1] >> 5) & 1;
            }
            int main(void) { char a[32] = {0}, b[32] = {0}; return f(a, b) + cpu_has_avx2(); }
        " LH_SIMD_HAVE_AVX2)
    else ()
        # GCC/Clang: per-function __attribute__((target(...))) selects the ISA for
        # that function only; runtime gating uses __builtin_cpu_supports.
        check_c_source_compiles("
            #include <immintrin.h>
            __attribute__((target(\"sse2\")))
            static int f(const char *a, const char *b) {
                __m128i va = _mm_loadu_si128((const __m128i *)a);
                __m128i vb = _mm_loadu_si128((const __m128i *)b);
                return _mm_movemask_epi8(_mm_cmpeq_epi8(va, vb));
            }
            int main(void) {
                char a[16] = {0}, b[16] = {0};
                return f(a, b) + __builtin_cpu_supports(\"sse2\");
            }
        " LH_SIMD_HAVE_SSE2)

        check_c_source_compiles("
            #include <immintrin.h>
            __attribute__((target(\"avx2\")))
            static int f(const char *a, const char *b) {
                __m256i va = _mm256_loadu_si256((const __m256i *)a);
                __m256i vb = _mm256_loadu_si256((const __m256i *)b);
                return _mm256_movemask_epi8(_mm256_cmpeq_epi8(va, vb));
            }
            int main(void) {
                char a[32] = {0}, b[32] = {0};
                return f(a, b) + __builtin_cpu_supports(\"avx2\");
            }
        " LH_SIMD_HAVE_AVX2)
    endif ()

    set(LH_SIMD_HAVE_SSE2 "${LH_SIMD_HAVE_SSE2}" PARENT_SCOPE)
    set(LH_SIMD_HAVE_AVX2 "${LH_SIMD_HAVE_AVX2}" PARENT_SCOPE)
endfunction()
