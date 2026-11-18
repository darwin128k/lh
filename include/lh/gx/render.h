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
 * backend it is used with. The walk is recursive, one level of the stack per
 * level of the tree.
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
 * @brief Find, in @p entity's subtree, the front-most entity that covers the
 *        whole of @p area opaquely, into @p best.
 *
 * An entity that is hidden, or whose rectangle does not hold the whole area, is
 * skipped with its subtree: its children are drawn inside it, so none of them
 * can cover what it does not. Entities later in the drawing order replace an
 * earlier find, so the one in front wins.
 *
 * @param entity The subtree's top; must not be null.
 * @param origin The screen position of @p entity's owner.
 * @param area   The area, screen coordinates.
 * @param best   Holds the best so far, ::lh_null for none; receives the new best.
 */
lh_void
lh_gx_render_find_cover(const lh_gx_entity_t *entity, lh_gx_point_t origin,
                        const lh_gx_rect_t *area, const lh_gx_entity_t **best);

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
 * @brief Draw @p entity's subtree inside @p clip, from @p cover on.
 *
 * Drawing order is the tree's: an owner before its children, the children from
 * the first to the last. While @p started is false, entities are not drawn —
 * every entity before @p cover in that order is hidden by it — and the walk only
 * goes down towards @p cover; reaching it sets @p started.
 *
 * @param self    The renderer; must not be null.
 * @param entity  The subtree's top; must not be null.
 * @param origin  The screen position of @p entity's owner.
 * @param clip    The area to draw in, screen coordinates.
 * @param cover   The entity to start from, or ::lh_null to start at once.
 * @param started Whether drawing has started; must not be null.
 */
lh_void
lh_gx_render_paint_subtree(const lh_gx_render_t *self, const lh_gx_entity_t *entity,
                           lh_gx_point_t origin, const lh_gx_rect_t *clip,
                           const lh_gx_entity_t *cover, lh_bool_t *started);

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
