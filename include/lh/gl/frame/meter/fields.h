/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gl_frame_meter_t.
 */

#ifndef LH_GL_FRAME_METER_FIELDS_H
#define LH_GL_FRAME_METER_FIELDS_H

#include <lh/core/ptr.h>

/**
 * @def lh_gl_frame_meter_fields(stats_type, paint_cb_type)
 * @brief The stats a meter records into, and the paint it measures.
 *
 * Nothing here is owned: the stats and the paint's context are the caller's.
 *
 * @param stats_type    Type of the stats (::lh_gl_frame_stats_t).
 * @param paint_cb_type Type of the measured paint (::lh_os_window_on_paint_cb).
 */
#define lh_gl_frame_meter_fields(stats_type, paint_cb_type)                                        \
    stats_type *stats;                                                                             \
    paint_cb_type on_paint;                                                                        \
    lh_ptr on_paint_context

#endif /* LH_GL_FRAME_METER_FIELDS_H */
