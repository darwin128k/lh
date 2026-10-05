/**
 * @file scroll.h
 * @brief A scrollbar. A trackbar whose thumb shows how much is visible.
 *
 * The record starts with ::lh_entity_range_t, and its ends are the offset into
 * the content: 0 is the first pixel, and the last end is the last one. The
 * page is how much of the content is on screen, counted in the same pixels, so
 * a content of (last end + page) is what a full length thumb stands for. The
 * box is the thickness and the length, and which of the two is the length is
 * ::lh_entity_range_set_axis, reached through
 * ::lh_entity_scroll_get_range.
 *
 * The track and the thumb are rounded to half their thickness. The track is
 * the style background. The thumb is ::lh_entity_scroll_set_thumb, or the text
 * color when that is null. Whether the bar is there at all is
 * ::lh_entity_scroll_set_mode. A press is ::lh_entity_scroll_press: on the
 * thumb it takes hold of it, and anywhere else it turns a page towards the
 * pointer. A press, a drag and a page all come through
 * ::lh_entity_scroll_set_value, which scrolls a linked view and sends
 * ::LH_ENTITY_EVENT_CLICKED when the value moved.
 *
 * Every number in the paint path is a field or a constant named here: the
 * thumb's length comes from the page, its floor from
 * ::LH_ENTITY_SCROLL_THUMB_MIN, and its room from
 * ::LH_ENTITY_SCROLL_INSET under the one rule of ::LH_ENTITY_SCROLL_INSET_CAP.
 */

#ifndef LH_ENTITY_SCROLL_H
#define LH_ENTITY_SCROLL_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/range.h>
#include <lh/ui/style.h>

struct lh_entity_view;
/* view.h declares it as well. C11 and C++ both allow the same typedef twice,
   and this is the half of the header that must know the view exists. */
typedef struct lh_entity_view lh_entity_view_t;

/**
 * @def LH_ENTITY_SCROLL_SHOW_AUTO
 * @brief The bar is there only when there is something to move.
 *
 * The initial mode, and the right one by default: a bar over content that fits
 * is a bar over nothing.
 */
#define LH_ENTITY_SCROLL_SHOW_AUTO 0

/**
 * @def LH_ENTITY_SCROLL_SHOW_ALWAYS
 * @brief The bar is there even when there is nothing to move, and stays
 *        grabbable at both ends.
 */
#define LH_ENTITY_SCROLL_SHOW_ALWAYS 1

/**
 * @def LH_ENTITY_SCROLL_SHOW_NEVER
 * @brief The bar is not there, whatever the ends say. Its ends still work, so
 *        a value can be set on a bar nobody can see.
 */
#define LH_ENTITY_SCROLL_SHOW_NEVER 2

/**
 * @def LH_ENTITY_SCROLL_THUMB_MIN
 * @brief The shortest thumb a new bar draws, in pixels.
 *
 * The default, not a constant in the paint path:
 * ::lh_entity_scroll_set_thumb_min changes it. A deep content would otherwise
 * give a thumb too small to catch.
 */
#define LH_ENTITY_SCROLL_THUMB_MIN 8

/**
 * @def LH_ENTITY_SCROLL_INSET
 * @brief The room a new bar leaves at both sides of its thumb, across the
 *        thickness, in pixels.
 *
 * The default, not a constant in the paint path:
 * ::lh_entity_scroll_set_thumb_inset changes it. It is what keeps the track
 * showing at both sides past the thumb instead of the thumb covering it, and it
 * never takes more than a quarter of the thickness, so a thin bar still shows a
 * thumb. It is across the thickness and not along the track: a thumb at the
 * start of the travel begins at the start of the track.
 */
#define LH_ENTITY_SCROLL_INSET 2

/**
 * @def LH_ENTITY_SCROLL_PAGE
 * @brief The page a new bar starts with, in the same units as the value.
 *
 * The default, not a constant in the paint path:
 * ::lh_entity_scroll_set_page changes it, and a linked ::lh_entity_view_t
 * overwrites it with its own size on every draw.
 */
#define LH_ENTITY_SCROLL_PAGE 10

/**
 * @def LH_ENTITY_SCROLL_INSET_CAP
 * @brief The most of the thickness, in parts, that the inset at both sides of
 *        the thumb may take.
 *
 * Two sides and this many parts, so the thumb always keeps at least half the
 * thickness to be seen by, and a bar of any thickness obeys it. This is a rule
 * about the shape rather than a setting: the only number in this header that
 * ::lh_entity_scroll_set_thumb_inset cannot reach, and the reason it is
 * reachable is that a caller who wants a wider thumb asks for less inset.
 */
