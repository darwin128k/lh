/**
 * @file backend.h
 * @brief One pixel sink: ::lh_ui_canvas_backend_t (function table, no state).
 *
 * GDI, GL, software and STM32 paths each ship one of these, wherever they
 * live (`lh/os`, an application). ::lh_ui_canvas_t binds a backend to a
 * context and dispatches through it.
 */

#ifndef LH_UI_CANVAS_BACKEND_H
#define LH_UI_CANVAS_BACKEND_H

#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/ui/canvas/backend/fields.h>
#include <lh/ui/color.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_ui_canvas_begin_fn
 * @brief Start a frame on @p context.
 */
typedef lh_void(lh_ui_canvas_begin_fn)(lh_ptr context);

/**
 * @typedef lh_ui_canvas_end_fn
 * @brief Finish a frame on @p context (present / flush).
 */
typedef lh_void(lh_ui_canvas_end_fn)(lh_ptr context);

/**
 * @typedef lh_ui_canvas_clear_fn
 * @brief Fill the whole target with @p color.
 */
typedef lh_void(lh_ui_canvas_clear_fn)(lh_ptr context, const lh_ui_color_t *color);

/**
 * @typedef lh_ui_canvas_fill_rect_fn
 * @brief Fill @p rect with the solid @p color (axis-aligned, half-open).
 */
typedef lh_void(lh_ui_canvas_fill_rect_fn)(lh_ptr context, const lh_ui_rect_t *rect,
                                           const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

/**
 * @struct lh_ui_canvas_backend
 * @typedef lh_ui_canvas_backend_t
 * @brief Function table for one renderer implementation.
 *
 * Usually a static const; not owned by ::lh_ui_canvas_t. A slot may be
 * ::lh_null until that primitive exists; dispatch skips null slots.
 */
struct lh_ui_canvas_backend
{
    lh_ui_canvas_backend_fields(lh_ui_canvas_begin_fn, lh_ui_canvas_end_fn, lh_ui_canvas_clear_fn,
                                lh_ui_canvas_fill_rect_fn);
};
typedef struct lh_ui_canvas_backend lh_ui_canvas_backend_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Backend that accepts every call and draws nothing.
 */
extern const lh_ui_canvas_backend_t lh_ui_canvas_backend_null;

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_BACKEND_H */
