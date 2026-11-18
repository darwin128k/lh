/**
 * @file class.c
 * @brief Implementation of `lh/gx/class.h`.
 */

#include <lh/gx/class.h>

lh_gx_class_paint_fn *
lh_gx_class_get_paint(const lh_gx_class_t *self)
{
    lh_assert_runtime_ref(self);
    return self->paint;
}

lh_void
lh_gx_class_set_paint(lh_gx_class_t *self, lh_gx_class_paint_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->paint = fn;
}

lh_gx_class_covers_fn *
lh_gx_class_get_covers(const lh_gx_class_t *self)
{
    lh_assert_runtime_ref(self);
    return self->covers;
}

lh_void
lh_gx_class_set_covers(lh_gx_class_t *self, lh_gx_class_covers_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->covers = fn;
}

lh_gx_class_hit_fn *
lh_gx_class_get_hit(const lh_gx_class_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hit;
}

lh_void
lh_gx_class_set_hit(lh_gx_class_t *self, lh_gx_class_hit_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->hit = fn;
}

lh_gx_class_input_fn *
lh_gx_class_get_input(const lh_gx_class_t *self)
{
    lh_assert_runtime_ref(self);
    return self->input;
}

lh_void
lh_gx_class_set_input(lh_gx_class_t *self, lh_gx_class_input_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->input = fn;
}

lh_gx_class_report_fn *
lh_gx_class_get_report(const lh_gx_class_t *self)
{
    lh_assert_runtime_ref(self);
    return self->report;
}

lh_void
lh_gx_class_set_report(lh_gx_class_t *self, lh_gx_class_report_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->report = fn;
}

lh_void
lh_gx_class_init(lh_gx_class_t *self)
{
    lh_gx_class_set_paint(self, lh_null);
    lh_gx_class_set_covers(self, lh_null);
    lh_gx_class_set_hit(self, lh_null);
    lh_gx_class_set_input(self, lh_null);
    lh_gx_class_set_report(self, lh_null);
}
