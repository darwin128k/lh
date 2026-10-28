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
#include <lh/ui/canvas/web/atlas.h>
#include <lh/str/lit.h>
#include <lh/ui/web/buf.h>
#include <lh/ui/canvas/web/glyphs.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/canvas/web.h requires LH_LIBRARY_OPTION_UI"
#endif

/** Append string literal @p s to the ops of @p self, its length taken at compile time. */
#define lh_ui_canvas_web_put_lit(self, s) lh_ui_web_buf_lit(lh_addr_of((self)->buf), lh_str_lit(s))

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief An open JSON array of canvas ops, in a buffer the caller owns.
 */
typedef struct lh_ui_canvas_web
{
    lh_ui_web_buf_t buf; /**< The caller's bytes; failed once they ran out. */
    lh_uint_t width;     /**< Frame width in pixels. */
    lh_uint_t height;    /**< Frame height in pixels. */
    lh_uint_t ops;       /**< Ops recorded so far. */
    lh_bool_t comma;     /**< True when the next op needs a separating comma. */
    lh_ui_canvas_web_glyphs_t
        *glyphs;            /**< Which covers the reader has; ::lh_null = this frame's own. */
    lh_bool_t glyphs_owned; /**< @c glyphs was allocated here and is freed on close. */
} lh_ui_canvas_web_t;

/** The table ::lh_ui_canvas_init wants. */
extern const lh_ui_canvas_backend_t lh_ui_canvas_backend_web;

/** One number, without a format string to parse: a frame is mostly numbers. */
lh_void
lh_ui_canvas_web_put_int(lh_ui_canvas_web_t *self, lh_sint_t v);

/** Append the JSON array `[a,b,c,d]`. */
lh_void
lh_ui_canvas_web_put_quad(lh_ui_canvas_web_t *self, lh_sint_t a, lh_sint_t b, lh_sint_t c,
                          lh_sint_t d);

/** Append @p n bytes of @p px as lower-case hex. */
lh_void
lh_ui_canvas_web_put_hex(lh_ui_canvas_web_t *self, const lh_byte_ptr px, lh_usize_t n);

/** Start an op: the separating comma when one is due, and count it. */
lh_void
lh_ui_canvas_web_begin_op(lh_ui_canvas_web_t *self);

/** Append @p color as `[r,g,b,a]`. */
lh_void
lh_ui_canvas_web_put_color(lh_ui_canvas_web_t *self, const lh_ui_color_t *color);

/** Append @p rect as `[x,y,w,h]`. */
lh_void
lh_ui_canvas_web_put_rect(lh_ui_canvas_web_t *self, const lh_ui_rect_t *rect);

/** Which covers the reader holds: the caller's set, or one owned by this frame. */
lh_ui_canvas_web_glyphs_t *
lh_ui_canvas_web_get_glyphs(lh_ui_canvas_web_t *self);

/** Free the set ::lh_ui_canvas_web_get_glyphs made for this frame, if it made one. */
lh_void
lh_ui_canvas_web_free_glyphs(lh_ui_canvas_web_t *self);

/** The id of @p px, and whether the reader still needs its cover. -1 when the
 * table is full: the caller sends the cover inline, every time. */
lh_sint_t
lh_ui_canvas_web_intern(lh_ui_canvas_web_t *self, const lh_byte_ptr px, lh_s32_t w, lh_s32_t h,
                        lh_bool_t *is_new);

/** Empty @p self: no buffer, no ops, sized @p width x @p height. */
lh_void
lh_ui_canvas_web_reset(lh_ui_canvas_web_t *self, lh_uint_t width, lh_uint_t height);

/** Append the frame header and open the `ops` array. */
lh_void
lh_ui_canvas_web_put_head(lh_ui_canvas_web_t *self);

/** Backend `clear`: record a `clear` op. */
lh_void
lh_ui_canvas_web_clear(lh_ptr context, const lh_ui_color_t *color);

/** Backend `fill_rect`: record a `fill_rect` op. */
lh_void
lh_ui_canvas_web_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color);

/** Backend `fill_round_rect`: record a `fill_round_rect` op. */
lh_bool_t
lh_ui_canvas_web_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                 const lh_ui_color_t *color);

/** Backend `set_clip`: ignored, the ops already arrive cut by the canvas. */
lh_void
lh_ui_canvas_web_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip);

/** Backend `shadow`: record a `shadow` op. */
lh_bool_t
lh_ui_canvas_web_shadow(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                        const lh_ui_shadow_t *shadow);

/** A mask too big for the intern table: its coverage goes inline, a row at a time. */
lh_void
lh_ui_canvas_web_put_cover(lh_ui_canvas_web_t *self, const lh_ui_mask_t *mask, lh_s32_t w,
                           lh_s32_t h);

/** Backend `fill_mask`: a `fill_mask` op with an interned cover id, and the cover
 * when the reader lacks it. */
lh_bool_t
lh_ui_canvas_web_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                           const lh_ui_color_t *color);

/**
 * @brief Start a buffer of ops. @p buf is the caller's and is not freed here.
 *
 * A file is not a second sink: write ::lh_ui_canvas_web_bytes through
 * ::lh_os_fs_file_write.
 */
lh_bool_t
lh_ui_canvas_web_open_mem(lh_ui_canvas_web_t *self, lh_str_ptr buf, lh_usize_t cap, lh_uint_t width,
                          lh_uint_t height);

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
const lh_str_ptr
lh_ui_canvas_web_bytes(const lh_ui_canvas_web_t *self);

/** How many of those bytes. */
lh_usize_t
lh_ui_canvas_web_length(const lh_ui_canvas_web_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_CANVAS_WEB_H */
