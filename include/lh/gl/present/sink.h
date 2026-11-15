/**
 * @file sink.h
 * @brief One present target: ::lh_gl_present_sink_t.
 *
 * Distinct from ::lh_gl_canvas_backend_t. The backend **draws** into a pixmap
 * (or records ops). The sink **shows** damage from that pixmap on a panel,
 * window, web player, or log. An application picks one sink or several; the
 * same frame fans out to every enabled sink.
 *
 * F4VE maps here as: RGB565 strip pixmap → ILI9341 flush with MADCTL mirror.
 * Desktop GDI maps as: DIB section → BitBlt of the drawn rects.
 */

#ifndef LH_GL_PRESENT_SINK_H
#define LH_GL_PRESENT_SINK_H

#include <lh/gl/pixmap.h>
#include <lh/gl/present/sink/fields.h>
#include <lh/gl/rects.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gl_present_fn
 * @brief Show @p drawn of @p pixels on the sink. @p drawn is in pixmap space
 *        (strip-local when the display is partial). Empty @p drawn means the
 *        whole pixmap, the same rule as ::lh_gl_canvas_end_fn.
 */
typedef lh_void(lh_gl_present_fn)(lh_ptr context, const lh_gl_pixmap_t *pixels,
                                  const lh_gl_rects_t *drawn);

LH_COMPILER_EXTERN_C_END

/**
 * @struct lh_gl_present_sink
 * @brief Name + present slot + context. Not owned by the display.
 */
struct lh_gl_present_sink
{
    /** Members: ::lh_gl_present_sink_fields. */
    lh_gl_present_sink_fields(const char *, lh_gl_present_fn, lh_ptr);
};
/** @copybrief lh_gl_present_sink */
typedef struct lh_gl_present_sink lh_gl_present_sink_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self: @p name (may be ::lh_null), @p present (may be
 *        ::lh_null → no-op), @p context.
 */
lh_void
lh_gl_present_sink_init(lh_gl_present_sink_t *self, const char *name, lh_gl_present_fn *present,
                        lh_ptr context);

/**
 * @brief Call @p self→present when both sink and @p pixels are live.
 */
lh_void
lh_gl_present_sink_run(const lh_gl_present_sink_t *self, const lh_gl_pixmap_t *pixels,
                       const lh_gl_rects_t *drawn);

/**
 * @brief The name of the sink (level 1).
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 */
const char *
lh_gl_present_sink_get_name(const lh_gl_present_sink_t *self);

/**
 * @brief Store the name of the sink (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 * @param name The new value.
 */
lh_void
lh_gl_present_sink_set_name(lh_gl_present_sink_t *self, const char * name);

/**
 * @brief The call a finished strip is handed to (level 1).
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 */
lh_gl_present_fn *
lh_gl_present_sink_get_present_fn(const lh_gl_present_sink_t *self);

/**
 * @brief Store the call a finished strip is handed to (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 * @param present The new value.
 */
lh_void
lh_gl_present_sink_set_present_fn(lh_gl_present_sink_t *self, lh_gl_present_fn * present);

/**
 * @brief The value passed to that call (level 1).
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gl_present_sink_get_context(const lh_gl_present_sink_t *self);

/**
 * @brief Store the value passed to that call (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 * @param context The new value.
 */
lh_void
lh_gl_present_sink_set_context(lh_gl_present_sink_t *self, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_PRESENT_SINK_H */
