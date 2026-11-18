/**
 * @file fn.h
 * @brief Function type ::lh_gx_class_key_fn.
 */

#ifndef LH_GX_CLASS_KEY_FN_H
#define LH_GX_CLASS_KEY_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_gx_entity;
struct lh_gx_event;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_class_key_fn
 * @brief Take @p event: a key was pressed, a character typed, or @p entity got
 *        or lost the focus (::lh_gx_event_t).
 *
 * Optional, and what makes an entity take the focus at all: the focus
 * (::lh_gx_focus_t) settles only on an entity whose class has `key`, and a
 * press on one with no `key` gives the focus to the nearest owner that has it,
 * or to nobody. There is no "focusable" flag: a class takes keys or it does
 * not. A text field has `key`; a button, which needs no keyboard, does not.
 *
 * The entity is not const: a key changes it (a field's text). A change that
 * shows is reported by the class (::lh_gx_entity_invalidate).
 *
 * @param entity The entity with the focus.
 * @param event  A key, a text, a focus or a blur.
 */
typedef lh_void(lh_gx_class_key_fn)(struct lh_gx_entity *entity, const struct lh_gx_event *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CLASS_KEY_FN_H */
