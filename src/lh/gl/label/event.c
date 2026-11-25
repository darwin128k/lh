/**
 * @file event.c
 * @brief Implementation of `lh/gl/label.h`: the entity events of a label: its children, its
 * baseline, its measure, its draw.
 */

#include <lh/gl/label.h>

#include <lh/core/attribute/unused.h>
#include <lh/gl/text.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_label_on_children(const lh_gl_label_t *self, const lh_gl_entity_event_t *event)
{
    lh_gl_entity_transform_t *transform;

    LH_ATTRIBUTE_UNUSED(self);
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_children);
    /* The transform is the one thing an event carries that a class fills in, and
       the getter takes it without the const every event handler is given. */
    transform = lh_gl_entity_event_get_transform(lh_ptr_rcast(lh_gl_entity_event_t, event));
    /* The container behind the label cut its children to its own rect, and that
       is right for a button and wrong for a label: the label's rect is where the
       text is *centred* (::lh_gl_text_get_size measures the cap line to the
       baseline), so the tail of a 'p' hangs below it. Cutting there drew half a
       letter — measured on the demo, "Hide panel" lost every pixel under row 199.
       The offset and the placement the container just did stay; only the knife
       goes. */
    lh_gl_entity_transform_set_clip(transform, lh_bool_false);
}

lh_void
lh_gl_label_on_baseline(const lh_gl_label_t *self, const lh_gl_entity_event_t *event)
{
    lh_gl_scalar_t *answer;

    LH_ATTRIBUTE_UNUSED(self);
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_baseline);
    /* The one thing that stands on a line in this picture is the text, and where
       that line is follows the alignment and the padding, not the box: a label
       centred in 28 rows has its baseline in the middle of them. */
    answer = lh_gl_entity_event_get_baseline(lh_ptr_rcast(lh_gl_entity_event_t, event));
    lh_ptr_deref(answer) = lh_gl_label_get_baseline(self);
}

lh_void
lh_gl_label_on_measure(const lh_gl_label_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_measure);
    lh_gl_rect_t *bounds = lh_gl_entity_event_get_bounds(event);
    lh_gl_rect_t text;
    lh_gl_label_get_text_rect(self, lh_addr_of(text));
    lh_gl_rect_union(bounds, lh_addr_of(text), bounds);
}

lh_void
lh_gl_label_on_draw(const lh_gl_label_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_draw);
    lh_gl_label_draw_text(self, lh_gl_entity_event_get_canvas(event));
}

lh_void
lh_gl_label_event(const struct lh_gl_entity *self, const lh_gl_entity_event_t *event)
{
    /* The container (and its entity) is the first field: self is the label. */
    const lh_gl_label_t *label = lh_ptr_rcast(const lh_gl_label_t, self);

    lh_gl_entity_class_event_base(lh_addr_of(lh_gl_label_class), self, event);
    lh_gl_label_on_children(label, event);
    lh_gl_label_on_baseline(label, event);
    lh_gl_label_on_draw(label, event);
    lh_gl_label_on_measure(label, event);
}
