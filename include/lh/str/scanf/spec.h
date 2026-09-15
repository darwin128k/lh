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
#include <lh/numeric/types.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/str/scanf/spec/kind.h>

/**
 * @struct lh_str_scanf_spec
 * @typedef lh_str_scanf_spec_t
 * @brief One literal run or conversion from a format mask.
 */
typedef struct lh_str_scanf_spec
{
    lh_str_scanf_spec_kind_t kind;
    lh_bool_t zero_pad;
    lh_bool_t left_justify;
    lh_bool_t uppercase;
    lh_uint_t width;
    lh_bool_t have_precision;
    lh_uint_t precision;
    lh_str_cptr literal;
    lh_usize_t literal_size;
} lh_str_scanf_spec_t;

#endif /* LH_STR_SCANF_SPEC_H */
