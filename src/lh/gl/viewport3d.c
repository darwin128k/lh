/**
 * @file viewport3d.c
 * @brief Implementation of `lh/gl/viewport3d.h`.
 */

#include <lh/gl/viewport3d.h>
#include <lh/core/attribute/unused.h>

lh_void
lh_gl_viewport3d_null_begin(lh_ptr context, const lh_gl_rect_t *rect)
{
    LH_ATTRIBUTE_UNUSED(context);
    LH_ATTRIBUTE_UNUSED(rect);
}

lh_void
lh_gl_viewport3d_null_end(lh_ptr context)
{
    LH_ATTRIBUTE_UNUSED(context);
}

const lh_gl_viewport3d_backend_t lh_gl_viewport3d_backend_null = {
    lh_gl_viewport3d_null_begin,
    lh_gl_viewport3d_null_end,
};

lh_void
lh_gl_viewport3d_begin(const lh_gl_viewport3d_backend_t *backend, lh_ptr context,
                       const lh_gl_rect_t *rect)
{
    if (backend == lh_null || backend->begin == lh_null)
    {
        return;
    }
    backend->begin(context, rect);
}

lh_void
lh_gl_viewport3d_end(const lh_gl_viewport3d_backend_t *backend, lh_ptr context)
{
    if (backend == lh_null || backend->end == lh_null)
    {
        return;
    }
    backend->end(context);
}
