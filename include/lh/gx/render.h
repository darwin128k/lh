/**
 * @file render.h
 * @brief The part that knows what to draw: ::lh_gx_render_t.
 *
 * The renderer holds the screen — the root of the tree, embedded in it — and the
 * damage: what changed since the last frame. It is the one part of gx that
 * remembers anything about drawing (`docs/design/gx.md`, section 5.1):
 *
 * - an entity that changes reports its rectangle up to the root, and the
 *   screen's class (::lh_gx_render_screen_class) adds it to the damage;
 * - a frame (::lh_gx_render_draw) takes the damage, and for every rectangle of
 *   it walks the tree in drawing order: hidden entities and entities that do
 *   not meet the rectangle are skipped with their whole subtree, every entity
 *   draws inside its owner (the clip is cut by every owner on the way down), and
 *   the class of each draws through a ::lh_gx_paint_t;
 * - before the walk, the front-most entity that covers the whole rectangle
 *   opaquely (`covers` of its class) is found, and nothing under it is drawn;
 * - the rectangles drawn are handed back, for the present to show — only they
 *   go to the screen.
 *
 * The canvas below keeps nothing, so the same renderer draws into a
 * screen-sized buffer, a strip that moves down the screen
 * (::lh_gx_render_paint_area with the strip as the area), or a JSON stream.
 *
 * Ownership: the renderer is the application's, as is the canvas table and the
 * backend it is used with. The walks are not recursive: the owner links of the
 * tree are the way back up, so the stack a frame needs does not grow with the
 * depth of the tree — what a board with a small stack needs.
 */

#ifndef LH_GX_RENDER_H
#define LH_GX_RENDER_H

#include <lh/gx/class.h>
#include <lh/gx/damage.h>
#include <lh/gx/entity.h>
#include <lh/gx/paint.h>
#include <lh/gx/render/fields.h>

/**
 * @struct lh_gx_render
 * @brief The screen, the damage, and the canvas.
 */
struct lh_gx_render
{
    /** Members: ::lh_gx_render_fields. */
    lh_gx_render_fields(lh_gx_entity_t, lh_gx_damage_t, lh_gx_canvas_t);
};
/** @copybrief lh_gx_render */
typedef struct lh_gx_render lh_gx_render_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class of a renderer's screen: it draws nothing, and its `report`
 *        adds what changed to the renderer's damage (::lh_gx_render_report).
 */
extern const lh_gx_class_t lh_gx_render_screen_class;

/**
 * @brief The screen of @p self: the root the application attaches to (level 1).
 *
 * @param self The renderer; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_entity_t *
lh_gx_render_get_screen(const lh_gx_render_t *self);

/**
 * @brief The screen of @p self, to attach to (level 1).
 *
 * @param self The renderer; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_render_get_screen_mut(lh_gx_render_t *self);

/**
 * @brief What changed since the last frame (level 1).
 *
 * @param self The renderer; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_damage_t *
lh_gx_render_get_damage(const lh_gx_render_t *self);

/**
 * @brief What changed since the last frame, to change in place (level 1).
 *
 * @param self The renderer; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_damage_t *
lh_gx_render_get_damage_mut(lh_gx_render_t *self);

/**
 * @brief The canvas table frames are drawn with (level 1).
 *
 * @param self The renderer; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_canvas_t *
lh_gx_render_get_canvas(const lh_gx_render_t *self);

/**
 * @brief Store the canvas table frames are drawn with (level 1).
 *
 * @param self   The renderer; checked by ::lh_assert_runtime_ref.
 * @param canvas The table; not owned, it must have `fill_rect`.
 */
lh_void
lh_gx_render_set_canvas(lh_gx_render_t *self, const lh_gx_canvas_t *canvas);

/**
 * @brief The context every canvas call gets: the backend (level 1).
 *
 * @param self The renderer; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gx_render_get_canvas_context(const lh_gx_render_t *self);

/**
 * @brief Store the context every canvas call gets (level 1).
 *
 * @param self    The renderer; checked by ::lh_assert_runtime_ref.
 * @param context The backend; not owned.
 */
lh_void
lh_gx_render_set_canvas_context(lh_gx_render_t *self, lh_ptr context);

/**
 * @brief The renderer whose screen is @p screen.
 *
 * @param screen The screen of a renderer — an entity of
 *               ::lh_gx_render_screen_class, embedded in its renderer.
 * @return The renderer.
 */
lh_gx_render_t *
lh_gx_render_from_screen(lh_gx_entity_t *screen);

