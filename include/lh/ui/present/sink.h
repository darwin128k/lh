/**
 * @file sink.h
 * @brief One present target: ::lh_ui_present_sink_t.
 *
 * Distinct from ::lh_ui_canvas_backend_t. The backend **draws** into a pixmap
 * (or records ops). The sink **shows** damage from that pixmap on a panel,
 * window, web player, or log. An application picks one sink or several; the
 * same frame fans out to every enabled sink.
 *
 * F4VE maps here as: RGB565 strip pixmap → ILI9341 flush with MADCTL mirror.
 * Desktop GDI maps as: DIB section → BitBlt of the drawn rects.
 */

#ifndef LH_UI_PRESENT_SINK_H
#define LH_UI_PRESENT_SINK_H

#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/present/sink/fields.h>
#include <lh/ui/rects.h>
#include <lh/void.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_ui_present_fn
 * @brief Show @p drawn of @p pixels on the sink. @p drawn is in pixmap space
 *        (strip-local when the display is partial). Empty @p drawn means the
 *        whole pixmap, the same rule as ::lh_ui_canvas_end_fn.
 */
typedef lh_void(lh_ui_present_fn)(lh_ptr context, const lh_ui_pixmap_t *pixels,
                                  const lh_ui_rects_t *drawn);

/**
 * @struct lh_ui_present_sink
 * @typedef lh_ui_present_sink_t
 * @brief Name + present slot + context. Not owned by the display.
 */
struct lh_ui_present_sink
{
    lh_ui_present_sink_fields(const char *, lh_ui_present_fn, lh_ptr);
};
typedef struct lh_ui_present_sink lh_ui_present_sink_t;

/**
 * @brief Fill @p self: @p name (may be ::lh_null), @p present (may be
 *        ::lh_null → no-op), @p context.
 */
lh_void
lh_ui_present_sink_init(lh_ui_present_sink_t *self, const char *name, lh_ui_present_fn *present,
                        lh_ptr context);

/**
 * @brief Call @p self→present when both sink and @p pixels are live.
 */
lh_void
lh_ui_present_sink_run(const lh_ui_present_sink_t *self, const lh_ui_pixmap_t *pixels,
                       const lh_ui_rects_t *drawn);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_PRESENT_SINK_H */
