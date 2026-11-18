/**
 * @file fn.h
 * @brief Function type ::lh_gx_class_input_fn.
 */

#ifndef LH_GX_CLASS_INPUT_FN_H
#define LH_GX_CLASS_INPUT_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_gx_entity;
struct lh_gx_event;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_class_input_fn
 * @brief Take @p event: the pointer was pressed on @p entity, moved, released or
 *        cancelled (::lh_gx_event_t).
 *
 * Optional, and what makes an entity take input at all: an entity whose class
 * has no `input` is passed over when the pointer looks for its target, so a
 * press goes to what is under it — the caption of a button is a text with no
 * `input`, and a press on it is a press on the button. There is no "clickable"
 * flag: a class takes input or it does not.
 *
 * The entity is not const: taking input changes it (a button is pressed). A
 * change that shows is reported by the class (::lh_gx_entity_invalidate).
 *
 * @param entity The entity.
 * @param event  What happened, and where on the screen.
 */
typedef lh_void(lh_gx_class_input_fn)(struct lh_gx_entity *entity, const struct lh_gx_event *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CLASS_INPUT_FN_H */
