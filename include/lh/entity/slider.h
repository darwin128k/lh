/**
 * @file slider.h
 * @brief A trackbar: the progress bar, plus a drag that sets the value.
 *
 * The record starts with ::lh_entity_range_t, so the range functions take
 * it. The track ends are rounded to half the thickness. The thumb is a
 * circle as tall as the short side, and a drag along the long side moves
 * the value. Each change sends ::LH_ENTITY_EVENT_CLICKED.
 */

#ifndef LH_ENTITY_SLIDER_H
#define LH_ENTITY_SLIDER_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/range.h>
#include <lh/ui/style.h>

struct lh_entity_circle;

/**
 * @struct lh_entity_slider
 * @brief A progress bar that follows the pointer.
 */
struct lh_entity_slider
{
    lh_entity_range_t range;
    lh_bool_t dragging;
    struct lh_entity_circle *thumb;
    lh_ui_style_t thumb_style;
};
typedef struct lh_entity_slider lh_entity_slider_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_slider_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_slider_class;

/**
 * @brief The value record of @p self: ends, start, current value, thickness
 *        and the travel it has. The API is ::lh_entity_range_t's, not this
 *        widget's, and this is how a caller reaches it.
 *
 * The range is a member by value, so this hands back @p self's own record:
 * writing through it changes the widget, and nothing is allocated.
 */
lh_entity_range_t *
lh_entity_slider_get_range(lh_entity_slider_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_SLIDER_H */
