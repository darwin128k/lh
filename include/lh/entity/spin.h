/**
 * @file spin.h
 * @brief A value with a minus side and a plus side.
 *
 * The record starts with ::lh_entity_range_t. The sides are buttons and the
 * number between them is a label, and those three sit in a flex container
 * that fills the bar. The box itself is one rounded bar: the
 * radius is half the short side, the same cap a track uses. A press steps
 * by ::lh_entity_spin_get_step, and holding a side keeps stepping, the same
 * repeat a button uses. A change sends ::LH_ENTITY_EVENT_CLICKED.
 */

#ifndef LH_ENTITY_SPIN_H
#define LH_ENTITY_SPIN_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/button.h>
#include <lh/entity/flex.h>
#include <lh/entity/label.h>
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
    lh_entity_flex_t *bar;
    const lh_ui_font_t *font;
    lh_int_t step;
    lh_entity_button_t *minus;
    lh_entity_button_t *plus;
    lh_entity_label_t *value;
    lh_char_t digits[16];
};
typedef struct lh_entity_spin lh_entity_spin_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_spin_t, derived from ::lh_entity_2d_class.
 *
 * A new spin box steps by 1 and uses ::lh_ui_font_get_default.
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
