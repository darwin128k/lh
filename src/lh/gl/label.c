/**
 * @file label.c
 * @brief Implementation of `lh/gl/label.h`: the label itself: its fields (level 1), the parts they
 * live in, init and its class.
 */

#include <lh/gl/label.h>

#include <lh/core/assert.h>
#include <lh/gl/text.h>

const lh_char_t *
lh_gl_label_get_text(const lh_gl_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

const lh_gl_container_t *
lh_gl_label_get_container(const lh_gl_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->container);
}

lh_gl_container_t *
lh_gl_label_get_container_mut(lh_gl_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->container);
}

const lh_gl_label_metrics_t *
lh_gl_label_get_metrics(const lh_gl_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->metrics);
}

lh_gl_label_metrics_t *
lh_gl_label_get_metrics_mut(lh_gl_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->metrics);
}

const lh_gl_label_seen_t *
lh_gl_label_get_seen(const lh_gl_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->seen);
}

lh_gl_label_seen_t *
lh_gl_label_get_seen_mut(lh_gl_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->seen);
}

const lh_gl_entity_class_t lh_gl_label_class = {lh_gl_label_event,
                                                lh_addr_of(lh_gl_container_class)};

lh_void
lh_gl_label_set_container(lh_gl_label_t *self, lh_gl_container_t container)
{
    lh_assert_runtime_ref(self);
    self->container = container;
}

lh_bool_t
lh_gl_label_is_metrics_ready(const lh_gl_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->metrics_ready;
}

lh_void
lh_gl_label_set_metrics_ready(lh_gl_label_t *self, lh_bool_t metrics_ready)
{
    lh_assert_runtime_ref(self);
    self->metrics_ready = metrics_ready;
}

