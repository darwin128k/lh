/**
 * @file fields.h
 * @brief Member fields of ::lh_str_scanf_spec_t.
 */

#ifndef LH_STR_SCANF_SPEC_FIELDS_H
#define LH_STR_SCANF_SPEC_FIELDS_H

/**
 * @def lh_str_scanf_spec_fields(kind_type, bool_type, uint_type, text_type, size_type)
 * @brief One literal run or conversion from a format mask.
 *
 * - `kind`: a literal run or which conversion.
 * - `zero_pad`: the `0` flag, pad with zeros.
 * - `left_justify`: the `-` flag, pad on the right.
 * - `uppercase`: upper-case form of the conversion (`%X`).
 * - `width`: minimum field width; `0` when none was given.
 * - `have_precision`: whether a `.precision` was given.
 * - `precision`: the precision, when `have_precision`.
 * - `literal`: start of a literal run in the mask (not owned).
 * - `literal_size`: bytes in `literal`.
 */
#define lh_str_scanf_spec_fields(kind_type, bool_type, uint_type, text_type, size_type)            \
    kind_type kind;                                                                                \
    bool_type zero_pad;                                                                            \
    bool_type left_justify;                                                                        \
    bool_type uppercase;                                                                           \
    uint_type width;                                                                               \
    bool_type have_precision;                                                                      \
    uint_type precision;                                                                           \
    text_type literal;                                                                             \
    size_type literal_size

#endif /* LH_STR_SCANF_SPEC_FIELDS_H */
