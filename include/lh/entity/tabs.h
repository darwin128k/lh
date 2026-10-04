/**
 * @file tabs.h
 * @brief A row of toggles and the page each one shows.
 *
 * This is a ::lh_entity_group_t of toggles above a ::lh_entity_pages_t.
 * ::lh_entity_tabs_add adds one toggle and one page, and returns the page
 * so the caller can put that page's contents in it. The toggles are
 * exclusive: one page is visible.
 */

#ifndef LH_ENTITY_TABS_H
#define LH_ENTITY_TABS_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/entity/group.h>
#include <lh/entity/pages.h>
#include <lh/ui/font.h>

/**
 * @struct lh_entity_tabs
 * @brief Toggles and the pages they switch.
 */
struct lh_entity_tabs
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_entity_group_t *bar;
    lh_entity_pages_t *pages;
    const lh_ui_font_t *font;
    const lh_ui_style_t *tab_style;
};
typedef struct lh_entity_tabs lh_entity_tabs_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_tabs_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_tabs_class;

/**
 * @brief Font of the toggle titles. Not owned.
 */
lh_void
lh_entity_tabs_set_font(lh_entity_tabs_t *self, const lh_ui_font_t *font);

/**
 * @brief Style of the toggles. Not owned.
 */
lh_void
lh_entity_tabs_set_style(lh_entity_tabs_t *self, const lh_ui_style_t *style);

/**
 * @brief Add a tab titled @p title and return the page that goes with it.
 *
 * @p title is not copied. The page is a plain 2D entity, a child of the
 * pages, the size of the area under the bar.
 */
lh_entity_t *
lh_entity_tabs_add(lh_entity_tabs_t *self, const lh_char_t *title);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_TABS_H */
