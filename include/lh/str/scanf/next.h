/**
 * @file next.h
 * @brief Walk a printf/scanf format mask, one token at a time.
 *
 * The piece shared by ::lh_str_ptr_format_text and ::lh_str_ptr_parse_text:
 * both interpret the same mask (`%%`, `%c`, `%s`, `%d`/`%i`, `%u`, `%x`/`%X`,
 * flags `0`/`-`, decimal width, `.` precision). Conversion of values is left
 * to the caller.
 *
 * Exported API (not a `util` kernel): the walker is a `.so` symbol. Digit
 * accumulate stays in `lh/char/digit.h`.
 */

#ifndef LH_STR_SCANF_NEXT_H
#define LH_STR_SCANF_NEXT_H

#include <lh/compiler/extern/c.h>
#include <lh/str/scanf/spec.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the literal run starting at `*pos` (up to a `%` or the NUL)
 *        into @p out and move `*pos` past it.
 *
 * @param fmt NUL-terminated format mask.
 * @param pos In/out cursor, at a character that is not `%`.
 * @param out Receives a ::lh_str_scanf_spec_kind_literal token.
 */
lh_void
lh_str_ptr_scanf_next_literal(const lh_str_ptr fmt, lh_usize_t *pos, lh_str_scanf_spec_t *out);

/**
 * @brief Read the `0` / `-` flags starting at `*pos` into @p out and move
 *        `*pos` past them.
 */
lh_void
lh_str_ptr_scanf_next_flags(const lh_str_ptr fmt, lh_usize_t *pos, lh_str_scanf_spec_t *out);

/**
 * @brief Read the decimal number starting at `*pos` into @p value (0 when
 *        there are no digits) and move `*pos` past it.
 *
 * @return ::lh_bool_false when the number does not fit ::lh_uint_t; `*pos`
 *         then stays at the digit that overflowed.
 */
lh_bool_t
lh_str_ptr_scanf_next_number(const lh_str_ptr fmt, lh_usize_t *pos, lh_uint_t *value);

/**
 * @brief Extract the next mask token starting at `*pos`.
 *
 * `*pos` is advanced past the token. Loop until this returns ::lh_bool_false
 * to walk every token in @p fmt.
 *
 * @param fmt  NUL-terminated format mask.
 * @param pos  In/out cursor. Start at `0`.
 * @param out  Receives the token. On ::lh_str_scanf_spec_kind_invalid the
 *             mask is malformed; on ::lh_str_scanf_spec_kind_literal,
 *             `literal` / `literal_size` view a slice of @p fmt.
 *
 * @return ::lh_bool_true if a token was produced (including invalid),
 *         ::lh_bool_false if `*pos` is already at the NUL (nothing left).
 */
lh_bool_t
lh_str_ptr_scanf_next(const lh_str_ptr fmt, lh_usize_t *pos, lh_str_scanf_spec_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_SCANF_NEXT_H */
