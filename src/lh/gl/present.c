/**
 * @file present.c
 * @brief Fan-out ::lh_gl_present_run.
 */
#include <lh/gl/present.h>

lh_u32_t
lh_gl_present_enable_all(lh_u32_t count)
{
    if (count > lh_cast_static(lh_u32_t, LH_GL_PRESENT_SINKS_MAX))
    {
        count = lh_cast_static(lh_u32_t, LH_GL_PRESENT_SINKS_MAX);
    }
    if (count == 0u)
    {
        return 0u;
    }
    if (count >= 32u)
    {
        return 0xFFFFFFFFu;
    }
    return (1u << count) - 1u;
}

lh_void
lh_gl_present_run(const lh_gl_present_sink_t *sinks, lh_u32_t count, lh_u32_t enable,
                  const lh_gl_pixmap_t *pixels, const lh_gl_rects_t *drawn)
{
    if (sinks == lh_null || pixels == lh_null)
    {
        return;
    }
    if (count > lh_cast_static(lh_u32_t, LH_GL_PRESENT_SINKS_MAX))
    {
        count = lh_cast_static(lh_u32_t, LH_GL_PRESENT_SINKS_MAX);
    }
    for (lh_u32_t i = 0; i < count; ++i)
    {
        if ((enable & (1u << i)) != 0u)
        {
            lh_gl_present_sink_run(lh_addr_of(sinks[i]), pixels, drawn);
        }
    }
}
