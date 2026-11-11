/**
 * @file target.h
 * @brief A request target as a safe relative file path.
 *
 * A static file server maps the path of a request target onto files below a
 * root. Before it does, the path must not escape the root nor be read
 * differently by the file system than by the server. These bricks do that part:
 *
 * - ::lh_http_target_try_decode_path drops the query and the fragment, requires a
 *   leading `/`, and decodes `%XX` exactly once into the caller's buffer. An
 *   encoded separator or NUL never becomes one, and every decoded byte must be one
 *   a published name may hold (::lh_http_is_name_char).
 * - ::lh_http_target_get_path_error checks the components of a decoded path: none
 *   empty, none hidden (`.`, `..`, `.name`), none that Windows would trim or alias
 *   (a trailing `.` or space, an 8.3 short name such as `NAME~1`).
 * - ::lh_http_target_try_copy_path is the two in turn.
 *
 * The rules are the same on every platform, so a path refused on Windows is
 * refused on Linux too. Which directories and extensions may be served is the
 * caller's policy, not these bricks'.
 */

#ifndef LH_HTTP_TARGET_H
#define LH_HTTP_TARGET_H

#include <lh/core/http/target/error.h>
#include <lh/core/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Decode the path of @p target into @p out, without its leading `/`,
 *        NUL-terminated.
 *
 * @param target   The request target (raw URI text); borrowed.
 * @param out      Destination buffer (not null).
 * @param capacity Capacity of @p out, the terminator included; must not be 0.
 * @param error    Receives ::lh_http_target_error_none on success, otherwise why
 *                 the target was refused (`bad_target`, `bad_encoding`,
 *                 `encoded_separator`, `bad_char`, `too_long`).
 *
 * @return Length of the decoded path; 0 on refusal, and @p out is then empty.
 */
lh_usize_t
lh_http_target_try_decode_path(const lh_str_view_t *target, lh_str_ptr out, lh_usize_t capacity,
                               lh_http_target_error_t *error);

/**
 * @brief Why the decoded relative @p path is not safe to open, or
 *        ::lh_http_target_error_none.
 *
 * @param path A path as ::lh_http_target_try_decode_path writes it: no leading
 *             `/`, components separated by `/`.
 *
 * @return `empty_component`, `hidden_or_dot`, `trailing_dot_or_space`,
 *         `short_name`, or ::lh_http_target_error_none.
 */
lh_http_target_error_t
lh_http_target_get_path_error(const lh_str_view_t *path);

/**
 * @brief ::lh_http_target_try_decode_path, then ::lh_http_target_get_path_error.
 *
 * @return Length of the safe relative path in @p out; 0 on refusal, with
 *         @p error saying why and @p out empty.
 */
lh_usize_t
lh_http_target_try_copy_path(const lh_str_view_t *target, lh_str_ptr out, lh_usize_t capacity,
                             lh_http_target_error_t *error);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_TARGET_H */
