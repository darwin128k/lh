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

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/str/scanf/spec.h>

LH_COMPILER_EXTERN_C_BEGIN

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
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_ptr_scanf_next(lh_str_cptr fmt, lh_usize_t *pos, lh_str_scanf_spec_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_SCANF_NEXT_H */
