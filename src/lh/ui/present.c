/**
 * @file present.c
 * @brief Fan-out ::lh_ui_present_run.
 */
#include <lh/ui/present.h>

lh_u32_t
lh_ui_present_enable_all(lh_u32_t count)
{
    if (count > (lh_u32_t)LH_UI_PRESENT_SINKS_MAX)
    {
        count = (lh_u32_t)LH_UI_PRESENT_SINKS_MAX;
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
lh_ui_present_run(const lh_ui_present_sink_t *sinks, lh_u32_t count, lh_u32_t enable,
                  const lh_ui_pixmap_t *pixels, const lh_ui_rects_t *drawn)
{
    lh_u32_t i;

    if (sinks == lh_null || pixels == lh_null)
    {
        return;
    }
    if (count > (lh_u32_t)LH_UI_PRESENT_SINKS_MAX)
    {
        count = (lh_u32_t)LH_UI_PRESENT_SINKS_MAX;
    }
    for (i = 0; i < count; ++i)
    {
        if ((enable & (1u << i)) != 0u)
        {
            lh_ui_present_sink_run(&sinks[i], pixels, drawn);
        }
    }
}