#define LH_ENTITY_SCROLL_INSET_CAP 4

/**
 * @struct lh_entity_scroll
 * @brief A thumb on a track.
 */
struct lh_entity_scroll
{
    lh_entity_range_t range;
    struct lh_entity_view *view;
    const lh_ui_style_t *thumb;
    lh_int_t page;
    lh_int_t mode;
    lh_int_t min_thumb;
    lh_int_t inset;
    lh_int_t grab;
    lh_bool_t dragging;
};
typedef struct lh_entity_scroll lh_entity_scroll_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_scroll_t, derived from ::lh_entity_2d_class.
 *
 * A new bar runs from 0 to 100 with a page of 10, on
 * ::LH_ENTITY_SCROLL_SHOW_AUTO, with the default thumb floor and inset.
 */
extern const lh_entity_class_t lh_entity_scroll_class;

/**
 * @brief How much of the whole the thumb stands for.
 */
lh_int_t
lh_entity_scroll_get_page(const lh_entity_scroll_t *self);

/**
 * @brief Set the thumb's share of the whole. Zero and below become 1.
 */
lh_void
lh_entity_scroll_set_page(lh_entity_scroll_t *self, lh_int_t page);

/**
 * @brief Style of the thumb, or ::lh_null when the track's text color is used.
 */
const lh_ui_style_t *
lh_entity_scroll_get_thumb(const lh_entity_scroll_t *self);

/**
 * @brief Point the thumb at @p style. Not owned, and not copied.
 *        ::lh_null uses the track's text color.
 */
lh_void
lh_entity_scroll_set_thumb(lh_entity_scroll_t *self, const lh_ui_style_t *style);

/**
 * @brief The value record of @p self: ends, start, current value, thickness,
 *        the way the bar runs and the travel it has. The API is
 *        ::lh_entity_range_t's, not this widget's, and this is how a caller
 *        reaches it. The page, what one screenful covers, is this widget's own
 *        and is not part of it.
 *
 * The range is a member by value, so this hands back @p self's own record:
 * writing through it changes the widget, and nothing is allocated.
 */
lh_entity_range_t *
lh_entity_scroll_get_range(lh_entity_scroll_t *self);

/**
 * @brief Draw the track and the thumb of @p self into @p canvas.
 *
 * The track is the whole box in the style background, and the thumb is
 * ::lh_entity_scroll_thumb. Nothing is drawn without a style.
 */
