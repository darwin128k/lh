/**
 * @file sink.c
 * @brief ::lh_ui_present_sink_t.
 */
#include <lh/ui/present/sink.h>
#include <lh/assert.h>

lh_void
lh_ui_present_sink_init(lh_ui_present_sink_t *self, const char *name, lh_ui_present_fn *present,
                        lh_ptr context)
{
    lh_ui_present_sink_set_name(self, name);
    lh_ui_present_sink_set_present_fn(self, present);
    lh_ui_present_sink_set_context(self, context);
}

lh_void
lh_ui_present_sink_run(const lh_ui_present_sink_t *self, const lh_ui_pixmap_t *pixels,
                       const lh_ui_rects_t *drawn)
{
    if (self == lh_null || lh_ui_present_sink_get_present_fn(self) == lh_null || pixels == lh_null)
    {
        return;
    }
    lh_ui_present_sink_get_present_fn(self)(lh_ui_present_sink_get_context(self), pixels, drawn);
}

const char *
lh_ui_present_sink_get_name(const lh_ui_present_sink_t *self)
{
    lh_assert_runtime_ref(self);
    return self->name;
}

lh_void
lh_ui_present_sink_set_name(lh_ui_present_sink_t *self, const char * name)
{
    lh_assert_runtime_ref(self);
    self->name = name;
}

lh_ui_present_fn *
lh_ui_present_sink_get_present_fn(const lh_ui_present_sink_t *self)
{
    lh_assert_runtime_ref(self);
    return self->present;
}

lh_void
lh_ui_present_sink_set_present_fn(lh_ui_present_sink_t *self, lh_ui_present_fn * present)
{
    lh_assert_runtime_ref(self);
    self->present = present;
}

lh_ptr
lh_ui_present_sink_get_context(const lh_ui_present_sink_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_ui_present_sink_set_context(lh_ui_present_sink_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}
