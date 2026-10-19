/**
 * @file lit.h
 * @brief A string literal's length, taken at compile time.
 */

#ifndef LH_STR_LIT_H
#define LH_STR_LIT_H

#include <lh/cast/static.h>
#include <lh/size.h>

/**
 * @def lh_str_lit_size(s)
 * @brief Characters of string literal @p s, without its NUL.
 */
#define lh_str_lit_size(s) lh_cast_static(lh_usize_t, (sizeof(s) - 1u))

/**
 * @def lh_str_lit(s)
 * @brief String literal @p s and its length, as the two arguments a
 *        pointer-and-size function takes.
 */
#define lh_str_lit(s) (s), lh_str_lit_size(s)

#endif /* LH_STR_LIT_H */
