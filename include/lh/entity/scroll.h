/**
 * @file scroll.h
 * @brief A scrollbar. A trackbar whose thumb shows how much is visible.
 *
 * The record starts with ::lh_entity_range_t. The page is how much of the
 * whole the thumb stands for, in the same units as the value. The box is
 * the thickness and the length: a taller box is vertical. The track and the
 * thumb are rounded to half their thickness. The track is the style
 * background. The thumb is ::lh_entity_scroll_set_thumb, or the text color
 * when that is null. A drag sends ::LH_ENTITY_EVENT_CLICKED.
 */

#ifndef LH_ENTITY_SCROLL_H
#define LH_ENTITY_SCROLL_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/range.h>
#include <lh/ui/style.h>

/**
 * @struct lh_entity_scroll
 * @brief A thumb on a track.
 */
struct lh_entity_scroll
{
    lh_entity_range_t range;
    const lh_ui_style_t *thumb;
    lh_int_t page;
    lh_bool_t dragging;
};
typedef struct lh_entity_scroll lh_entity_scroll_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_scroll_t, derived from ::lh_entity_2d_class.
 *
 * A new bar runs from 0 to 100 with a page of 10.
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
 * @brief The value record of @p self: ends, start, current value, thickness
 *        and the travel it has. The API is ::lh_entity_range_t's, not this
 *        widget's, and this is how a caller reaches it. The page, what one
 *        screenful covers, is this widget's own and is not part of it.
 *
 * The range is a member by value, so this hands back @p self's own record:
 * writing through it changes the widget, and nothing is allocated.
 */
lh_entity_range_t *
lh_entity_scroll_get_range(lh_entity_scroll_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_SCROLL_H */
