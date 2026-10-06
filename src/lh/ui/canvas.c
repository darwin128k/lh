/**
 * @file canvas.c
 * @brief Implementation of `lh/ui/canvas.h` and the null backend.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/util/return.h>

/* ── Null backend ────────────────────────────────────────────────────────── */

const lh_ui_canvas_backend_t lh_ui_canvas_backend_null = {lh_null, lh_null, lh_null, lh_null};

/* ── Lifetime ────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_init(lh_ui_canvas_t *self, const lh_ui_canvas_backend_t *backend, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->backend = backend;
    self->context = context;
}

lh_void
lh_ui_canvas_deinit(lh_ui_canvas_t *self)
{
    lh_ui_canvas_init(self, lh_null, lh_null);
}

/* ── Accessors ───────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_set_backend(lh_ui_canvas_t *self, const lh_ui_canvas_backend_t *backend)
{
    lh_assert_runtime_ref(self);
    self->backend = backend;
}

const lh_ui_canvas_backend_t *
lh_ui_canvas_get_backend(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->backend;
}

lh_void
lh_ui_canvas_set_context(lh_ui_canvas_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_ptr
lh_ui_canvas_get_context(const lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

/* ── Frame ───────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_begin(lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->begin));
    self->backend->begin(self->context);
}

lh_void
lh_ui_canvas_end(lh_ui_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->end));
    self->backend->end(self->context);
}

lh_void
lh_ui_canvas_clear(lh_ui_canvas_t *self, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(color);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->clear));
    self->backend->clear(self->context, color);
}

lh_void
lh_ui_canvas_fill_rect(lh_ui_canvas_t *self, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_assert_runtime_ref(color);
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->fill_rect));
    self->backend->fill_rect(self->context, rect, color);
}
