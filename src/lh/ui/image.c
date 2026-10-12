/**
 * @file image.c
 * @brief Implementation of `lh/ui/image.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/entity.h>
#include <lh/ui/image.h>
#include <lh/ui/insets.h>
#include <lh/ui/point.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

const lh_ui_entity_class_t lh_ui_image_class = {lh_ui_image_event, lh_addr_of(lh_ui_entity_class)};

/* ── Events ──────────────────────────────────────────────────────────────── */

lh_void
lh_ui_image_event(const lh_ui_entity_t *self, const lh_ui_entity_event_t *event)
{
    /* The entity is the first field: self is the image. */
    const lh_ui_image_t *image = lh_ptr_rcast(const lh_ui_image_t, self);

    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_image_class), self, event);
    if (lh_ui_entity_event_get_code(event) == lh_ui_entity_event_draw)
    {
        lh_ui_image_draw(image, lh_ui_entity_event_get_canvas(event));
    }
    if (lh_ui_entity_event_get_code(event) == lh_ui_entity_event_measure)
    {
        lh_ui_image_on_measure(image, event);
    }
    if (lh_ui_entity_event_get_code(event) == lh_ui_entity_event_baseline)
    {
        lh_ui_image_on_baseline(image, event);
    }
}

lh_void
lh_ui_image_draw(const lh_ui_image_t *self, lh_ui_canvas_t *canvas)
{
    lh_ui_point_t origin;

    lh_return_if(lh_null_eq(canvas) || lh_null_eq(lh_ui_image_get_mask(self)));
    /* The mask starts where lh_ui_image_get_origin says and is as big as it is:
       it is not scaled, so there is nothing to centre. */
    origin = lh_ui_image_get_origin(self);
    lh_ui_canvas_fill_mask(canvas, lh_ui_image_get_mask(self), origin, lh_ui_image_get_tint(self));
}

lh_void
lh_ui_image_on_measure(const lh_ui_image_t *self, const lh_ui_entity_event_t *event)
{
    const lh_ui_mask_t *mask = lh_ui_image_get_mask(self);
    lh_ui_rect_t *bounds;
    lh_ui_point_t origin;
    lh_ui_size_t size;
    lh_ui_rect_t picture;

    lh_return_if(lh_null_eq(mask));
    bounds = lh_ui_entity_event_get_bounds(event);
    /* Where it would sit: the picture is as big as the mask, and it keeps the
       padding that keeps it off the edges — the same box the text of a label
       measures as. */
    origin = lh_ui_image_get_origin(self);
    size = lh_ui_image_get_size(self);
    lh_ui_rect_init(lh_addr_of(picture), lh_ui_point_get_x(&origin), lh_ui_point_get_y(&origin),
                    lh_ui_size_get_width(lh_addr_of(size)), lh_ui_size_get_height(lh_addr_of(size)));
    *bounds = lh_ui_rect_union(bounds, lh_addr_of(picture));
}

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

lh_void
lh_ui_image_on_baseline(const lh_ui_image_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_scalar_t *answer;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_baseline);
    answer = lh_ui_entity_event_get_baseline(lh_ptr_rcast(lh_ui_entity_event_t, event));
    *answer = self->baseline;
}

lh_void
lh_ui_image_set_baseline(lh_ui_image_t *self, lh_ui_scalar_t baseline)
{
    lh_assert_runtime_ref(self);
    self->baseline = baseline;
}

lh_ui_scalar_t
lh_ui_image_get_baseline(const lh_ui_image_t *self)
{
    lh_assert_runtime_ref(self);
    return self->baseline;
}

lh_void
lh_ui_image_init(lh_ui_image_t *self, lh_ui_rect_t rect, const lh_ui_mask_t *mask)
{
    lh_assert_runtime_ref(self);
    lh_ui_entity_init(lh_addr_of(self->entity), rect);
    lh_ui_entity_set_class(lh_addr_of(self->entity), lh_addr_of(lh_ui_image_class));
    self->mask = mask;
    /* A mask is cropped to ink and knows nothing about the type it came out of,
       so a picture has no line until somebody says where its is. */
    self->baseline = lh_ui_scalar(-1);
    lh_ui_color_init(lh_addr_of(self->tint), 255, 255, 255, 255);
}

lh_ui_entity_t *
lh_ui_image_as_entity(lh_ui_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_ui_image_t *
lh_ui_entity_as_image(lh_ui_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(!lh_ui_entity_class_is(lh_ui_entity_get_class(entity), lh_addr_of(lh_ui_image_class)),
                 lh_null);
    return lh_ptr_rcast(lh_ui_image_t, entity);
}

/* ── Picture ─────────────────────────────────────────────────────────────── */

const lh_ui_mask_t *
lh_ui_image_get_mask(const lh_ui_image_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mask;
}

lh_void
lh_ui_image_set_mask(lh_ui_image_t *self, const lh_ui_mask_t *mask)
{
    lh_assert_runtime_ref(self);
    self->mask = mask;
}

const lh_ui_color_t *
lh_ui_image_get_tint(const lh_ui_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tint);
}

lh_void
lh_ui_image_set_tint(lh_ui_image_t *self, const lh_ui_color_t *tint)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(tint));
    self->tint = *tint;
}

lh_ui_size_t
lh_ui_image_get_size(const lh_ui_image_t *self)
{
    lh_ui_size_t size;

    lh_assert_runtime_ref(self);
    lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(0), lh_ui_scalar(0));
    if (lh_null_ne(self->mask))
    {
        lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(lh_ui_mask_get_width(self->mask)),
                        lh_ui_scalar(lh_ui_mask_get_height(self->mask)));
    }
    return size;
}

lh_ui_point_t
lh_ui_image_get_origin(const lh_ui_image_t *self)
{
    const lh_ui_entity_t *entity = lh_ui_image_as_entity(lh_ptr_rcast(lh_ui_image_t, self));
    const lh_ui_rect_t rect = lh_ui_entity_get_rect(entity);
    const lh_ui_insets_t padding = lh_ui_entity_get_padding(entity);
    const lh_ui_point_t corner = *lh_ui_rect_get_origin_as_const(lh_addr_of(rect));
    lh_ui_point_t origin;

    /* The corner of the box, moved in by the padding: the picture is not
       aligned and not scaled, so this is the whole of where it goes. */
    lh_ui_point_init(lh_addr_of(origin),
                     lh_ui_point_get_x(&corner) + lh_ui_insets_get_left(&padding),
                     lh_ui_point_get_y(&corner) + lh_ui_insets_get_top(&padding));
    return origin;
}