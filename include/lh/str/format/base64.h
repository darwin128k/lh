/**
 * @file base64.h
 * @brief Format a byte span as Base64 text (RFC 4648, standard alphabet, `=` padding).
 */

#ifndef LH_STR_FORMAT_BASE64_H
#define LH_STR_FORMAT_BASE64_H

#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Characters the Base64 text of @p data_size bytes takes: four for every
 *        three bytes or part of three.
 */
lh_usize_t
lh_str_ptr_format_base64_size(lh_usize_t data_size);

/**
 * @brief Write the four Base64 characters of @p count (`1`..`3`) bytes at @p data
 *        to @p str, `=` standing in for the bytes past @p count.
 */
lh_void
lh_str_ptr_format_base64_block(const lh_byte_ptr data, lh_usize_t count, lh_str_ptr str);

/**
 * @brief Write @p data_size bytes of @p data as Base64 into @p str.
 *
 * No NUL is written. Nothing is written when it does not fit.
 *
 * @param data      Bytes to encode; may be ::lh_null when @p data_size is zero.
 * @param data_size Number of bytes.
 * @param str       Destination.
 * @param str_size  Capacity of @p str in characters.
 *
 * @return Characters written (::lh_str_ptr_format_base64_size), or `0` when they
 *         would not fit.
 */
lh_usize_t
lh_str_ptr_format_base64(const lh_ptr data, lh_usize_t data_size, lh_str_ptr str,
                         lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_FORMAT_BASE64_H */
