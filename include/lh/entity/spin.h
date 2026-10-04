/**
 * @file spin.h
 * @brief A value with a minus side and a plus side.
 *
 * The record starts with ::lh_entity_range_t. A press on the left third
 * steps down, a press on the right third steps up, by
 * ::lh_entity_spin_get_step. The number is drawn in the middle with the
 * text color. A change sends ::LH_ENTITY_EVENT_CLICKED.
 */

#ifndef LH_ENTITY_SPIN_H
#define LH_ENTITY_SPIN_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/range.h>
#include <lh/numeric/types.h>
#include <lh/ui/font.h>

/**
 * @struct lh_entity_spin
 * @brief A stepped value.
 */
struct lh_entity_spin
{
    lh_entity_range_t range;
    const lh_ui_font_t *font;
    lh_int_t step;
};
typedef struct lh_entity_spin lh_entity_spin_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_spin_t, derived from ::lh_entity_2d_class.
 *
 * A new spin box steps by 1 and uses ::lh_ui_font_basic.
 */
extern const lh_entity_class_t lh_entity_spin_class;

/**
 * @brief How much one press adds or removes.
 */
lh_int_t
lh_entity_spin_get_step(const lh_entity_spin_t *self);

/**
 * @brief Set how much one press adds or removes. Zero and below become 1.
 */
lh_void
lh_entity_spin_set_step(lh_entity_spin_t *self, lh_int_t step);

/**
 * @brief Font of the number, or ::lh_null.
 */
const lh_ui_font_t *
lh_entity_spin_get_font(const lh_entity_spin_t *self);

/**
 * @brief Point @p self at @p font. Not owned, and not copied.
 */
lh_void
lh_entity_spin_set_font(lh_entity_spin_t *self, const lh_ui_font_t *font);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_SPIN_H */