lh_void
lh_gl_label_set_text(lh_gl_label_t *self, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

lh_bool_t
lh_gl_label_is_text_hashed(const lh_gl_label_t *self)
{
    return lh_gl_label_seen_is_hashed(lh_gl_label_get_seen(self));
}

lh_void
lh_gl_label_set_text_hashed(lh_gl_label_t *self, lh_bool_t text_hashed)
{
    lh_gl_label_seen_set_hashed(lh_gl_label_get_seen_mut(self), text_hashed);
}

lh_u32_t
lh_gl_label_get_text_hash(const lh_gl_label_t *self)
{
    return lh_gl_label_seen_get_hash(lh_gl_label_get_seen(self));
}

lh_void
lh_gl_label_set_text_hash(lh_gl_label_t *self, lh_u32_t text_hash)
{
    lh_gl_label_seen_set_hash(lh_gl_label_get_seen_mut(self), text_hash);
}

lh_u32_t
lh_gl_label_get_text_length(const lh_gl_label_t *self)
{
    return lh_gl_label_seen_get_length(lh_gl_label_get_seen(self));
}

lh_void
lh_gl_label_set_text_length(lh_gl_label_t *self, lh_u32_t text_length)
{
    lh_gl_label_seen_set_length(lh_gl_label_get_seen_mut(self), text_length);
}

const lh_gl_rect_t *
lh_gl_label_get_text_noted(const lh_gl_label_t *self)
{
    return lh_gl_label_seen_get_noted(lh_gl_label_get_seen(self));
}

lh_gl_rect_t *
lh_gl_label_get_text_noted_mut(lh_gl_label_t *self)
{
    return lh_gl_label_seen_get_noted_mut(lh_gl_label_get_seen_mut(self));
}

lh_void
lh_gl_label_set_text_noted(lh_gl_label_t *self, lh_gl_rect_t text_noted)
{
    lh_gl_label_seen_set_noted(lh_gl_label_get_seen_mut(self), text_noted);
}

const struct lh_font *
lh_gl_label_get_metrics_font(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_font(lh_gl_label_get_metrics(self));
}

lh_void
lh_gl_label_set_metrics_font(lh_gl_label_t *self, const struct lh_font *metrics_font)
{
    lh_gl_label_metrics_set_font(lh_gl_label_get_metrics_mut(self), metrics_font);
}

const lh_gl_rect_t *
lh_gl_label_get_metrics_rect(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_rect(lh_gl_label_get_metrics(self));
}

lh_gl_rect_t *
lh_gl_label_get_metrics_rect_mut(lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_rect_mut(lh_gl_label_get_metrics_mut(self));
}

lh_void
lh_gl_label_set_metrics_rect(lh_gl_label_t *self, lh_gl_rect_t metrics_rect)
{
    lh_gl_label_metrics_set_rect(lh_gl_label_get_metrics_mut(self), metrics_rect);
}

const lh_gl_insets_t *
lh_gl_label_get_metrics_padding(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_padding(lh_gl_label_get_metrics(self));
}

lh_gl_insets_t *
lh_gl_label_get_metrics_padding_mut(lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_padding_mut(lh_gl_label_get_metrics_mut(self));
}

lh_void
lh_gl_label_set_metrics_padding(lh_gl_label_t *self, lh_gl_insets_t metrics_padding)
{
    lh_gl_label_metrics_set_padding(lh_gl_label_get_metrics_mut(self), metrics_padding);
}

lh_gl_text_align_h_t
lh_gl_label_get_metrics_align_h(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_align_h(lh_gl_label_get_metrics(self));
}

lh_void
lh_gl_label_set_metrics_align_h(lh_gl_label_t *self, lh_gl_text_align_h_t metrics_align_h)
{
    lh_gl_label_metrics_set_align_h(lh_gl_label_get_metrics_mut(self), metrics_align_h);
}

lh_gl_text_align_v_t
lh_gl_label_get_metrics_align_v(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_align_v(lh_gl_label_get_metrics(self));
}

lh_void
lh_gl_label_set_metrics_align_v(lh_gl_label_t *self, lh_gl_text_align_v_t metrics_align_v)
{
    lh_gl_label_metrics_set_align_v(lh_gl_label_get_metrics_mut(self), metrics_align_v);
}

const lh_gl_point_t *
lh_gl_label_get_metrics_origin(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_origin(lh_gl_label_get_metrics(self));
}

lh_gl_point_t *
lh_gl_label_get_metrics_origin_mut(lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_origin_mut(lh_gl_label_get_metrics_mut(self));
}

lh_void
lh_gl_label_set_metrics_origin(lh_gl_label_t *self, lh_gl_point_t metrics_origin)
{
    lh_gl_label_metrics_set_origin(lh_gl_label_get_metrics_mut(self), metrics_origin);
}

lh_gl_scalar_t
lh_gl_label_get_metrics_ink_top(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_ink_top(lh_gl_label_get_metrics(self));
}

lh_void
lh_gl_label_set_metrics_ink_top(lh_gl_label_t *self, lh_gl_scalar_t metrics_ink_top)
{
    lh_gl_label_metrics_set_ink_top(lh_gl_label_get_metrics_mut(self), metrics_ink_top);
}

lh_gl_scalar_t
lh_gl_label_get_metrics_wrap(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_wrap(lh_gl_label_get_metrics(self));
}

lh_void
lh_gl_label_set_metrics_wrap(lh_gl_label_t *self, lh_gl_scalar_t metrics_wrap)
{
    lh_gl_label_metrics_set_wrap(lh_gl_label_get_metrics_mut(self), metrics_wrap);
}

const lh_gl_rect_t *
lh_gl_label_get_metrics_ink(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_ink(lh_gl_label_get_metrics(self));
}

lh_gl_rect_t *
lh_gl_label_get_metrics_ink_mut(lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_ink_mut(lh_gl_label_get_metrics_mut(self));
}

lh_void
lh_gl_label_set_metrics_ink(lh_gl_label_t *self, lh_gl_rect_t metrics_ink)
{
    lh_gl_label_metrics_set_ink(lh_gl_label_get_metrics_mut(self), metrics_ink);
}

const lh_gl_size_t *
lh_gl_label_get_metrics_size(const lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_size(lh_gl_label_get_metrics(self));
}

lh_gl_size_t *
lh_gl_label_get_metrics_size_mut(lh_gl_label_t *self)
{
    return lh_gl_label_metrics_get_size_mut(lh_gl_label_get_metrics_mut(self));
}

lh_void
lh_gl_label_set_metrics_size(lh_gl_label_t *self, lh_gl_size_t metrics_size)
{
    lh_gl_label_metrics_set_size(lh_gl_label_get_metrics_mut(self), metrics_size);
}

lh_gl_entity_t *
lh_gl_label_as_entity(lh_gl_label_t *self)
{
    return lh_gl_container_as_entity(lh_gl_label_get_container_mut(self));
}

lh_gl_container_t *
lh_gl_label_as_container(lh_gl_label_t *self)
{
    return lh_gl_label_get_container_mut(self);
}

lh_void
lh_gl_label_init(lh_gl_label_t *self, lh_gl_rect_t rect, const lh_char_t *text)
{
    lh_gl_container_init(lh_gl_label_get_container_mut(self), rect);
    lh_gl_label_set_text(self, text);
    lh_gl_label_set_metrics_ready(self, lh_bool_false);
    /* Hashed from the start, so a refresh that sets the text the label was built with
       records nothing. Nothing is noted here: the label is in no tree yet. */
    lh_u32_t length;
    lh_gl_label_set_text_hash(self, lh_gl_label_hash_text(text, lh_addr_of(length)));
    lh_gl_label_set_text_length(self, length);
    lh_gl_label_set_text_hashed(self, lh_bool_true);
    lh_gl_rect_init_empty(lh_gl_label_get_text_noted_mut(self));
    lh_gl_entity_set_class_v(lh_gl_label_as_entity(self), lh_addr_of(lh_gl_label_class));
}
