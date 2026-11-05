#include <lh/core/crypto/is_equal.h>
#include <lh/core/byte/ptr.h>
#include <lh/core/math.h>

lh_bool_t
lh_crypto_is_equal(const lh_ptr a, const lh_ptr b, lh_usize_t size)
{
    const lh_byte_t *x = lh_ptr_rcast(const lh_byte_t, a);
    const lh_byte_t *y = lh_ptr_rcast(const lh_byte_t, b);
    lh_u32_t diff = 0U;
    for (lh_usize_t i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        diff = lh_bit_or(diff, lh_bit_xor(x[i], y[i]));
    }
    /* diff is 0..255: diff - 1 borrows into bit 8 only when diff is 0. No branch
       on the contents. */
    return lh_cast_static(lh_bool_t, lh_bit_and(lh_bit_shr(lh_math_sub(diff, 1U), 8U), 1U));
}