lh_void
lh_entity_scroll_paint(const lh_entity_scroll_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief What a press at @p point, in screen coordinates, means: take hold of
 *        the thumb under it, or turn one page towards it.
 *
 * This is the whole of a press, so a caller with a point of its own can do it
 * without an event. A press on the thumb is a grab and says so by returning
 * true, so the moves that follow drag it. A press anywhere else on the track
 * turns ::lh_entity_scroll_get_page towards the pointer and holds nothing: a
 * move after it leaves the value alone, because a bar dragged under a pointer
 * that never took hold of it is a bar that jumped there, and a press on the
 * track is not that.
 *
 * The point of the thumb the press landed on is the one the drag keeps under
 * the pointer, so taking hold of an edge does not throw the thumb over to its
 * middle.
 */
lh_bool_t
lh_entity_scroll_press(lh_entity_scroll_t *self, const lh_math_vec2_t *point);

/**
 * @brief Drag the thumb to @p point, which is in screen coordinates, and scroll
 *        a linked view if the value moved.
 *
 * The point of the thumb that ::lh_entity_scroll_press took hold of is the one
 * that rides under the pointer, over the room left after the thumb's own
 * length, which is the same room ::lh_entity_scroll_thumb places it in. A
 * caller with no press behind it moves the thumb's own origin to the pointer,
 * which is the same rule with nothing taken hold of. A drag past either end
 * stops at it, and one that does not change the value sends nothing and
 * touches no view.
 */
lh_void
lh_entity_scroll_apply(lh_entity_scroll_t *self, const lh_math_vec2_t *point);

/**
 * @brief Turn @p self by @p pages of its page, the way it runs.
 *
 * A positive count is forward along the bar, which is the way the value and a
 * view's offset run, and zero is nothing. The value stops at the ends either
 * way, so a page past the end of the content is the end of it, and one that
 * changes nothing sends nothing. This is the step a press on the track takes,
 * and the one a key would take.
 */
lh_void
lh_entity_scroll_page_by(lh_entity_scroll_t *self, lh_int_t pages);

/**
 * @brief Set the value of @p self to @p value, and when that moved, scroll a
 *        linked view and send ::LH_ENTITY_EVENT_CLICKED.
 *
 * This is the one door the bar's own input comes through, so a value set by hand
 * scrolls its view exactly as a drag or a page does. The quiet way in is
 * ::lh_entity_range_set_value, which writes the number and redraws and tells
 * nobody: that is how a ::lh_entity_view_t passes its offset on, since the view
 * has already moved and the bar is only being told. The value is pulled into the
 * ends first, and true comes back only when it moved.
 */
lh_bool_t
lh_entity_scroll_set_value(lh_entity_scroll_t *self, lh_int_t value);

/**
 * @brief Where the thumb sits on the track @p bounds, as a box.
 *
 * The long side of @p bounds is the track, taken from the axis. The thumb is
 * ::LH_ENTITY_SCROLL_THUMB_MIN long at the very least, and the whole track when
 * there is nothing to move, which is what an empty extent means. It is inset
 * by ::LH_ENTITY_SCROLL_INSET on both sides of the thick way, as far as a
 * quarter of the thickness. The origin comes from the value, over the travel
 * left after the thumb's own length, so the value and the thumb agree.
 *
 * ::lh_entity_scroll_paint draws this, and ::lh_entity_scroll_apply drags it.
 */
lh_math_rect_t
lh_entity_scroll_thumb(const lh_entity_scroll_t *self, const lh_math_rect_t *bounds);

/**
 * @brief The shortest thumb, in pixels.
 *
 * A content many times the track gives a thumb a few pixels long, which is
 * there to be seen rather than caught, so it stops at this. The thumb is
 * shorter than the track as a whole, and the value is what is left to move.
 */
lh_int_t
lh_entity_scroll_get_thumb_min(const lh_entity_scroll_t *self);

/**
 * @brief Set the shortest thumb, in pixels. A negative one becomes 0, and one
 *        past the track is kept by the bar that draws it.
 */
lh_void
lh_entity_scroll_set_thumb_min(lh_entity_scroll_t *self, lh_int_t min_thumb);

/**
 * @brief The room at both sides of the thumb, across the thickness, in pixels.
 */
lh_int_t
lh_entity_scroll_get_thumb_inset(const lh_entity_scroll_t *self);

/**
 * @brief Set the room at both sides of the thumb, across the thickness, in
 *        pixels. A negative one becomes 0, and a quarter of the thickness is
 *        the most it ever takes.
 */
lh_void
lh_entity_scroll_set_thumb_inset(lh_entity_scroll_t *self, lh_int_t inset);

/**
 * @brief ::LH_ENTITY_SCROLL_SHOW_AUTO, ::LH_ENTITY_SCROLL_SHOW_ALWAYS or
 *        ::LH_ENTITY_SCROLL_SHOW_NEVER.
 */
lh_int_t
lh_entity_scroll_get_mode(const lh_entity_scroll_t *self);

/**
 * @brief Set when the bar is there. A mode that is none of the three is read
 *        as ::LH_ENTITY_SCROLL_SHOW_AUTO. Takes effect at once, through
 *        ::lh_entity_scroll_update_mode.
 */
lh_void
lh_entity_scroll_set_mode(lh_entity_scroll_t *self, lh_int_t mode);

/**
 * @brief True when @p self's ends are apart, which is what the thumb is sized
 *        against and what :LH_ENTITY_SCROLL_SHOW_AUTO asks about.
 *
 * A bar from 0 to 0 has nothing to move, whatever the page says.
 */
lh_bool_t
lh_entity_scroll_is_needed(const lh_entity_scroll_t *self);

/**
 * @brief Put @p self's showing in agreement with its mode and its ends, and
 *        redraw it when that changes.
 *
 * The mode and the ends can both move without either setter running, because a
 * linked ::lh_entity_view_t sets the ends on every draw. So this is called from
 * the draw too, and by ::lh_entity_scroll_set_mode.
 */
lh_void
lh_entity_scroll_update_mode(lh_entity_scroll_t *self);

/**
 * @brief Bind @p self to @p view, so a drag scrolls it.
 *
 * The link runs both ways and this is the half of it that reaches into the
 * view: ::lh_entity_view_set_scrollbar stores the bar and calls this. A
 * ::lh_null view drops this bar's half of the link and leaves the view's.
 */
lh_void
lh_entity_scroll_set_view(lh_entity_scroll_t *self, lh_entity_view_t *view);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_SCROLL_H */
