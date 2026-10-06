/**
 * @file bytes.h
 * @brief Unpack an integer into 8-bit bytes: ::lh_numeric_parse_bytes.
 *
 * Most-significant byte of the used width first. For example @p count 4 on
 * `0xRRGGBBAA` writes `R, G, B, A`. This is number → bytes, not text →
 * number; string hex lives in <lh/str/parse/hex.h>.
 */

#ifndef LH_NUMERIC_PARSE_BYTES_H
#define LH_NUMERIC_PARSE_BYTES_H

#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write @p count bytes of @p value into @p out, high byte first.
 *
 * @p count must be in `1..sizeof(::lh_uint_t)`. Only the low
 * `8 * count` bits of @p value are used.
 */
lh_void
lh_numeric_parse_bytes(lh_uint_t value, lh_byte_t *out, lh_usize_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NUMERIC_PARSE_BYTES_H */
