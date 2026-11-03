/**
 * @file request.h
 * @brief HTTP/1 request-line views and parsed header count.
 */

#ifndef LH_HTTP_REQUEST_H
#define LH_HTTP_REQUEST_H

#include <lh/str/view.h>
#include <lh/http/method/kind.h>
#include <lh/http/request/fields.h>

/**
 * @struct lh_http_request
 * @brief Parsed request line: method, target, version and field count.
 *
 * The views point into the caller's input buffer and expire with it.
 */
struct lh_http_request
{
    /** Members: ::lh_http_request_fields. */
    lh_http_request_fields(lh_str_view_t, lh_u8_t, lh_usize_t);
};
/** @copybrief lh_http_request */
typedef struct lh_http_request lh_http_request_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The request method token, e.g. `GET`.
 * The view points into the parsed input and lives as long as that buffer.
 */
const lh_str_view_t *
lh_http_request_get_method(const lh_http_request_t *self);

/**
 * @brief Store the request method token, e.g. `GET`.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_request_set_method(lh_http_request_t *self, lh_str_view_t method);

/**
 * @brief The request target as raw URI text, e.g. `/maps/a.bsp?x=1`.
 * The view points into the parsed input and lives as long as that buffer.
 */
const lh_str_view_t *
lh_http_request_get_target(const lh_http_request_t *self);

/**
 * @brief Store the request target as raw URI text, e.g. `/maps/a.bsp?x=1`.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_request_set_target(lh_http_request_t *self, lh_str_view_t target);

/** @brief The minor digit of `HTTP/1.x` (0..9); which ones a service accepts is its policy. */
lh_u8_t
lh_http_request_get_version_minor(const lh_http_request_t *self);

/**
 * @brief Store the minor digit of `HTTP/1.x` (0..9); which ones a service accepts is its policy.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_request_set_version_minor(lh_http_request_t *self, lh_u8_t version_minor);

/** @brief Number of header fields the parse stored in the caller's array. */
lh_usize_t
lh_http_request_get_header_count(const lh_http_request_t *self);

/**
 * @brief Store number of header fields the parse stored in the caller's array.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_request_set_header_count(lh_http_request_t *self, lh_usize_t header_count);

/**
 * @brief Return the request target before its first query delimiter.
 *
 * The result is a borrowed view into the same input as the target. If there
 * is no query delimiter, it covers the entire target. This does not decode or
 * normalize the target, nor interpret an absolute-form URI.
 *
 * @param self Request whose target must be a valid view.
 * @return The path portion, including an empty view when the target starts
 *         with a query delimiter. The caller does not own its bytes.
 * @fails ::lh_runtime_error_code_null_pointer @p self is null.
 */
lh_str_view_t
lh_http_request_get_path(const lh_http_request_t *self);

/**
 * @brief Find the value following a query field prefix in the request target.
 *
 * The prefix is matched at the start of a field (after `?` or `&`). Include
 * the equals sign in @p key when it is part of the desired match. No decoding
 * is performed. The first match wins, including a match with an empty value.
 *
 * @param self Request with a valid target view.
 * @param key Borrowed prefix to find, normally a name followed by `=`.
 * @param key_len Size of @p key in bytes; zero never matches.
 * @param value Receives the borrowed value pointer on a match, including an
 *              empty value. It is unchanged when no match is found.
 * @return Value length in bytes, or zero for no match or an empty value.
 * @pre When @p key_len is nonzero, @p self and @p key are valid. If a match
 *      is found, @p value points to writable pointer storage.
 */
lh_usize_t
lh_http_request_get_query_value_size(const lh_http_request_t *self, const lh_str_ptr key,
                                     lh_usize_t key_len, lh_str_cptr *value);

/**
 * @brief Test a parsed request's method against a known HTTP method.
 *
 * The comparison is case-sensitive. Both the request and its method storage
 * remain caller-owned; unknown enum values never match.
 *
 * @param self Parsed request with a valid borrowed method view.
 * @param method Known method to compare.
 * @return ::lh_bool_true if the method matches, ::lh_bool_false otherwise.
 * @fails ::lh_runtime_error_code_null_pointer @p self is null.
 */
lh_bool_t
lh_http_request_is_method(const lh_http_request_t *self, lh_http_method_kind_t method);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_REQUEST_H */
