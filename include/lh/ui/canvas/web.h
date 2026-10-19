/**
 * @file web.h
 * @brief Canvas backend that records primitives as JSON: ::lh_ui_canvas_backend_web.
 *
 * Same slot table as GDI and the software pixmap. A browser that replays the
 * bytes is painting this stream, not a DOM layout. Glyph covers are interned
 * (`m` plus `cover` on first use). A 3D viewport is not an op here — see
 * `lh/ui/viewport3d.h`. XP never loads that player; the desktop there stays
 * GDI.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_CANVAS_WEB_H
#define LH_UI_CANVAS_WEB_H

#include <lh/ui/canvas/backend.h>
#include <lh/ui/web/buf.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/canvas/web.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/** How many distinct glyph covers the process-wide intern table holds. */
#define LH_UI_CANVAS_WEB_GLYPHS_MAX 1024u

/**
 * @brief Which interned covers a reader has already been sent.
 *
 * One frame on its own carries every cover it uses once (the default, when
 * nothing is set with ::lh_ui_canvas_web_set_glyphs). A stream of frames to
 * the same reader keeps one of these across frames, so a glyph's coverage
 * crosses the wire once per reader and not once per frame; a new reader
 * starts from ::lh_ui_canvas_web_glyphs_reset.
 */
typedef struct lh_ui_canvas_web_glyphs
{
    lh_byte_t sent[LH_UI_CANVAS_WEB_GLYPHS_MAX]; /**< Non-zero for each glyph id already sent. */
} lh_ui_canvas_web_glyphs_t;

/**
 * @brief An open JSON array of canvas ops, in a buffer the caller owns.
 */
typedef struct lh_ui_canvas_web
{
    lh_ui_web_buf_t buf; /**< The caller's bytes; failed once they ran out. */
    unsigned width;      /**< Frame width in pixels. */
    unsigned height;     /**< Frame height in pixels. */
    unsigned ops;        /**< Ops recorded so far. */
    int comma;           /**< Non-zero when the next op needs a separating comma. */
    lh_ui_canvas_web_glyphs_t
        *glyphs;            /**< Which covers the reader has; ::lh_null = this frame's own. */
    lh_bool_t glyphs_owned; /**< @c glyphs was allocated here and is freed on close. */
} lh_ui_canvas_web_t;

/** The table ::lh_ui_canvas_init wants. */
extern const lh_ui_canvas_backend_t lh_ui_canvas_backend_web;

/**
 * @brief Start a buffer of ops. @p buf is the caller's and is not freed here.
 *
 * A file is not a second sink: write ::lh_ui_canvas_web_bytes through
 * ::lh_os_fs_file_write.
 */
lh_bool_t
lh_ui_canvas_web_open_mem(lh_ui_canvas_web_t *self, char *buf, lh_usize_t cap, unsigned width,
                          unsigned height);

/** Forget every cover @p self says was sent: the next frame carries them again. */
lh_void
lh_ui_canvas_web_glyphs_reset(lh_ui_canvas_web_glyphs_t *self);

/**
 * @brief Record sent covers in @p glyphs (not owned) instead of in this frame alone.
 *
 * Call after open and before the first op. A cover marked there is sent as
 * its id only, so the reader must have kept every earlier frame's covers.
 */
lh_void
lh_ui_canvas_web_set_glyphs(lh_ui_canvas_web_t *self, lh_ui_canvas_web_glyphs_t *glyphs);

/**
 * @brief Close the array. @p self's buffer stays valid at ::lh_ui_canvas_web_bytes.
 */
lh_bool_t
lh_ui_canvas_web_close(lh_ui_canvas_web_t *self);

/** Bytes of JSON after a successful close, else "". */
const char *
lh_ui_canvas_web_bytes(const lh_ui_canvas_web_t *self);

/** How many of those bytes. */
lh_usize_t
lh_ui_canvas_web_length(const lh_ui_canvas_web_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_WEB_H */
