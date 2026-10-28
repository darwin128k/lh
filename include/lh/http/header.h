/**
 * @file header.h
 * @brief Non-owning HTTP field views; populated by the parser.
 */

#ifndef LH_HTTP_HEADER_H
#define LH_HTTP_HEADER_H

#include <lh/str/view.h>
#include <lh/http/header/fields.h>

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

LH_COMPILER_EXTERN_C_END

#endif /* LH_HTTP_HEADER_H */
