#include <lh/core/crypto/ed25519/slide.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>

lh_s16_t
lh_crypto_ed25519_slide_get_next_index(const lh_crypto_ed25519_slide_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next_index;
}

lh_void
lh_crypto_ed25519_slide_set_next_index(lh_crypto_ed25519_slide_t *self, lh_s16_t next_index)
{
    lh_assert_runtime_ref(self);
    self->next_index = next_index;
}

lh_s8_t
lh_crypto_ed25519_slide_get_next_digit(const lh_crypto_ed25519_slide_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next_digit;
}

lh_void
lh_crypto_ed25519_slide_set_next_digit(lh_crypto_ed25519_slide_t *self, lh_s8_t next_digit)
{
    lh_assert_runtime_ref(self);
    self->next_digit = next_digit;
}

lh_u8_t
lh_crypto_ed25519_slide_get_next_check(const lh_crypto_ed25519_slide_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next_check;
}

lh_void
lh_crypto_ed25519_slide_set_next_check(lh_crypto_ed25519_slide_t *self, lh_u8_t next_check)
{
    lh_assert_runtime_ref(self);
    self->next_check = next_check;
}

lh_void
lh_crypto_ed25519_slide_init(lh_crypto_ed25519_slide_t *self,
                             const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE])
{
    lh_s32_t i = 252;
    while (i > 0 && lh_crypto_ed25519_scalar_get_bit(scalar, i) == 0)
    {
        i = lh_math_sub(i, 1);
    }
    lh_crypto_ed25519_slide_set_next_check(self, lh_cast_static(lh_u8_t, i + 1));
    lh_crypto_ed25519_slide_set_next_index(self, -1);
    lh_crypto_ed25519_slide_set_next_digit(self, -1);
}

lh_s32_t
lh_crypto_ed25519_slide_step(lh_crypto_ed25519_slide_t *self, lh_s32_t width, lh_s32_t i,
                             const lh_byte_t scalar[LH_CRYPTO_ED25519_SCALAR_SIZE])
{
    if (i == lh_crypto_ed25519_slide_get_next_check(self))
    {
        if (lh_crypto_ed25519_scalar_get_bit(scalar, i) ==
            lh_crypto_ed25519_scalar_get_bit(scalar, i - 1))
        {
            lh_crypto_ed25519_slide_set_next_check(
                self, lh_cast_static(lh_u8_t, lh_crypto_ed25519_slide_get_next_check(self) - 1U));
        }
        else
        {
            /* The digit of the next window. */
            const lh_s32_t w = lh_math_min(width, i + 1);
            lh_s32_t v = -(lh_crypto_ed25519_scalar_get_bit(scalar, i) << (w - 1));
            for (lh_s32_t j = 0; j < w - 1; j = lh_math_add_one(j))
            {
                v += lh_crypto_ed25519_scalar_get_bit(scalar, i - (w - 1) + j) << j;
            }
            v += lh_crypto_ed25519_scalar_get_bit(scalar, i - w);
            const lh_s32_t lsb = v & (~v + 1); /* the lowest set bit of v */
            const lh_s32_t s = (((lsb & 0xAA) != 0) << 0) | (((lsb & 0xCC) != 0) << 1) |
                               (((lsb & 0xF0) != 0) << 2); /* log2(lsb) */
            lh_crypto_ed25519_slide_set_next_index(self, lh_cast_static(lh_s16_t, i - (w - 1) + s));
            lh_crypto_ed25519_slide_set_next_digit(self, lh_cast_static(lh_s8_t, v >> s));
            lh_crypto_ed25519_slide_set_next_check(
                self, lh_cast_static(lh_u8_t, lh_crypto_ed25519_slide_get_next_check(self) -
                                                  lh_cast_static(lh_u8_t, w)));
        }
    }
    return i == lh_crypto_ed25519_slide_get_next_index(self)
               ? lh_crypto_ed25519_slide_get_next_digit(self)
               : 0;
}
