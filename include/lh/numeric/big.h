/**
 * @file big.h
 * @brief Fixed-width unsigned big number (::lh_numeric_big_t) and its arithmetic.
 *
 * ::LH_NUMERIC_BIG_WORDS 32-bit words, least significant first: 256 bits.
 * Wide enough for exact decimal conversion of single-precision floats (the
 * widest case there needs 192 bits; the rest leaves room for one more
 * multiplication by 10 without a check). Arithmetic wraps at 256 bits.
 */

#ifndef LH_NUMERIC_BIG_H
#define LH_NUMERIC_BIG_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/big/fields.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>
#include <lh/void.h>
#include <lh/size.h>

/** @brief Number of 32-bit words in ::lh_numeric_big_t. */
#define LH_NUMERIC_BIG_WORDS 8U

/**
 * @struct lh_numeric_big
 * @brief An unsigned 256-bit number. Fields via ::lh_numeric_big_fields.
 */
struct lh_numeric_big
{
    /** Members: ::lh_numeric_big_fields. */
    lh_numeric_big_fields(lh_u32_t, LH_NUMERIC_BIG_WORDS);
};
/** @copybrief lh_numeric_big */
typedef struct lh_numeric_big lh_numeric_big_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Word @p index, 0 being the least significant.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below ::LH_NUMERIC_BIG_WORDS.
 */
lh_u32_t
lh_numeric_big_get_word(const lh_numeric_big_t *self, lh_usize_t index);

/**
 * @brief Store @p value as word @p index, 0 being the least significant.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below ::LH_NUMERIC_BIG_WORDS.
 */
lh_void
lh_numeric_big_set_word(lh_numeric_big_t *self, lh_usize_t index, lh_u32_t value);

/** @brief @p self = @p value. */
lh_void
lh_numeric_big_set_u32(lh_numeric_big_t *self, lh_u32_t value);

/** @brief @p self *= @p k, wrapping at 256 bits. */
lh_void
lh_numeric_big_mul_u32(lh_numeric_big_t *self, lh_u32_t k);

/** @brief @p self <<= @p bits, wrapping at 256 bits. */
lh_void
lh_numeric_big_shl(lh_numeric_big_t *self, lh_usize_t bits);

/**
 * @brief Order of @p a and @p b.
 * @return Negative when @p a is smaller, 0 when equal, positive when larger.
 */
lh_sint_t
lh_numeric_big_compare(const lh_numeric_big_t *a, const lh_numeric_big_t *b);

/** @brief @p a -= @p b; the caller guarantees @p a >= @p b. */
lh_void
lh_numeric_big_sub(lh_numeric_big_t *a, const lh_numeric_big_t *b);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NUMERIC_BIG_H */
