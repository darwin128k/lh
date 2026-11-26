/**
 * @file image.c
 * @brief Implementation of `lh/gl/image.h`.
 */
#include <lh/core/assert.h>

#include <lh/gl/canvas/mask.h>
#include <lh/gl/image.h>
#include <lh/core/util/return.h>

const lh_gl_entity_class_t lh_gl_image_class = {lh_gl_image_event, lh_addr_of(lh_gl_entity_class)};

lh_void
lh_gl_image_set_baseline(lh_gl_image_t *self, lh_gl_scalar_t baseline)
{
    lh_assert_runtime_ref(self);
    self->baseline = baseline;
}

lh_gl_scalar_t
lh_gl_image_get_baseline(const lh_gl_image_t *self)
{
    lh_assert_runtime_ref(self);
    return self->baseline;
}

lh_gl_entity_t *
lh_gl_image_as_entity(lh_gl_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_gl_image_t *
lh_gl_entity_as_image(lh_gl_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(
        !lh_gl_entity_class_is(lh_gl_entity_get_class(entity), lh_addr_of(lh_gl_image_class)),
        lh_null);
    return lh_ptr_rcast(lh_gl_image_t, entity);
}

const lh_gl_mask_t *
lh_gl_image_get_mask(const lh_gl_image_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mask;
}

lh_void
lh_gl_image_set_mask(lh_gl_image_t *self, const lh_gl_mask_t *mask)
{
    lh_assert_runtime_ref(self);
    self->mask = mask;
}

const lh_gl_color_t *
lh_gl_image_get_tint(const lh_gl_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tint);
}

const lh_gl_entity_t *
lh_gl_image_get_entity(const lh_gl_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_gl_entity_t *
lh_gl_image_get_entity_mut(lh_gl_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_void
lh_gl_image_set_entity(lh_gl_image_t *self, lh_gl_entity_t entity)
{
    lh_assert_runtime_ref(self);
    self->entity = entity;
}

lh_gl_color_t *
lh_gl_image_get_tint_mut(lh_gl_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->tint);
}

lh_void
lh_gl_image_set_tint(lh_gl_image_t *self, lh_gl_color_t tint)
{
    lh_assert_runtime_ref(self);
    self->tint = tint;
}

/* -- Events ---------------------------------------------------------------- */

/**
 * @copydoc lh_gl_image_on_baseline(const struct lh_gl_image *, const struct lh_gl_entity_event *)
 */
lh_void
lh_gl_image_on_baseline(const lh_gl_image_t *self, const lh_gl_entity_event_t *event)
{
    lh_gl_scalar_t *answer;

    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_baseline);
    answer = lh_gl_entity_event_get_baseline(lh_ptr_rcast(lh_gl_entity_event_t, event));
    lh_ptr_deref(answer) = lh_gl_image_get_baseline(self);
}

/* -- Lifetime and fields --------------------------------------------------- */

lh_void
lh_gl_image_init(lh_gl_image_t *self, lh_gl_rect_t rect, const lh_gl_mask_t *mask)
{
    lh_gl_entity_init(lh_gl_image_get_entity_mut(self), rect);
    lh_gl_entity_set_class_v(lh_gl_image_get_entity_mut(self), lh_addr_of(lh_gl_image_class));
    lh_gl_image_set_mask(self, mask);
    /* A mask is cropped to ink and knows nothing about the type it came out of,
       so a picture has no line until somebody says where its is. */
    lh_gl_image_set_baseline(self, lh_gl_scalar(-1));
    lh_gl_color_init(lh_gl_image_get_tint_mut(self), 255, 255, 255, 255);
}

/* -- Picture --------------------------------------------------------------- */

lh_void
lh_gl_image_set_tint_v(lh_gl_image_t *self, const lh_gl_color_t *tint)
{
    lh_return_if(lh_null_eq(tint));
    lh_gl_image_set_tint(self, lh_ptr_deref(tint));
}

lh_void
lh_gl_image_get_size(const lh_gl_image_t *self, lh_gl_size_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), lh_gl_scalar(0), lh_gl_scalar(0));
    if (lh_null_ne(lh_gl_image_get_mask(self)))
    {
        lh_gl_size_init(lh_addr_of(size),
                        lh_gl_scalar_of(lh_gl_mask_get_width(lh_gl_image_get_mask(self))),
                        lh_gl_scalar_of(lh_gl_mask_get_height(lh_gl_image_get_mask(self))));
    }
    lh_ptr_deref(out) = size;
}

