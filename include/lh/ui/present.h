/**
 * @file present.h
 * @brief Fan-out of damage to several ::lh_ui_present_sink_t.
 *
 * After a canvas/view frame, call ::lh_ui_present_run with the pixmap and the
 * drawn rects from ::lh_ui_canvas_end. Each enabled sink gets the same pair.
 * Order is the table order; sinks must not assume exclusivity of the bits.
 */

#ifndef LH_UI_PRESENT_H
#define LH_UI_PRESENT_H

#include <lh/ui/present/sink.h>

/**
 * @def LH_UI_PRESENT_SINKS_MAX
 * @brief How many sinks one fan-out table holds (window + web + panel + spare).
 */
#define LH_UI_PRESENT_SINKS_MAX 4

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Run @p count sinks (at most ::LH_UI_PRESENT_SINKS_MAX). @p enable is
 *        a bit mask: bit @c i set → sinks[i] runs. @p enable of all-ones runs
 *        every non-null present slot.
 */
lh_void
lh_ui_present_run(const lh_ui_present_sink_t *sinks, lh_u32_t count, lh_u32_t enable,
                  const lh_ui_pixmap_t *pixels, const lh_ui_rects_t *drawn);

/**
 * @brief Bit mask with the low @p count bits set (clamped to
 *        ::LH_UI_PRESENT_SINKS_MAX).
 */
lh_u32_t
lh_ui_present_enable_all(lh_u32_t count);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_PRESENT_H */
