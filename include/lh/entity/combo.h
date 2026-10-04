/**
 * @file combo.h
 * @brief A closed list that opens under itself.
 *
 * The rows live on a ::lh_entity_list_t, so the list's mode is the combo's
 * mode: one row, or several. The first row is selected when none is. The
 * closed box draws that row. A press opens or closes the list. While open,
 * the list is the last child of the combo's parent, so it paints above what
 * was added after the combo. The strings are not copied.
 */

#ifndef LH_ENTITY_COMBO_H
#define LH_ENTITY_COMBO_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/list.h>

/**
 * @struct lh_entity_combo
 * @brief A box and the list it opens.
 */
struct lh_entity_combo
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_entity_list_t *list;
    const lh_ui_font_t *font;
};
typedef struct lh_entity_combo lh_entity_combo_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_combo_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_combo_class;

/**
 * @brief The list the box opens. Same lifetime as the combo.
 */
lh_entity_list_t *
lh_entity_combo_get_list(lh_entity_combo_t *self);

/**
 * @brief Put @p text at row @p index of the open list.
 */
lh_void
lh_entity_combo_set_item(lh_entity_combo_t *self, lh_int_t index, const lh_char_t *text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_COMBO_H */
