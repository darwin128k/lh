#include <lh/entity/tabs.h>
#include <lh/assert.h>
#include <lh/entity/label.h>
#include <lh/entity/option.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_tabs_on_pick(lh_entity_event_t *event, lh_ptr user)
{
    lh_entity_tabs_t *const tabs = lh_ptr_rcast(lh_entity_tabs_t, user);
    lh_entity_t *const target = lh_entity_event_get_target(event);
    lh_int_t index = 0;
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_CLICKED || lh_ptr_is_null(tabs) ||
        lh_ptr_is_null(tabs->bar))
    {
        return;
    }
    lh_entity_foreach_child(child, (lh_entity_t *)tabs->bar)
    {
        if (child == target)
        {
            lh_entity_pages_set_index(tabs->pages, index);
            return;
        }
        index += 1;
    }
}

lh_void
lh_entity_tabs_construct(lh_entity_t *self)
{
    lh_entity_tabs_t *const tabs = lh_ptr_rcast(lh_entity_tabs_t, self);
    tabs->font = lh_addr_of(lh_ui_font_basic);
    tabs->bar = (lh_entity_group_t *)lh_entity_create(&lh_entity_group_class, self);
    tabs->pages = (lh_entity_pages_t *)lh_entity_create(&lh_entity_pages_class, self);
    lh_entity_group_set_mode(tabs->bar, LH_ENTITY_GROUP_ONE);
    lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, tabs->bar), lh_math_vec2_make(0.0f, 0.0f));
    lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, tabs->pages), lh_math_vec2_make(0.0f, 28.0f));
}

const lh_entity_class_t lh_entity_tabs_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_tabs_t),
                                lh_entity_tabs_construct, lh_null, lh_null);

lh_void
lh_entity_tabs_set_font(lh_entity_tabs_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
}

lh_void
lh_entity_tabs_set_style(lh_entity_tabs_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->tab_style = style;
}

lh_entity_t *
lh_entity_tabs_add(lh_entity_tabs_t *self, const lh_char_t *title)
{
    lh_entity_t *toggle;
    lh_entity_t *page;
    lh_entity_label_t *label;
    lh_int_t count = 0;
    lh_math_vec2_t size;
    lh_assert_runtime_ref(self);
    lh_entity_foreach_child(child, (lh_entity_t *)self->bar)
    {
        (void)child;
        count += 1;
    }
    toggle = lh_entity_create(&lh_entity_toggle_class, (lh_entity_t *)self->bar);
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, toggle), self->tab_style);
    lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, toggle),
                              lh_math_vec2_make((lh_float_t)(count * 76), 0.0f));
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, toggle), lh_math_vec2_make(72.0f, 26.0f));
    label = (lh_entity_label_t *)lh_entity_create(&lh_entity_label_class, toggle);
    lh_entity_add_flags((lh_entity_t *)label, lh_entity_flags_event_bubble);
    lh_entity_add_flags((lh_entity_t *)label, lh_entity_flags_own_background);
    lh_entity_label_set_font(label, self->font);
    lh_entity_label_set_text(label, title);
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, label), self->tab_style);
    lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, label), lh_math_vec2_make(6.0f, 4.0f));
    lh_entity_add_handler(toggle, lh_entity_tabs_on_pick, self);
    page = lh_entity_create(&lh_entity_2d_class, (lh_entity_t *)self->pages);
    size = lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self->bar),
                          lh_math_vec2_make(lh_math_vec2_get_x(lh_addr_of(size)), 28.0f));
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self->pages),
                          lh_math_vec2_make(lh_math_vec2_get_x(lh_addr_of(size)),
                                            lh_math_vec2_get_y(lh_addr_of(size)) - 28.0f));
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, page),
                          lh_math_vec2_make(lh_math_vec2_get_x(lh_addr_of(size)),
                                            lh_math_vec2_get_y(lh_addr_of(size)) - 28.0f));
    if (count == 0)
    {
        lh_entity_option_set_on(lh_ptr_rcast(lh_entity_option_t, toggle), lh_bool_true);
    }
    lh_entity_pages_set_index(self->pages, lh_entity_pages_get_index(self->pages));
    return page;
}
