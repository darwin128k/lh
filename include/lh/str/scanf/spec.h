/**
 * @file spec.h
 * @brief One token of a printf/scanf format mask.
 *
 * Produced by ::lh_str_ptr_scanf_next. ::lh_str_ptr_format_text writes from
 * these tokens; ::lh_str_ptr_parse_text reads from them. The flags (`0`, `-`,
 * width, precision) are always parsed the printf way so both callers share
 * one walker; parse ignores padding flags it does not need.
 */

#ifndef LH_STR_SCANF_SPEC_H
#define LH_STR_SCANF_SPEC_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/str/scanf/spec/fields.h>
#include <lh/str/scanf/spec/kind.h>

/**
 * @struct lh_str_scanf_spec
 * @brief One literal run or conversion from a format mask. Fields via
 *        ::lh_str_scanf_spec_fields.
 */
struct lh_str_scanf_spec
{
    /** Members: ::lh_str_scanf_spec_fields. */
    lh_str_scanf_spec_fields(lh_str_scanf_spec_kind_t, lh_bool_t, lh_uint_t, const lh_str_ptr,
                             lh_usize_t);
};
/** @copybrief lh_str_scanf_spec */
typedef struct lh_str_scanf_spec lh_str_scanf_spec_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief A literal run or which conversion. */
lh_str_scanf_spec_kind_t
lh_str_scanf_spec_get_kind(const lh_str_scanf_spec_t *self);

/** @brief Store: a literal run or which conversion. */
lh_void
lh_str_scanf_spec_set_kind(lh_str_scanf_spec_t *self, lh_str_scanf_spec_kind_t kind);

/** @brief The `0` flag: pad with zeros. */
lh_bool_t
lh_str_scanf_spec_get_zero_pad(const lh_str_scanf_spec_t *self);

/** @brief Store: the `0` flag: pad with zeros. */
lh_void
lh_str_scanf_spec_set_zero_pad(lh_str_scanf_spec_t *self, lh_bool_t zero_pad);

/** @brief The `-` flag: pad on the right. */
lh_bool_t
lh_str_scanf_spec_get_left_justify(const lh_str_scanf_spec_t *self);

/** @brief Store: the `-` flag: pad on the right. */
lh_void
lh_str_scanf_spec_set_left_justify(lh_str_scanf_spec_t *self, lh_bool_t left_justify);

/** @brief Upper-case form of the conversion (`%X`). */
lh_bool_t
lh_str_scanf_spec_get_uppercase(const lh_str_scanf_spec_t *self);

/** @brief Store: upper-case form of the conversion (`%X`). */
lh_void
lh_str_scanf_spec_set_uppercase(lh_str_scanf_spec_t *self, lh_bool_t uppercase);

/** @brief Minimum field width; `0` when none was given. */
lh_uint_t
lh_str_scanf_spec_get_width(const lh_str_scanf_spec_t *self);

/** @brief Store: minimum field width; `0` when none was given. */
lh_void
lh_str_scanf_spec_set_width(lh_str_scanf_spec_t *self, lh_uint_t width);

/** @brief Whether a `.precision` was given. */
lh_bool_t
lh_str_scanf_spec_get_have_precision(const lh_str_scanf_spec_t *self);

/** @brief Store: whether a `.precision` was given. */
lh_void
lh_str_scanf_spec_set_have_precision(lh_str_scanf_spec_t *self, lh_bool_t have_precision);

/** @brief The precision, when have_precision. */
lh_uint_t
lh_str_scanf_spec_get_precision(const lh_str_scanf_spec_t *self);

/** @brief Store: the precision, when have_precision. */
lh_void
lh_str_scanf_spec_set_precision(lh_str_scanf_spec_t *self, lh_uint_t precision);

/** @brief Start of a literal run in the mask (not owned). */
const lh_str_ptr
lh_str_scanf_spec_get_literal(const lh_str_scanf_spec_t *self);

/** @brief Store: start of a literal run in the mask (not owned). */
lh_void
lh_str_scanf_spec_set_literal(lh_str_scanf_spec_t *self, const lh_str_ptr literal);

/** @brief Bytes in the literal run. */
lh_usize_t
lh_str_scanf_spec_get_literal_size(const lh_str_scanf_spec_t *self);

/** @brief Store: bytes in the literal run. */
lh_void
lh_str_scanf_spec_set_literal_size(lh_str_scanf_spec_t *self, lh_usize_t literal_size);

/**
 * @brief Reset @p self to an invalid conversion with no flags, an empty
 *        literal at @p fmt.
 */
lh_void
lh_str_scanf_spec_clear(lh_str_scanf_spec_t *self, const lh_str_ptr fmt);

/**
 * @brief How many characters the conversion may take out of @p remaining:
 *        the width when one was given and it is smaller.
 */
lh_usize_t
lh_str_scanf_spec_get_limit(const lh_str_scanf_spec_t *self, lh_usize_t remaining);

/**
 * @brief Padding a field of @p size characters needs to reach the width:
 *        width - @p size, or 0 when it is already as wide.
 */
lh_usize_t
lh_str_scanf_spec_get_pad(const lh_str_scanf_spec_t *self, lh_usize_t size);

/**
 * @brief Store the conversion character @p conversion as kind and case
 *        (`%X` is hex, upper case); anything else leaves the kind invalid.
 */
lh_void
lh_str_scanf_spec_set_conversion(lh_str_scanf_spec_t *self, lh_char_t conversion);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_SCANF_SPEC_H */
