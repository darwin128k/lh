/**
 * @file list.h
 * @brief A column of buttons. One row, or several, can be selected.
 *
 * Each row is a ::lh_entity_button_t, and its words are a label. The button
 * centers that label. The box stays square until
 * ::lh_entity_list_set_radius. The strings are not copied and not owned. A marked row
 * uses the list's mark style. ::LH_ENTITY_GROUP_ONE keeps a single row.
 * ::LH_ENTITY_GROUP_MANY keeps a mark on each. A press sends
 * ::LH_ENTITY_EVENT_CLICKED. At most ::LH_ENTITY_LIST_LIMIT rows are kept.
 */

#ifndef LH_ENTITY_LIST_H
#define LH_ENTITY_LIST_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/flex.h>
#include <lh/numeric/types.h>
#include <lh/ui/font.h>
#include <lh/ui/style.h>

struct lh_entity_button;

/**
 * @def LH_ENTITY_LIST_LIMIT
 * @brief How many rows one list can hold.
 */
#define LH_ENTITY_LIST_LIMIT 12

/**
 * @struct lh_entity_list
 * @brief Buttons and which of them are on.
 */
struct lh_entity_list
{
    lh_entity_flex_t flex;
    const lh_char_t *items[LH_ENTITY_LIST_LIMIT];
    struct lh_entity_button *rows[LH_ENTITY_LIST_LIMIT];
    lh_byte_t marks[LH_ENTITY_LIST_LIMIT];
    lh_ui_style_t marked;
    const lh_ui_font_t *font;
    lh_int_t count;
    lh_int_t mode;
    lh_int_t origin;
    lh_int_t row;
    lh_int_t radius;
};
typedef struct lh_entity_list lh_entity_list_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_list_t, derived from ::lh_entity_flex_class.
 *
 * A new list is ::LH_ENTITY_GROUP_ONE, draws with ::lh_ui_font_get_default, and
 * gives each button 22 pixels of height.
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
 * @brief Select row @p index. ::LH_ENTITY_GROUP_ONE turns the other rows off.
 *        ::LH_ENTITY_GROUP_MANY toggles that row.
 */
lh_void
lh_entity_list_set_on(lh_entity_list_t *self, lh_int_t index);

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

/**
 * @brief Corner radius of @p self, in pixels. 0 is a square.
 */
lh_int_t
lh_entity_list_get_radius(const lh_entity_list_t *self);

/**
 * @brief Round the box of @p self by @p radius pixels. 0 keeps the square.
 */
lh_void
lh_entity_list_set_radius(lh_entity_list_t *self, lh_int_t radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_LIST_H */
