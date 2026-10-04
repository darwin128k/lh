/**
 * @file list.h
 * @brief A column of rows. One row, or several, can be selected.
 *
 * The strings are not copied and not owned, the same way a label keeps its
 * text. ::LH_ENTITY_GROUP_ONE keeps a single row. ::LH_ENTITY_GROUP_MANY
 * keeps a mark on each. A press sends ::LH_ENTITY_EVENT_CLICKED. At most
 * ::LH_ENTITY_LIST_LIMIT rows are kept.
 */

#ifndef LH_ENTITY_LIST_H
#define LH_ENTITY_LIST_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>
#include <lh/ui/font.h>

/**
 * @def LH_ENTITY_LIST_LIMIT
 * @brief How many rows one list can hold.
 */
#define LH_ENTITY_LIST_LIMIT 12

/**
 * @struct lh_entity_list
 * @brief Rows of text and which of them are on.
 */
struct lh_entity_list
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    const lh_char_t *items[LH_ENTITY_LIST_LIMIT];
    lh_byte_t marks[LH_ENTITY_LIST_LIMIT];
    const lh_ui_font_t *font;
    lh_int_t count;
    lh_int_t mode;
    lh_int_t origin;
    lh_int_t row;
};
typedef struct lh_entity_list lh_entity_list_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_list_t, derived from ::lh_entity_2d_class.
 *
 * A new list is ::LH_ENTITY_GROUP_ONE, draws with ::lh_ui_font_basic, and
 * gives each row 22 pixels.
 */
extern const lh_entity_class_t lh_entity_list_class;

/**
 * @brief How many rows @p self holds.
 */
lh_int_t
lh_entity_list_get_count(const lh_entity_list_t *self);

/**
 * @brief Replace row @p index. ::lh_null removes it and the rows after it
 *        shift down. Past ::LH_ENTITY_LIST_LIMIT, nothing is stored.
 */
lh_void
lh_entity_list_set_item(lh_entity_list_t *self, lh_int_t index, const lh_char_t *text);

/**
 * @brief The text of row @p index, or ::lh_null.
 */
const lh_char_t *
lh_entity_list_get_item(const lh_entity_list_t *self, lh_int_t index);

/**
 * @brief True when row @p index is selected.
 */
lh_bool_t
lh_entity_list_is_on(const lh_entity_list_t *self, lh_int_t index);

/**
 * @brief ::LH_ENTITY_GROUP_ONE or ::LH_ENTITY_GROUP_MANY.
 */
lh_void
lh_entity_list_set_mode(lh_entity_list_t *self, lh_int_t mode);

/**
 * @brief First row drawn at the top of the box.
 */
lh_void
lh_entity_list_set_origin(lh_entity_list_t *self, lh_int_t origin);

/**
 * @brief Point @p self at @p font. Not owned.
 */
lh_void
lh_entity_list_set_font(lh_entity_list_t *self, const lh_ui_font_t *font);

/**
 * @brief Height of one row, in pixels.
 */
lh_int_t
lh_entity_list_get_row(const lh_entity_list_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_LIST_H */
