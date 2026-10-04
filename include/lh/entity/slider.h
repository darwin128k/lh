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

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_SLIDER_H */
