/**
 * @file sink.c
 * @brief ::lh_ui_present_sink_t.
 */
#include <lh/ui/present/sink.h>

lh_void
lh_ui_present_sink_init(lh_ui_present_sink_t *self, const char *name, lh_ui_present_fn *present,
                        lh_ptr context)
{
    self->name = name;
    self->present = present;
    self->context = context;
}

lh_void
lh_ui_present_sink_run(const lh_ui_present_sink_t *self, const lh_ui_pixmap_t *pixels,
                       const lh_ui_rects_t *drawn)
{
    if (self == lh_null || self->present == lh_null || pixels == lh_null)
    {
        return;
    }
    self->present(self->context, pixels, drawn);
}
