/**
 * @file map.h
 * @brief Lower and upper case mapping of ::lh_char_t buffers in place.
 */

#ifndef LH_UTIL_STR_CASE_MAP_H
#define LH_UTIL_STR_CASE_MAP_H

#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Replace each of the first @p size ::lh_char_t elements of @p str
 *        using Unicode simple lowercase mapping for single-byte scalars (0..255),
 *        same subset as ::lh_wstr_ptr_to_lower for Latin-1 style buffers.
 *
 * @note Interprets each code unit as an unsigned byte; not UTF-8 scalar mapping.
 *
 * @param str  Buffer to transform (::lh_runtime_check_ref).
 * @param size Number of ::lh_char_t elements to map.
 *
 * @return Pointer one past the last element updated (@p str + @p size).
 */
lh_str_ptr
lh_str_ptr_to_lower(lh_str_ptr str, lh_usize_t size);

/**
 * @brief Replace each of the first @p size ::lh_char_t elements of @p str
 *        using Unicode simple uppercase mapping for single-byte scalars (0..255).
 *
 * @see lh_str_ptr_to_lower
 */
lh_str_ptr
lh_str_ptr_to_upper(lh_str_ptr str, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_STR_CASE_MAP_H */
