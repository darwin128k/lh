/**
 * @file fn.h
 * @brief Function type ::lh_gx_checkbox_change_fn.
 */

#ifndef LH_GX_CHECKBOX_CHANGE_FN_H
#define LH_GX_CHECKBOX_CHANGE_FN_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_gx_checkbox;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_checkbox_change_fn
 * @brief The application's reaction to the user changing @p checkbox.
 *
 * Called once per tap — a press on the checkbox and a release on it — after
 * the checkbox is drawn in its new state. Not called when the application
 * changes the state itself (::lh_gx_checkbox_check). The pointer is already
 * free, so the reaction may change the screen.
 *
 * @param checkbox The checkbox changed.
 * @param checked  Its new state.
 * @param context  What the application gave the checkbox with the reaction.
 */
typedef lh_void(lh_gx_checkbox_change_fn)(struct lh_gx_checkbox *checkbox, lh_bool_t checked,
                                          lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CHECKBOX_CHANGE_FN_H */