/**
 * @brief The front-most entity on the screen that covers the whole of @p area
 *        opaquely, or ::lh_null.
 *
 * A walk in drawing order, without recursion: an entity that is hidden, or whose
 * rectangle does not hold the whole area, is skipped with its subtree — its
 * children are drawn inside it, so none of them can cover what it does not. A
 * later find replaces an earlier one, so the one in front wins.
 *
 * @param self The renderer; must not be null.
 * @param area The area, screen coordinates.
 * @return The entity to start drawing from, or ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_render_find_cover(const lh_gx_render_t *self, const lh_gx_rect_t *area);

/**
 * @brief Draw @p entity alone through its class's `paint`, at @p place inside
 *        @p clip; nothing when the class has no `paint`.
 *
 * @param self   The renderer; must not be null.
 * @param entity The entity; must not be null.
 * @param place  Its rectangle on the screen.
 * @param clip   What it may touch, screen coordinates.
 */
lh_void
lh_gx_render_paint_entity(const lh_gx_render_t *self, const lh_gx_entity_t *entity,
                          const lh_gx_rect_t *place, const lh_gx_rect_t *clip);

/**
 * @brief Add @p area, cut by the screen, to what changed.
 *
 * @param self The renderer; must not be null.
 * @param area The rectangle, screen coordinates.
 */
lh_void
lh_gx_render_add_damage(lh_gx_render_t *self, const lh_gx_rect_t *area);

/**
 * @brief True when something changed since the last frame.
 *
 * @param self The renderer; must not be null.
 */
lh_bool_t
lh_gx_render_has_damage(const lh_gx_render_t *self);

/**
 * @brief Move what changed into @p out and start the next frame with nothing
 *        changed.
 *
 * @param self The renderer; must not be null.
 * @param out  Receives the damage; must not be null.
 */
lh_void
lh_gx_render_take_damage(lh_gx_render_t *self, lh_gx_damage_t *out);

/**
 * @brief Draw everything on the screen inside @p area, from @p cover on.
 *
 * A walk in drawing order — an owner before its children, the children from the
 * first to the last — without recursion: the tree's owner links are the way back
 * up, so the stack does not grow with the depth of the tree. Going down and
 * across, the clip and the position carry over; going up, they are found again
 * from the owners (::lh_gx_entity_get_owner_clip,
 * ::lh_gx_entity_get_owner_origin). Hidden entities and entities that miss the
 * clip are skipped with their subtree. Until @p cover is reached nothing is
 * drawn — everything before it is under it — and the walk goes down only
 * towards it.
 *
 * @param self  The renderer; must not be null.
 * @param area  The area, screen coordinates.
 * @param cover The entity to start drawing from, or ::lh_null to draw everything.
 */
lh_void
lh_gx_render_paint_from(const lh_gx_render_t *self, const lh_gx_rect_t *area,
                        const lh_gx_entity_t *cover);

/**
 * @brief `report` of the screen's class (::lh_gx_class_report_fn): add @p area to
 *        the damage of the renderer @p root belongs to.
 *
 * @param root The screen of a renderer.
 * @param area The changed rectangle, screen coordinates.
 */
lh_void
lh_gx_render_report(lh_gx_entity_t *root, const lh_gx_rect_t *area);

/**
 * @brief A renderer of a @p width by @p height screen, drawn with @p canvas and
 *        @p context.
 *
 * The screen starts with no children, and the whole of it is damaged, so the
 * first frame draws everything.
 *
 * @param self    The renderer; must not be null. It must not move while entities
 *                are attached to its screen.
 * @param canvas  The canvas table; must have `fill_rect`.
 * @param context The backend the table is used with.
 * @param width   The width of the screen.
 * @param height  The height of the screen.
 */
lh_void
lh_gx_render_init(lh_gx_render_t *self, const lh_gx_canvas_t *canvas, lh_ptr context,
                  lh_gx_scalar_t width, lh_gx_scalar_t height);

/**
 * @brief Draw everything inside @p area: from the front-most entity that covers
 *        it, or from the screen.
 *
 * The damage is not touched. A strip renderer calls this once per strip, with
 * the strip as the area and the backend's origin moved to the strip.
 *
 * @param self The renderer; must not be null.
 * @param area The area, screen coordinates.
 */
lh_void
lh_gx_render_paint_area(const lh_gx_render_t *self, const lh_gx_rect_t *area);

/**
 * @brief Draw one frame: every rectangle that changed, and only those.
 *
 * @param self The renderer; must not be null.
 * @param out  Receives the rectangles drawn — what the present has to show;
 *             empty when nothing changed.
 */
lh_void
lh_gx_render_draw(lh_gx_render_t *self, lh_gx_damage_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_RENDER_H */
