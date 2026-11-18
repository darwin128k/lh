/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_render_t.
 */

#ifndef LH_GX_RENDER_FIELDS_H
#define LH_GX_RENDER_FIELDS_H

#include <lh/core/ptr.h>

/**
 * @def lh_gx_render_fields(entity_type, damage_type, canvas_type)
 * @brief The screen, what changed on it, and the canvas it is drawn on.
 *
 * `screen` is the root of the tree, embedded: the application attaches its own
 * entities to it, and a change anywhere below reaches the renderer through the
 * screen's class (::lh_gx_render_screen_class). `damage` is what changed since
 * the last frame — the only memory of the drawing, and it is here, not in the
 * canvas. `canvas` and `canvas_context` are where a frame is drawn; neither is
 * owned.
 *
 * @param entity_type Type of the screen (::lh_gx_entity_t).
 * @param damage_type Type of the damage (::lh_gx_damage_t).
 * @param canvas_type Type of the canvas table (::lh_gx_canvas_t).
 */
#define lh_gx_render_fields(entity_type, damage_type, canvas_type)                                 \
    entity_type screen;                                                                            \
    damage_type damage;                                                                            \
    const canvas_type *canvas;                                                                     \
    lh_ptr canvas_context

#endif /* LH_GX_RENDER_FIELDS_H */
