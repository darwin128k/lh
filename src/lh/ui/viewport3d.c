/**
 * @file viewport3d.c
 * @brief Implementation of `lh/ui/viewport3d.h`.
 */

#include <lh/void.h>
#include <lh/ui/viewport3d.h>

#include <lh/null.h>
#include <lh/util/ptr.h>

static lh_void
viewport3d_null_begin(lh_ptr context, const lh_ui_rect_t *rect)
{
    (void)context;
    (void)rect;
}

static lh_void
viewport3d_null_end(lh_ptr context)
{
    (void)context;
}

const lh_ui_viewport3d_backend_t lh_ui_viewport3d_backend_null = {
    viewport3d_null_begin,
    viewport3d_null_end,
};

lh_void
lh_ui_viewport3d_begin(const lh_ui_viewport3d_backend_t *backend, lh_ptr context, const lh_ui_rect_t *rect)
{
    if (backend == lh_null || backend->begin == lh_null)
    {
        return;
    }
    backend->begin(context, rect);
}

lh_void
lh_ui_viewport3d_end(const lh_ui_viewport3d_backend_t *backend, lh_ptr context)
{
    if (backend == lh_null || backend->end == lh_null)
    {
        return;
    }
    backend->end(context);
}
