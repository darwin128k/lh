/**
 * @file version.h
 * @brief The HTTP/1 version token: `HTTP/1.` followed by one minor digit.
 *
 * Which minor versions a service accepts stays application policy; this brick
 * only reads the token.
 */

#ifndef LH_HTTP_VERSION_H
#define LH_HTTP_VERSION_H

#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the minor version of an HTTP/1 version token.
 *
 * @param text Exactly the token, e.g. `HTTP/1.1`; nothing before or after it.
 * @param minor Receives the minor version (0..9); untouched on failure.
 * @return ::lh_bool_false when @p text is not eight bytes of `HTTP/1.` and a
 *         decimal digit.
 */
lh_bool_t
lh_http_parse_version(lh_str_view_t text, lh_u8_t *minor);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_VERSION_H */
