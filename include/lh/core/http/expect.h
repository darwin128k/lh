/**
 * @file expect.h
 * @brief Inspect HTTP Expect fields without owning their borrowed views.
 *
 * Multiple fields and comma-separated members are combined. The supported
 * expectation is `100-continue`, matched without ASCII case. Other members are
 * rejected so a server can answer 417 before reading the request content.
 */

#ifndef LH_HTTP_EXPECT_H
#define LH_HTTP_EXPECT_H

#include <lh/core/http/header.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Check parsed Expect fields and report whether 100-continue was requested.
 *
 * Empty list members are ignored, but an entirely empty Expect value is not
 * supported. Input remains owned by its caller; output is unchanged on error.
 * @param headers Borrowed parsed fields, or null when @p count is zero.
 * @param count Number of fields.
 * @param out Receives true when 100-continue is present, false if Expect is absent.
 * @return True for supported expectations or no Expect field; false otherwise.
 * @pre @p out is valid and a nonzero count has that many valid parsed fields.
 */
lh_bool_t
lh_http_expect_try_get_continue(const lh_http_header_t *headers, lh_usize_t count, lh_bool_t *out);

/**
 * @brief Get whether 100-continue was requested, checking expectation support.
 * @param headers Borrowed fields, or null when @p count is zero.
 * @param count Number of fields.
 * @return True when requested, false when Expect is absent.
 * @pre Nonzero @p count has that many valid parsed fields.
 * @fails ::lh_runtime_error_code_invalid_range An expectation is unsupported.
 */
lh_bool_t
lh_http_expect_get_continue(const lh_http_header_t *headers, lh_usize_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_EXPECT_H */
