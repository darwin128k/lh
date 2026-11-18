/**
 * @file fn.h
 * @brief Function type ::lh_gx_field_fn.
 */

#ifndef LH_GX_FIELD_FN_H
#define LH_GX_FIELD_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_gx_field;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_field_fn
 * @brief The application's reaction to @p field: its text changed, or Enter was
 *        pressed in it.
 *
 * The text is in the application's buffer (::lh_gx_field_get_text); it is
 * already drawn. A change by the program (::lh_gx_field_set_text) calls no
 * reaction.
 *
 * @param field   The field.
 * @param context What the application gave the field with the reaction.
 */
typedef lh_void(lh_gx_field_fn)(struct lh_gx_field *field, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_FIELD_FN_H */
