#include <lh/util/bit/scan.h>

#include <lh/cast/static.h>
#include <lh/compiler/type.h>

#if LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
#    include <intrin.h>
#    include <lh/numeric/types.h> /* lh_ulong_t, matching _BitScan{Forward,Reverse}'s own index type */
#endif

lh_usize_t
lh_bit_scan_forward(lh_u32_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return lh_cast_static(lh_usize_t, __builtin_ctz(x));
#elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    lh_ulong_t index;
    _BitScanForward(&index, x);
    return lh_cast_static(lh_usize_t, index);
#else
    /* Portable fallback for any other standards-conforming compiler: x is nonzero
     * per this function's own precondition, so this always terminates. */
    lh_usize_t i = 0U;
    while (((x >> i) & 1U) == 0U)
    {
        ++i;
    }
    return i;
#endif
}

lh_usize_t
lh_bit_scan_reverse(lh_u32_t x)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE
    return lh_cast_static(lh_usize_t, 31 - __builtin_clz(x));
#elif LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC
    lh_ulong_t index;
    _BitScanReverse(&index, x);
    return lh_cast_static(lh_usize_t, index);
#else
    lh_usize_t i = 31U;
    while (((x >> i) & 1U) == 0U)
    {
        --i;
    }
    return i;
#endif
}
