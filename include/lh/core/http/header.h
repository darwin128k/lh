/**
 * @file header.h
 * @brief Non-owning HTTP field views; populated by the parser.
 */

#ifndef LH_HTTP_HEADER_H
#define LH_HTTP_HEADER_H

#include <lh/core/str/view.h>
#include <lh/core/http/header/fields.h>

/**
 * @struct lh_http_header
 * @brief One parsed HTTP field: its name and its value.
 *
 * Both views point into the caller's input buffer and expire with it.
 */
struct lh_http_header
{
    /** Members: ::lh_http_header_fields. */
    lh_http_header_fields(lh_str_view_t);
};
/** @copybrief lh_http_header */
typedef struct lh_http_header lh_http_header_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The field name, spelled as received (compare it without ASCII case).
 * The view points into the parsed input and lives as long as that buffer.
 */
const lh_str_view_t *
lh_http_header_get_name(const lh_http_header_t *self);

/**
 * @brief Store the field name, spelled as received (compare it without ASCII case).
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_header_set_name(lh_http_header_t *self, lh_str_view_t name);

/**
 * @brief The field value without its leading and trailing spaces and tabs.
 * The view points into the parsed input and lives as long as that buffer.
 */
const lh_str_view_t *
lh_http_header_get_value(const lh_http_header_t *self);

/**
 * @brief Store the field value without its leading and trailing spaces and tabs.
 *
 * Level-1 setter: no validation; the parser fills it.
 */
lh_void
lh_http_header_set_value(lh_http_header_t *self, lh_str_view_t value);

/**
 * @brief Store both borrowed views of an HTTP field.
 *
 * The source storage remains caller-owned and must outlive the field. The
 * names and values are stored as given; this function does not validate them.
 *
 * @param self Field to update.
 * @param name Borrowed field name.
 * @param value Borrowed field value, which may be empty.
 * @fails ::lh_runtime_error_code_null_pointer @p self is null.
 */
lh_void
lh_http_header_set(lh_http_header_t *self, lh_str_view_t name, lh_str_view_t value);

/**
 * @brief Find the first HTTP field with the given name, ignoring ASCII case.
 *
 * The fields and their name views are borrowed. A zero count permits a null
 * array. Field names follow HTTP's ASCII case-insensitive comparison.
 *
 * @param headers Array of fields, retained by the caller.
 * @param count Number of fields in the array.
 * @param name Field name to find, retained by the caller.
 * @return A pointer into @p headers, or ::lh_null when no field matches.
 * @pre A nonzero @p count requires @p headers to point to that many fields.
 */
const lh_http_header_t *
lh_http_header_find(const lh_http_header_t *headers, lh_usize_t count,
                    const lh_str_view_t *name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_HEADER_H */
