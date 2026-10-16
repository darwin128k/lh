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

#include <lh/str/ptr.h>
#include <lh/str/scanf/spec/kind.h>

/**
 * @struct lh_str_scanf_spec
 * @brief One literal run or conversion from a format mask.
 */
struct lh_str_scanf_spec
{
    lh_str_scanf_spec_kind_t kind; /**< A literal run or which conversion. */
    lh_bool_t zero_pad;            /**< The `0` flag: pad with zeros. */
    lh_bool_t left_justify;        /**< The `-` flag: pad on the right. */
    lh_bool_t uppercase;           /**< Upper-case form of the conversion (`%X`). */
    lh_uint_t width;               /**< Minimum field width; `0` when none was given. */
    lh_bool_t have_precision;      /**< Whether a `.precision` was given. */
    lh_uint_t precision;           /**< The precision, when ::have_precision. */
    const lh_str_ptr literal;      /**< Start of a literal run in the mask (not owned). */
    lh_usize_t literal_size;       /**< Bytes in ::literal. */
};
/** @copybrief lh_str_scanf_spec */
typedef struct lh_str_scanf_spec lh_str_scanf_spec_t;

#endif /* LH_STR_SCANF_SPEC_H */
