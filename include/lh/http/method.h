/**
 * @file method.h
 * @brief Names of known HTTP request methods.
 *
 * The enum is a convenience for known methods; arbitrary extension tokens
 * remain valid in request and route views. Returned names borrow static LH
 * storage and require no allocation or cleanup.
 */

#ifndef LH_HTTP_METHOD_H
#define LH_HTTP_METHOD_H

#include <lh/http/method/kind.h>
#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Get the case-sensitive wire token of a known method.
 * @param method Method to name.
 * @return Borrowed static token, or an empty view for an unknown enum value.
 *         The caller must not modify or free the returned bytes.
 */
lh_str_view_t
lh_http_method_get_name(lh_http_method_kind_t method);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_METHOD_H */
