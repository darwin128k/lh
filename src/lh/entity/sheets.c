#include <lh/entity/sheets.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_sheets_on_pick(lh_entity_event_t *event, lh_ptr user)
{
    lh_entity_sheets_t *const sheets = lh_ptr_rcast(lh_entity_sheets_t, user);
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_CLICKED || lh_ptr_is_null(sheets) ||
        lh_ptr_is_null(sheets->tabs) || lh_ptr_is_null(sheets->pages))
    {
        return;
    }
    lh_entity_pages_set_index(sheets->pages, lh_entity_tabs_get_index(sheets->tabs));
}

lh_void
lh_entity_sheets_construct(lh_entity_t *self)
{
    lh_entity_sheets_t *const sheets = lh_ptr_rcast(lh_entity_sheets_t, self);
    sheets->tabs = lh_ptr_rcast(lh_entity_tabs_t, lh_entity_create(&lh_entity_tabs_class, self));
    sheets->pages = lh_ptr_rcast(lh_entity_pages_t, lh_entity_create(&lh_entity_pages_class, self));
    lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, sheets->tabs), lh_math_vec2_make(0.0f, 0.0f));
    lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, sheets->pages),
                              lh_math_vec2_make(0.0f, 28.0f));
    lh_entity_add_handler(lh_ptr_rcast(lh_entity_t, sheets->tabs), lh_entity_sheets_on_pick, self);
}

const lh_entity_class_t lh_entity_sheets_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_sheets_t),
                                lh_entity_sheets_construct, lh_null, lh_null);

lh_void
lh_entity_sheets_set_font(lh_entity_sheets_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    lh_entity_tabs_set_font(self->tabs, font);
}

lh_void
lh_entity_sheets_set_style(lh_entity_sheets_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    lh_entity_tabs_set_style(self->tabs, style);
}

lh_void
lh_entity_sheets_set_radius(lh_entity_sheets_t *self, lh_int_t radius)
{
    lh_assert_runtime_ref(self);
    lh_entity_tabs_set_radius(self->tabs, radius);
}

lh_entity_t *
lh_entity_sheets_add(lh_entity_sheets_t *self, const lh_char_t *title)
{
    lh_entity_t *page;
    lh_math_vec2_t size;
    lh_assert_runtime_ref(self);
    lh_entity_tabs_add(self->tabs, title);
    page = lh_entity_create(lh_addr_of(lh_entity_2d_class), lh_ptr_rcast(lh_entity_t, self->pages));
    size = lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self->tabs),
                          lh_math_vec2_make(lh_math_vec2_get_x(lh_addr_of(size)), 28.0f));
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self->pages),
                          lh_math_vec2_make(lh_math_vec2_get_x(lh_addr_of(size)),
                                            lh_math_vec2_get_y(lh_addr_of(size)) - 28.0f));
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, page),
                          lh_math_vec2_make(lh_math_vec2_get_x(lh_addr_of(size)),
                                            lh_math_vec2_get_y(lh_addr_of(size)) - 28.0f));
    lh_entity_pages_set_index(self->pages, lh_entity_pages_get_index(self->pages));
    return page;
}
