/**
 * @file sheets.h
 * @brief A tab bar above a stack of pages.
 *
 * The bar is a ::lh_entity_tabs_t. Under it is a ::lh_entity_pages_t.
 * ::lh_entity_sheets_add adds one button and one page, and returns the page
 * so the caller can put that page's contents in it. One page is visible.
 */

#ifndef LH_ENTITY_SHEETS_H
#define LH_ENTITY_SHEETS_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/entity/pages.h>
#include <lh/entity/tabs.h>
#include <lh/ui/font.h>
#include <lh/ui/style.h>

/**
 * @struct lh_entity_sheets
 * @brief A tab bar and the pages it switches.
 */
struct lh_entity_sheets
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_entity_tabs_t *tabs;
    lh_entity_pages_t *pages;
};
typedef struct lh_entity_sheets lh_entity_sheets_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_sheets_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_sheets_class;

/**
 * @brief Font of the titles. Not owned.
 */
lh_void
lh_entity_sheets_set_font(lh_entity_sheets_t *self, const lh_ui_font_t *font);

/**
 * @brief Style of the bar. Not owned.
 */
lh_void
lh_entity_sheets_set_style(lh_entity_sheets_t *self, const lh_ui_style_t *style);

/**
 * @brief Round the bar by @p radius pixels. 0 keeps the square.
 */
lh_void
lh_entity_sheets_set_radius(lh_entity_sheets_t *self, lh_int_t radius);

/**
 * @brief Add a tab titled @p title and return the page that goes with it.
 *
 * @p title is not copied. The page is a plain 2D entity, a child of the
 * pages, the size of the area under the bar.
 */
lh_entity_t *
lh_entity_sheets_add(lh_entity_sheets_t *self, const lh_char_t *title);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_SHEETS_H */