lh_void
lh_gl_image_get_origin(const lh_gl_image_t *self, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_gl_entity_t *entity = lh_gl_image_as_entity(lh_ptr_rcast(lh_gl_image_t, self));
    lh_gl_rect_t rect;
    lh_gl_entity_copy_rect(entity, lh_addr_of(rect));
    lh_gl_insets_t padding;
    lh_gl_entity_get_padding(entity, lh_addr_of(padding));
    const lh_gl_point_t corner = lh_ptr_deref(lh_gl_rect_get_origin(lh_addr_of(rect)));
    lh_gl_point_t origin;

    /* The corner of the box, moved in by the padding: the picture is not
       aligned and not scaled, so this is the whole of where it goes. */
    lh_gl_point_init(
        lh_addr_of(origin),
        lh_gl_point_get_x(lh_addr_of(corner)) + lh_gl_insets_get_left(lh_addr_of(padding)),
        lh_gl_point_get_y(lh_addr_of(corner)) + lh_gl_insets_get_top(lh_addr_of(padding)));
    lh_ptr_deref(out) = origin;
}

/** @copydoc lh_gl_image_draw(const struct lh_gl_image *, struct lh_gl_canvas *) */
lh_void
lh_gl_image_draw(const lh_gl_image_t *self, lh_gl_canvas_t *canvas)
{
    lh_return_if(lh_null_eq(canvas) || lh_null_eq(lh_gl_image_get_mask(self)));
    /* The mask starts where lh_gl_image_get_origin says and is as big as it is:
       it is not scaled, so there is nothing to centre. */
    lh_gl_point_t origin;
    lh_gl_image_get_origin(self, lh_addr_of(origin));
    lh_gl_canvas_fill_mask(canvas, lh_gl_image_get_mask(self), origin, lh_gl_image_get_tint(self));
}

/**
 * @copydoc lh_gl_image_on_measure(const struct lh_gl_image *, const struct lh_gl_entity_event *)
 */
lh_void
lh_gl_image_on_measure(const lh_gl_image_t *self, const lh_gl_entity_event_t *event)
{
    const lh_gl_mask_t *mask = lh_gl_image_get_mask(self);

    lh_return_if(lh_null_eq(mask));
    lh_gl_rect_t *bounds = lh_gl_entity_event_get_bounds(event);
    lh_gl_point_t origin;
    /* Where it would sit: the picture is as big as the mask, and it keeps the
       padding that keeps it off the edges — the same box the text of a label
       measures as. */
    lh_gl_image_get_origin(self, lh_addr_of(origin));
    lh_gl_size_t size;
    lh_gl_image_get_size(self, lh_addr_of(size));
    lh_gl_rect_t picture;
    lh_gl_rect_init(lh_addr_of(picture), lh_gl_point_get_x(lh_addr_of(origin)),
                    lh_gl_point_get_y(lh_addr_of(origin)), lh_gl_size_get_width(lh_addr_of(size)),
                    lh_gl_size_get_height(lh_addr_of(size)));
    lh_gl_rect_union(bounds, lh_addr_of(picture), bounds);
}

/** @copydoc lh_gl_image_event(const struct lh_gl_entity *, const struct lh_gl_entity_event *) */
lh_void
lh_gl_image_event(const lh_gl_entity_t *self, const lh_gl_entity_event_t *event)
{
    /* The entity is the first field: self is the image. */
    const lh_gl_image_t *image = lh_ptr_rcast(const lh_gl_image_t, self);

    lh_gl_entity_class_event_base(lh_addr_of(lh_gl_image_class), self, event);
    if (lh_gl_entity_event_get_code(event) == lh_gl_entity_event_draw)
    {
        lh_gl_image_draw(image, lh_gl_entity_event_get_canvas(event));
    }
    if (lh_gl_entity_event_get_code(event) == lh_gl_entity_event_measure)
    {
        lh_gl_image_on_measure(image, event);
    }
    if (lh_gl_entity_event_get_code(event) == lh_gl_entity_event_baseline)
    {
        lh_gl_image_on_baseline(image, event);
    }
}
