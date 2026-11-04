/**
 * @file format.h
 * @brief Serialize HTTP/1 response objects and fields into caller-owned bytes.
 *
 * The formatter composes the string formatting bricks. Response and header
 * views borrow their source storage; the output belongs to the caller. Each
 * operation appends at an offset and can leave a partial prefix on failure.
 * It does not write a NUL, validate field syntax or provide message framing.
 */

#ifndef LH_HTTP_FORMAT_H
#define LH_HTTP_FORMAT_H

#include <lh/core/http/header.h>
#include <lh/core/http/response.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Append the HTTP/1 status line of a response.
 *
 * @param response Caller-owned response; its reason is a valid borrowed view.
 * @param out Caller-owned writable bytes.
 * @param capacity Output capacity in bytes.
 * @param offset Append position, advanced for each successful part.
 * @return ::lh_bool_true on success; ::lh_bool_false on insufficient room or
 *         an invalid status (outside 100..599) or minor version (outside 0..9).
 * @pre All pointers are valid, @p out holds @p capacity bytes, the initial
 *      offset is at most capacity, and the reason contains valid HTTP bytes.
 */
lh_bool_t
lh_http_format_try_status_line(const lh_http_response_t *response, lh_str_ptr out,
                               lh_usize_t capacity, lh_usize_t *offset);

/**
 * @brief Append a status line and check that it fits and is valid.
 *
 * Delegates to ::lh_http_format_try_status_line. Input and output storage
 * remain owned by the caller; output may contain a partial prefix on failure.
 *
 * @param response Response with a valid borrowed reason view.
 * @param out Caller-owned writable bytes.
 * @param capacity Output capacity in bytes.
 * @param offset Append position, advanced for each successful part.
 * @pre The pointers, output capacity, initial offset and reason bytes satisfy
 *      ::lh_http_format_try_status_line's preconditions.
 * @fails ::lh_runtime_error_code_invalid_range Formatting failed.
 */
lh_void
lh_http_format_status_line(const lh_http_response_t *response, lh_str_ptr out, lh_usize_t capacity,
                           lh_usize_t *offset);

/**
 * @brief Append one HTTP field, including its terminating CRLF.
 *
 * @param header Caller-owned field with borrowed name and value views.
 * @param out Caller-owned writable bytes.
 * @param capacity Output capacity in bytes.
 * @param offset Append position, advanced for each successful part.
 * @return ::lh_bool_true on success, ::lh_bool_false if a part does not fit.
 * @pre The field name is a nonempty HTTP token, the value has valid HTTP
 *      field bytes, all pointers are valid and the initial offset is at most
 *      capacity. The output contains at least @p capacity writable bytes.
 */
lh_bool_t
lh_http_format_try_header(const lh_http_header_t *header, lh_str_ptr out, lh_usize_t capacity,
                          lh_usize_t *offset);

/**
 * @brief Append a field and check that it fits.
 *
 * Delegates to ::lh_http_format_try_header. The caller retains all storage;
 * output may contain a partial prefix on failure.
 *
 * @param header Field with borrowed name and value views.
 * @param out Caller-owned writable bytes.
 * @param capacity Output capacity in bytes.
 * @param offset Append position, advanced for each successful part.
 * @pre The field, pointers, capacity and initial offset satisfy
 *      ::lh_http_format_try_header's preconditions.
 * @fails ::lh_runtime_error_code_invalid_range Formatting failed.
 */
lh_void
lh_http_format_header(const lh_http_header_t *header, lh_str_ptr out, lh_usize_t capacity,
                      lh_usize_t *offset);

/**
 * @brief Append a response status line, its fields and the final empty line.
 *
 * @param response Caller-owned response; its header count selects the array size.
 * @param headers Caller-owned fields, or null when the count is zero.
 * @param out Caller-owned writable bytes.
 * @param capacity Output capacity in bytes.
 * @param offset Append position, advanced for each successful part.
 * @return ::lh_bool_true on success; ::lh_bool_false on invalid status or
 *         version, or when output capacity is insufficient.
 * @pre The status line and fields satisfy their individual preconditions;
 *      @p headers contains at least the response's header count entries.
 */
lh_bool_t
lh_http_format_try_response_head(const lh_http_response_t *response,
                                 const lh_http_header_t *headers, lh_str_ptr out,
                                 lh_usize_t capacity, lh_usize_t *offset);

/**
 * @brief Append a response head and check that it fits and is valid.
 *
 * Delegates to ::lh_http_format_try_response_head. The caller retains all
 * storage; output may contain a partial prefix on failure.
 *
 * @param response Response whose header count selects the array size.
 * @param headers Caller-owned fields, or null when the count is zero.
 * @param out Caller-owned writable bytes.
 * @param capacity Output capacity in bytes.
 * @param offset Append position, advanced for each successful part.
 * @pre The response, fields, pointers, capacity and offset satisfy
 *      ::lh_http_format_try_response_head's preconditions.
 * @fails ::lh_runtime_error_code_invalid_range Formatting failed.
 */
lh_void
lh_http_format_response_head(const lh_http_response_t *response, const lh_http_header_t *headers,
                             lh_str_ptr out, lh_usize_t capacity, lh_usize_t *offset);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_FORMAT_H */
