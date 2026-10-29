/**
 * @file glyphs.h
 * @brief ::lh_ui_canvas_web_glyphs_t — which interned covers a reader has already been sent.
 */

#ifndef LH_UI_CANVAS_WEB_GLYPHS_H
#define LH_UI_CANVAS_WEB_GLYPHS_H

#include <lh/ui/canvas/backend.h>
#include <lh/ui/canvas/web/atlas.h>
#include <lh/str/lit.h>
#include <lh/ui/web/buf.h>

/**
 * @brief Which interned covers a reader has already been sent.
 *
 * One frame on its own carries every cover it uses once (the default, when
 * nothing is set with ::lh_ui_canvas_web_set_glyphs_v). A stream of frames to
 * the same reader keeps one of these across frames, so a glyph's coverage
 * crosses the wire once per reader and not once per frame; a new reader
 * starts from ::lh_ui_canvas_web_glyphs_reset.
 */
typedef struct lh_ui_canvas_web_glyphs
{
    lh_byte_t sent[LH_UI_CANVAS_WEB_GLYPHS_MAX]; /**< Non-zero for each glyph id already sent. */
} lh_ui_canvas_web_glyphs_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Forget every cover @p self says was sent: the next frame carries them again. */
lh_void
lh_ui_canvas_web_glyphs_reset(lh_ui_canvas_web_glyphs_t *self);

/**
 * @brief Which glyph masks the client already has (level 1).
 *
 * @param self The glyph cache; checked by ::lh_assert_runtime_ref.
 */
const lh_byte_t *
lh_ui_canvas_web_glyphs_get_sent(const lh_ui_canvas_web_glyphs_t *self);

/**
 * @brief Which glyph masks the client already has, to change in place (level 1).
 *
 * @param self The glyph cache; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_byte_t *
lh_ui_canvas_web_glyphs_get_sent_mut(lh_ui_canvas_web_glyphs_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_WEB_GLYPHS_H */
