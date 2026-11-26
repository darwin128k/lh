/**
 * @file decl.c
 * @brief Implementation of `lh/gl/decl.h`.
 */

#include <lh/gl/decl.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

lh_gl_decl_kind_t
lh_gl_decl_get_kind(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_gl_decl_set_kind(lh_gl_decl_t *self, lh_gl_decl_kind_t value)
{
    lh_assert_runtime_ref(self);
    self->kind = value;
}

lh_gl_scalar_t
lh_gl_decl_get_x(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_void
lh_gl_decl_set_x(lh_gl_decl_t *self, lh_gl_scalar_t value)
{
    lh_assert_runtime_ref(self);
    self->x = value;
}

lh_gl_scalar_t
lh_gl_decl_get_y(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_void
lh_gl_decl_set_y(lh_gl_decl_t *self, lh_gl_scalar_t value)
{
    lh_assert_runtime_ref(self);
    self->y = value;
}

lh_gl_scalar_t
lh_gl_decl_get_w(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->w;
}

lh_void
lh_gl_decl_set_w(lh_gl_decl_t *self, lh_gl_scalar_t value)
{
    lh_assert_runtime_ref(self);
    self->w = value;
}

lh_gl_scalar_t
lh_gl_decl_get_h(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->h;
}

lh_void
lh_gl_decl_set_h(lh_gl_decl_t *self, lh_gl_scalar_t value)
{
    lh_assert_runtime_ref(self);
    self->h = value;
}

lh_s32_t
lh_gl_decl_get_parent(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->parent;
}

lh_void
lh_gl_decl_set_parent(lh_gl_decl_t *self, lh_s32_t value)
{
    lh_assert_runtime_ref(self);
    self->parent = value;
}

const lh_gl_style_t *
lh_gl_decl_get_style(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->style;
}

lh_void
lh_gl_decl_set_style(lh_gl_decl_t *self, const lh_gl_style_t *value)
{
    lh_assert_runtime_ref(self);
    self->style = value;
}

const lh_gl_place_t *
lh_gl_decl_get_place(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->place;
}

lh_void
lh_gl_decl_set_place(lh_gl_decl_t *self, const lh_gl_place_t *value)
{
    lh_assert_runtime_ref(self);
    self->place = value;
}

const lh_gl_layout_t *
lh_gl_decl_get_layout(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->layout;
}

lh_void
lh_gl_decl_set_layout(lh_gl_decl_t *self, const lh_gl_layout_t *value)
{
    lh_assert_runtime_ref(self);
    self->layout = value;
}

lh_bool_t
lh_gl_decl_is_hidden(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hidden;
}

lh_void
lh_gl_decl_set_hidden(lh_gl_decl_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->hidden = value;
}

const lh_char_t *
lh_gl_decl_get_label_text(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.label.text;
}

lh_void
lh_gl_decl_set_label_text(lh_gl_decl_t *self, const lh_char_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.label.text = value;
}

const lh_gl_style_t *
lh_gl_decl_get_button_hot_style(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.button.hot_style;
}

lh_void
lh_gl_decl_set_button_hot_style(lh_gl_decl_t *self, const lh_gl_style_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.button.hot_style = value;
}

lh_gl_button_on_click_cb
lh_gl_decl_get_button_on_click(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.button.on_click;
}

lh_void
lh_gl_decl_set_button_on_click(lh_gl_decl_t *self, lh_gl_button_on_click_cb value)
{
    lh_assert_runtime_ref(self);
    self->as.button.on_click = value;
}

lh_ptr
lh_gl_decl_get_button_click_context(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.button.click_context;
}

lh_void
lh_gl_decl_set_button_click_context(lh_gl_decl_t *self, lh_ptr value)
{
    lh_assert_runtime_ref(self);
    self->as.button.click_context = value;
}

const lh_gl_style_t *
lh_gl_decl_get_toggle_off_style(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.toggle.off_style;
}

lh_void
lh_gl_decl_set_toggle_off_style(lh_gl_decl_t *self, const lh_gl_style_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.toggle.off_style = value;
}

const lh_gl_style_t *
lh_gl_decl_get_toggle_off_hot_style(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.toggle.off_hot_style;
}

lh_void
lh_gl_decl_set_toggle_off_hot_style(lh_gl_decl_t *self, const lh_gl_style_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.toggle.off_hot_style = value;
}

const lh_gl_style_t *
lh_gl_decl_get_toggle_on_style(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.toggle.on_style;
}

lh_void
lh_gl_decl_set_toggle_on_style(lh_gl_decl_t *self, const lh_gl_style_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.toggle.on_style = value;
}

const lh_gl_style_t *
lh_gl_decl_get_toggle_on_hot_style(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.toggle.on_hot_style;
}

lh_void
lh_gl_decl_set_toggle_on_hot_style(lh_gl_decl_t *self, const lh_gl_style_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.toggle.on_hot_style = value;
}

lh_bool_t
lh_gl_decl_is_toggle_checked(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.toggle.checked;
}

lh_void
lh_gl_decl_set_toggle_checked(lh_gl_decl_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->as.toggle.checked = value;
}

const lh_gl_mask_t *
lh_gl_decl_get_image_mask(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.image.mask;
}

lh_void
lh_gl_decl_set_image_mask(lh_gl_decl_t *self, const lh_gl_mask_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.image.mask = value;
}

const lh_gl_color_t *
lh_gl_decl_get_image_tint(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.image.tint;
}

lh_void
lh_gl_decl_set_image_tint(lh_gl_decl_t *self, const lh_gl_color_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.image.tint = value;
}

lh_s32_t
lh_gl_decl_get_scrollbar_box(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.scrollbar.box;
}

lh_void
lh_gl_decl_set_scrollbar_box(lh_gl_decl_t *self, lh_s32_t value)
{
    lh_assert_runtime_ref(self);
    self->as.scrollbar.box = value;
}

lh_gl_axis_t
lh_gl_decl_get_scrollbar_axis(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.scrollbar.axis;
}

lh_void
lh_gl_decl_set_scrollbar_axis(lh_gl_decl_t *self, lh_gl_axis_t value)
{
    lh_assert_runtime_ref(self);
    self->as.scrollbar.axis = value;
}

lh_gl_scrollbar_mode_t
lh_gl_decl_get_scrollbar_mode(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.scrollbar.mode;
}

lh_void
lh_gl_decl_set_scrollbar_mode(lh_gl_decl_t *self, lh_gl_scrollbar_mode_t value)
{
    lh_assert_runtime_ref(self);
    self->as.scrollbar.mode = value;
}

lh_char_t *
lh_gl_decl_get_input_buffer(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.input.buffer;
}

lh_void
lh_gl_decl_set_input_buffer(lh_gl_decl_t *self, lh_char_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.input.buffer = value;
}

lh_u16_t
lh_gl_decl_get_input_capacity(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.input.capacity;
}

lh_void
lh_gl_decl_set_input_capacity(lh_gl_decl_t *self, lh_u16_t value)
{
    lh_assert_runtime_ref(self);
    self->as.input.capacity = value;
}

const lh_char_t *
lh_gl_decl_get_input_text(const lh_gl_decl_t *self)
{
    lh_assert_runtime_ref(self);
    return self->as.input.text;
}

lh_void
lh_gl_decl_set_input_text(lh_gl_decl_t *self, const lh_char_t *value)
{
    lh_assert_runtime_ref(self);
    self->as.input.text = value;
}

lh_u32_t
lh_gl_decl_size_of(lh_gl_decl_kind_t kind)
{
    switch (kind)
    {
    case lh_gl_decl_kind_entity:
        return (lh_u32_t)sizeof(lh_gl_entity_t);
    case lh_gl_decl_kind_container:
        return (lh_u32_t)sizeof(lh_gl_container_t);
    case lh_gl_decl_kind_label:
        return (lh_u32_t)sizeof(lh_gl_label_t);
    case lh_gl_decl_kind_button:
        return (lh_u32_t)sizeof(lh_gl_button_t);
    case lh_gl_decl_kind_toggle:
        return (lh_u32_t)sizeof(lh_gl_toggle_t);
    case lh_gl_decl_kind_image:
        return (lh_u32_t)sizeof(lh_gl_image_t);
    case lh_gl_decl_kind_scrollbar:
        return (lh_u32_t)sizeof(lh_gl_scrollbar_t);
    case lh_gl_decl_kind_input:
        return (lh_u32_t)sizeof(lh_gl_input_t);
    default:
        return 0;
    }
}

lh_void
lh_gl_decl_fault(lh_gl_decl_fault_t *fault, lh_gl_decl_error_t code, lh_u32_t index)
{
    if (lh_null_ne(fault))
    {
        lh_gl_decl_fault_set_code(fault, code);
        lh_gl_decl_fault_set_index(fault, index);
    }
}

lh_void
lh_gl_decl_get_rect(const lh_gl_decl_t *self, lh_gl_rect_t *out)
{
    lh_gl_rect_init(out, lh_gl_decl_get_x(self), lh_gl_decl_get_y(self), lh_gl_decl_get_w(self),
                    lh_gl_decl_get_h(self));
}

lh_void
lh_gl_decl_apply_row(lh_gl_entity_t *entity, const lh_gl_decl_t *row)
{
    if (lh_null_ne(lh_gl_decl_get_style(row)) &&
        lh_gl_decl_get_kind(row) != lh_gl_decl_kind_button &&
        lh_gl_decl_get_kind(row) != lh_gl_decl_kind_toggle)
    {
        /* A button and a toggle aim the entity at their style themselves, so a press
           composes on top of whichever of the two is showing. */
        lh_gl_entity_set_style_v(entity, lh_gl_decl_get_style(row));
    }
    if (lh_null_ne(lh_gl_decl_get_place(row)))
    {
        lh_gl_entity_set_place_v(entity, lh_gl_decl_get_place(row));
    }
    /* A flow belongs to a container, and the question "is this one?" is the
       container's own answer: a button, a toggle and a caption are containers, and a
       plain entity or a picture is not. One call, no table of kinds. */
    if (lh_null_ne(lh_gl_decl_get_layout(row)))
    {
        lh_gl_container_t *box = lh_gl_entity_as_container(entity);

        if (lh_null_ne(box))
        {
            lh_gl_container_set_layout(box, lh_gl_decl_get_layout(row));
        }
    }
    if (lh_gl_decl_is_hidden(row))
    {
        lh_gl_entity_set_hidden_v(entity, lh_bool_true);
    }
}

lh_gl_entity_t *
lh_gl_decl_link(const lh_gl_decl_t *decls, lh_u32_t count, lh_gl_entity_t **nodes)
{
    lh_gl_entity_t *tree = lh_null;

    for (lh_u32_t index = 0; index < count; ++index)
    {
        const lh_gl_decl_t *row = lh_addr_of(decls[index]);

        if (lh_gl_decl_get_parent(row) == LH_GL_DECL_ROOT)
        {
            tree = nodes[index];
        }
        else
        {
            lh_gl_entity_add_child(nodes[lh_gl_decl_get_parent(row)], nodes[index]);
        }
    }
    return tree;
}

lh_gl_decl_error_t
lh_gl_decl_check(const lh_gl_decl_t *decls, lh_u32_t index)
{
    const lh_gl_decl_t *row = lh_addr_of(decls[index]);

    if (lh_gl_decl_size_of(lh_gl_decl_get_kind(row)) == 0)
    {
        return lh_gl_decl_error_kind;
    }
    /* A child cannot exist before the thing that holds it, so a parent is a lower
       index or nothing at all. */
    if (lh_gl_decl_get_parent(row) != LH_GL_DECL_ROOT &&
        (lh_gl_decl_get_parent(row) < 0 ||
         lh_cast_static(lh_u32_t, lh_gl_decl_get_parent(row)) >= index))
    {
        return lh_gl_decl_error_parent;
    }
    /* A scrollbar names the container it scrolls. Whether that row really is one is
       asked once the nodes are placed, because the answer is a property of the
       component the row builds and not of the kind it wrote. */
    if (lh_gl_decl_get_kind(row) == lh_gl_decl_kind_scrollbar &&
        (lh_gl_decl_get_scrollbar_box(row) < 0 ||
         lh_cast_static(lh_u32_t, lh_gl_decl_get_scrollbar_box(row)) >= index))
    {
        return lh_gl_decl_error_box;
    }
    return lh_gl_decl_error_none;
}

lh_gl_entity_t *
lh_gl_decl_place_entity(const lh_gl_decl_t *row, lh_byte_ptr slot)
{
    lh_gl_rect_t rect;

    lh_gl_decl_get_rect(row, lh_addr_of(rect));
    lh_gl_entity_t *entity = lh_ptr_rcast(lh_gl_entity_t, slot);

    lh_gl_entity_init(entity, rect);
    return entity;
}

lh_gl_entity_t *
lh_gl_decl_place_container(const lh_gl_decl_t *row, lh_byte_ptr slot)
{
    lh_gl_rect_t rect;

    lh_gl_decl_get_rect(row, lh_addr_of(rect));
    lh_gl_container_t *box = lh_ptr_rcast(lh_gl_container_t, slot);

    lh_gl_container_init(box, rect);
    return lh_gl_container_as_entity(box);
}

lh_gl_entity_t *
lh_gl_decl_place_label(const lh_gl_decl_t *row, lh_byte_ptr slot)
{
    lh_gl_rect_t rect;

    lh_gl_decl_get_rect(row, lh_addr_of(rect));
    lh_gl_label_t *label = lh_ptr_rcast(lh_gl_label_t, slot);

    lh_gl_label_init(label, rect, lh_gl_decl_get_label_text(row));
    return lh_gl_label_as_entity(label);
}

lh_gl_entity_t *
lh_gl_decl_place_button(const lh_gl_decl_t *row, lh_byte_ptr slot)
{
    lh_gl_rect_t rect;

    lh_gl_decl_get_rect(row, lh_addr_of(rect));
    lh_gl_button_t *button = lh_ptr_rcast(lh_gl_button_t, slot);

    lh_gl_button_init(button, rect);
    lh_gl_button_set_style(button, lh_gl_decl_get_style(row));
    lh_gl_button_set_hot_style_v(button, lh_gl_decl_get_button_hot_style(row));
    lh_gl_button_set_on_click(button, lh_gl_decl_get_button_on_click(row),
                              lh_gl_decl_get_button_click_context(row));
    return lh_gl_button_as_entity(button);
}

lh_gl_entity_t *
lh_gl_decl_place_toggle(const lh_gl_decl_t *row, lh_byte_ptr slot)
{
    lh_gl_rect_t rect;

    lh_gl_decl_get_rect(row, lh_addr_of(rect));
    lh_gl_toggle_t *toggle = lh_ptr_rcast(lh_gl_toggle_t, slot);

    lh_gl_toggle_init(toggle, rect);
    lh_gl_toggle_set_off_styles(toggle, lh_gl_decl_get_toggle_off_style(row),
                                lh_gl_decl_get_toggle_off_hot_style(row));
    lh_gl_toggle_set_on_styles(toggle, lh_gl_decl_get_toggle_on_style(row),
                               lh_gl_decl_get_toggle_on_hot_style(row));
    lh_gl_toggle_set_checked_v(toggle, lh_gl_decl_is_toggle_checked(row));
    return lh_gl_toggle_as_entity(toggle);
}

lh_gl_entity_t *
lh_gl_decl_place_image(const lh_gl_decl_t *row, lh_byte_ptr slot)
{
    lh_gl_rect_t rect;

    lh_gl_decl_get_rect(row, lh_addr_of(rect));
    lh_gl_image_t *image = lh_ptr_rcast(lh_gl_image_t, slot);

    lh_gl_image_init(image, rect, lh_gl_decl_get_image_mask(row));
    if (lh_null_ne(lh_gl_decl_get_image_tint(row)))
    {
        lh_gl_image_set_tint_v(image, lh_gl_decl_get_image_tint(row));
    }
    return lh_gl_image_as_entity(image);
}

lh_gl_entity_t *
lh_gl_decl_place_scrollbar(const lh_gl_decl_t *row, lh_byte_ptr slot, lh_gl_entity_t **nodes)
{
    lh_gl_rect_t rect;

    lh_gl_decl_get_rect(row, lh_addr_of(rect));
    lh_gl_scrollbar_t *bar = lh_ptr_rcast(lh_gl_scrollbar_t, slot);
    lh_gl_container_t *box = lh_gl_entity_as_container(nodes[lh_gl_decl_get_scrollbar_box(row)]);

    lh_gl_scrollbar_init(bar, rect, lh_gl_decl_get_scrollbar_axis(row), box);
    /* An undeclared mode is the one the component starts with, so a row that
       says nothing gets what a hand-written bar gets. */
    if (lh_gl_decl_get_scrollbar_mode(row) != lh_cast_static(lh_gl_scrollbar_mode_t, 0))
    {
        lh_gl_scrollbar_set_mode(bar, lh_gl_decl_get_scrollbar_mode(row));
    }
    return lh_gl_scrollbar_as_entity(bar);
}

lh_gl_entity_t *
lh_gl_decl_place_input(const lh_gl_decl_t *row, lh_byte_ptr slot)
{
    lh_gl_rect_t rect;

    lh_gl_decl_get_rect(row, lh_addr_of(rect));
    lh_gl_input_t *input = lh_ptr_rcast(lh_gl_input_t, slot);

    lh_gl_input_init(input, rect, lh_gl_decl_get_input_buffer(row),
                     lh_gl_decl_get_input_capacity(row));
    /* Init empties the buffer -- it is the field's from then on -- so what the
       row starts with is put in after, through the field's own door. */
    if (lh_null_ne(lh_gl_decl_get_input_text(row)))
    {
        lh_gl_input_set_text(input, lh_gl_decl_get_input_text(row));
    }
    return lh_gl_input_as_entity(input);
}

lh_bool_t
lh_gl_decl_try_measure(const lh_gl_decl_t *decls, lh_u32_t count, lh_u32_t *bytes,
                       lh_gl_decl_fault_t *fault)
{
    lh_u32_t roots = 0;
    lh_u32_t needed = 0;

    lh_assert_runtime_ref(bytes);
    for (lh_u32_t index = 0; index < count; ++index)
    {
        lh_gl_decl_error_t code = lh_gl_decl_check(decls, index);

        if (code != lh_gl_decl_error_none)
        {
            lh_gl_decl_fault(fault, code, index);
            return lh_bool_false;
        }
        /* Every sizeof is a multiple of the alignment of what it holds, so packing
           rows end to end keeps every widget aligned. */
        needed += lh_gl_decl_size_of(lh_gl_decl_get_kind(lh_addr_of(decls[index])));
        if (lh_gl_decl_get_parent(lh_addr_of(decls[index])) == LH_GL_DECL_ROOT)
        {
            ++roots;
        }
    }
    /* One tree, one root: a table with two is not a mistake a build can guess at. */
    if (roots != 1)
    {
        lh_gl_decl_fault(fault, lh_gl_decl_error_roots, count);
        return lh_bool_false;
    }
    lh_ptr_deref(bytes) = needed;
    return lh_bool_true;
}

lh_gl_entity_t *
lh_gl_decl_place_row(const lh_gl_decl_t *row, lh_byte_ptr slot, lh_gl_entity_t **nodes)
{
    switch (lh_gl_decl_get_kind(row))
    {
    case lh_gl_decl_kind_entity:
        return lh_gl_decl_place_entity(row, slot);
    case lh_gl_decl_kind_container:
        return lh_gl_decl_place_container(row, slot);
    case lh_gl_decl_kind_label:
        return lh_gl_decl_place_label(row, slot);
    case lh_gl_decl_kind_button:
        return lh_gl_decl_place_button(row, slot);
    case lh_gl_decl_kind_toggle:
        return lh_gl_decl_place_toggle(row, slot);
    case lh_gl_decl_kind_image:
        return lh_gl_decl_place_image(row, slot);
    case lh_gl_decl_kind_scrollbar:
        return lh_gl_decl_place_scrollbar(row, slot, nodes);
    case lh_gl_decl_kind_input:
        return lh_gl_decl_place_input(row, slot);
    default:
        return lh_null;
    }
}

lh_bool_t
lh_gl_decl_try_place(const lh_gl_decl_t *decls, lh_u32_t count, lh_gl_entity_t **nodes,
                     lh_byte_ptr storage, lh_gl_decl_fault_t *fault)
{
    lh_u32_t used = 0;

    for (lh_u32_t index = 0; index < count; ++index)
    {
        const lh_gl_decl_t *row = lh_addr_of(decls[index]);

        /* A scrollbar is built around the container it scrolls, and
           ::lh_gl_scrollbar_init will not take a null one. The row it names is lower,
           so its node already exists: asked here, before the widget is touched at all,
           and not after something has been written into it. */
        if (lh_gl_decl_get_kind(row) == lh_gl_decl_kind_scrollbar &&
            lh_null_eq(lh_gl_entity_as_container(nodes[lh_gl_decl_get_scrollbar_box(row)])))
        {
            lh_gl_decl_fault(fault, lh_gl_decl_error_box, index);
            return lh_bool_false;
        }
        lh_gl_entity_t *entity = lh_gl_decl_place_row(row, storage + used, nodes);

        lh_gl_decl_apply_row(entity, row);
        nodes[index] = entity;
        used += lh_gl_decl_size_of(lh_gl_decl_get_kind(row));
    }
    return lh_bool_true;
}

lh_bool_t
lh_gl_decl_build(const lh_gl_decl_t *decls, lh_u32_t count, lh_gl_entity_t **nodes,
                 lh_byte_ptr storage, lh_u32_t bytes, lh_gl_entity_t **root,
                 lh_gl_decl_fault_t *fault)
{
    lh_u32_t needed = 0;

    lh_gl_decl_fault(fault, lh_gl_decl_error_none, count);
    lh_return_if(lh_null_eq(decls) || lh_null_eq(nodes) || lh_null_eq(storage) ||
                     lh_null_eq(root) || count == 0,
                 lh_bool_false);
    lh_return_if(!lh_gl_decl_try_measure(decls, count, lh_addr_of(needed), fault), lh_bool_false);
    /* Measured before anything is written, which is the whole point of asking: the
       alternative is a pool that is too small and a row that lands outside it. */
    if (bytes < needed)
    {
        lh_gl_decl_fault(fault, lh_gl_decl_error_storage, count);
        return lh_bool_false;
    }
    /* Place every node first, then wire the tree: nothing is connected until all of
       it is true, so a refused build is not a tree that looks finished. */
    lh_return_if(!lh_gl_decl_try_place(decls, count, nodes, storage, fault), lh_bool_false);
    lh_ptr_deref(root) = lh_gl_decl_link(decls, count, nodes);
    return lh_bool_true;
}
