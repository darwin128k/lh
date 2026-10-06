/**
 * @file canvas.h
 * @brief Where an entity tree draws: ::lh_ui_canvas_t, a backend plus context.
 *
 * Callers use begin / clear / fill_rect / end; the bound
 * ::lh_ui_canvas_backend_t does the pixels. ::lh_ui_entity_draw hands the
 * canvas to every class draw event. No style cascade, no draw-task queue.
 */

#ifndef LH_UI_CANVAS_H
#define LH_UI_CANVAS_H

#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/ui/canvas/backend.h>
#include <lh/ui/canvas/fields.h>
#include <lh/ui/color.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

/**
 * @struct lh_ui_canvas
 * @typedef lh_ui_canvas_t
 * @brief Active draw target: backend and context.
 */
struct lh_ui_canvas
{
    lh_ui_canvas_fields(lh_ui_canvas_backend_t, lh_ptr);
};
typedef struct lh_ui_canvas lh_ui_canvas_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Bind @p self to @p backend and @p context. Neither is owned.
 *
 * ::lh_null @p backend makes every call a no-op until
 * ::lh_ui_canvas_set_backend. ::lh_ui_canvas_backend_null says the same
 * thing explicitly.
 */
lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, const lh_ui_canvas_backend_t *backend, lh_ptr context);

/**
 * @brief Drop the backend and context of @p self.
 */
lh_void
lh_ui_canvas_deinit(lh_ui_canvas_t *self);

/**
 * @brief Replace the backend of @p self. Not owned.
 */
lh_void
lh_ui_canvas_set_backend(lh_ui_canvas_t *self, const lh_ui_canvas_backend_t *backend);

/**
 * @brief Backend of @p self, or ::lh_null.
 */
const lh_ui_canvas_backend_t *
lh_ui_canvas_get_backend(const lh_ui_canvas_t *self);

/**
 * @brief Replace the context of @p self.
 */
lh_void
lh_ui_canvas_set_context(lh_ui_canvas_t *self, lh_ptr context);

/**
 * @brief Context of @p self.
 */
lh_ptr
lh_ui_canvas_get_context(const lh_ui_canvas_t *self);

/**
 * @brief Start a frame.
 */
lh_void
lh_ui_canvas_begin(lh_ui_canvas_t *self);

/**
 * @brief Finish a frame.
 */
lh_void
lh_ui_canvas_end(lh_ui_canvas_t *self);

/**
 * @brief Fill the whole target with @p color.
 */
lh_void
lh_ui_canvas_clear(lh_ui_canvas_t *self, const lh_ui_color_t *color);

/**
 * @brief Fill @p rect with the solid @p color.
 */
lh_void
lh_ui_canvas_fill_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_H */
