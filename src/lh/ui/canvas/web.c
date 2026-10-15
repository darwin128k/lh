/**
 * @file web.c
 * @brief Implementation of `lh/ui/canvas/web.h`.
 */

#include <lh/ui/canvas/web.h>
#include <lh/byte/ptr.h>

#include <lh/cast/const.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/runtime/allocator.h>
#include <lh/str/format/sint.h>
#include <lh/ui/color.h>
#include <lh/ui/mask.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/rects.h>
#include <lh/ui/scalar.h>
#include <lh/ui/shadow.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/cast/static.h>

/** Every write goes through here. A buffer always keeps one byte free, so close
 * can terminate it without a check of its own. */
static lh_void
web_put(lh_ui_canvas_web_t *self, const char *p, lh_usize_t n)
{
    if (!self->ok)
    {
        return;
    }
    if (self->mem == lh_null || n >= self->cap - self->len)
    {
        self->ok = lh_bool_false;
        return;
    }
    lh_memory_copy(self->mem + self->len, self->cap - self->len, lh_cast_const(lh_ptr, p), n);
    self->len += n;
}

/** Append string literal @p s, its length taken at compile time. */
#define web_lit(self, s) web_put((self), (s), lh_cast_static(lh_usize_t, (sizeof(s) - 1u)))

/** One number, without a format string to parse: a frame is mostly numbers. */
static lh_void
web_int(lh_ui_canvas_web_t *self, long v)
{
    char buf[24];

    web_put(self, buf, lh_str_ptr_format_sint(lh_cast_static(lh_sint_t, v), buf, sizeof(buf)));
}

/** Append the JSON array `[a,b,c,d]`. */
static lh_void
web_quad(lh_ui_canvas_web_t *self, long a, long b, long c, long d)
{
    web_lit(self, "[");
    web_int(self, a);
    web_lit(self, ",");
    web_int(self, b);
    web_lit(self, ",");
    web_int(self, c);
    web_lit(self, ",");
    web_int(self, d);
    web_lit(self, "]");
}

/** Append @p n bytes of @p px as lower-case hex. */
static lh_void
web_hex(lh_ui_canvas_web_t *self, lh_byte_cptr px, unsigned n)
{
    static const char digits[] = "0123456789abcdef";
    char buf[256];
    unsigned i;
    unsigned o = 0;

    for (i = 0; i < n; ++i)
    {
        buf[o++] = digits[px[i] >> 4];
        buf[o++] = digits[px[i] & 15u];
        if (o == sizeof(buf))
        {
            web_put(self, buf, lh_cast_static(lh_usize_t, o));
            o = 0;
        }
    }
    web_put(self, buf, lh_cast_static(lh_usize_t, o));
}

/** Start an op: the separating comma when one is due, and count it. */
static lh_void
web_comma(lh_ui_canvas_web_t *self)
{
    if (!self->ok)
    {
        return;
    }
    if (self->comma)
    {
        web_lit(self, ",");
    }
    self->comma = 1;
    ++self->ops;
}

/** Append @p color as `[r,g,b,a]`. */
static lh_void
web_color(lh_ui_canvas_web_t *self, const lh_ui_color_t *color)
{
    web_quad(self, lh_cast_static(long, lh_ui_color_get_r(color)), lh_cast_static(long, lh_ui_color_get_g(color)), lh_cast_static(long, lh_ui_color_get_b(color)),
             lh_cast_static(long, lh_ui_color_get_a(color)));
}

/** Append @p rect as `[x,y,w,h]`. */
static lh_void
web_rect(lh_ui_canvas_web_t *self, const lh_ui_rect_t *rect)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);

    web_quad(self, lh_cast_static(long, lh_ui_point_get_x(origin)), lh_cast_static(long, lh_ui_point_get_y(origin)),
             lh_cast_static(long, lh_ui_size_get_width(size)), lh_cast_static(long, lh_ui_size_get_height(size)));
}

/** Distinct glyph covers the process-wide table holds. */
#define WEB_ATLAS_MAX LH_UI_CANVAS_WEB_GLYPHS_MAX
/** Largest cover, in pixels, that is interned; bigger ones go inline every time. */
#define WEB_ATLAS_PIX 4096
/** Hash slots: twice the entries, a power of two. */
#define WEB_ATLAS_SLOTS (2u * WEB_ATLAS_MAX)

/** Every distinct glyph cover sent so far, by id, with a hash index over them. */
typedef struct web_atlas
{
    unsigned n;                    /**< covers held */
    int w[WEB_ATLAS_MAX];          /**< width of each cover */
    int h[WEB_ATLAS_MAX];          /**< height of each cover */
    unsigned len[WEB_ATLAS_MAX];   /**< bytes of each cover */
    unsigned hash[WEB_ATLAS_MAX];  /**< content hash of each cover */
    lh_byte_ptr px[WEB_ATLAS_MAX]; /**< the coverage bytes of each cover (owned) */
    int slot[WEB_ATLAS_SLOTS]; /**< open addressing by content hash: id + 1, 0 = empty */
} web_atlas_t; /**< Every distinct glyph cover sent so far. */

/** Cover identity is process-wide so `m` does not reshuffle when the tree
 * changes. `self->glyphs` is only which ids the reader already holds. */
static web_atlas_t g_web_atlas;

/** FNV-1a hash of a cover's size and pixels. */
static unsigned
web_atlas_hash(lh_byte_cptr px, unsigned n, int w, int h)
{
    unsigned x = 2166136261u ^ (lh_cast_static(unsigned, w)* 131u + lh_cast_static(unsigned, h));
    unsigned i;

    for (i = 0; i < n; ++i)
    {
        x = (x ^ px[i]) * 16777619u;
    }
    return x;
}

/** The id of these pixels, or -1 with @p slot the empty place they would go. */
static int
web_atlas_find(const web_atlas_t *atlas, lh_byte_cptr px, unsigned n, int w, int h, unsigned hash,
               unsigned *slot)
{
    unsigned s = hash & (WEB_ATLAS_SLOTS - 1u);

    for (;;)
    {
        const int id = atlas->slot[s] - 1;

        if (id < 0)
        {
            *slot = s;
            return -1;
        }
        if (atlas->hash[id] == hash && atlas->w[id] == w && atlas->h[id] == h && atlas->len[id] == n &&
            lh_memory_compare(atlas->px[id], n, lh_cast_const(lh_ptr, px), n) == lh_null)
        {
            return id;
        }
        s = (s + 1u) & (WEB_ATLAS_SLOTS - 1u);
    }
}

/** Copy a new cover into the table at hash @p slot; its id, or -1 when the table is full. */
static int
web_atlas_add(web_atlas_t *atlas, lh_byte_cptr px, unsigned n, int w, int h, unsigned hash,
              unsigned slot)
{
    const unsigned id = atlas->n;

    if (id >= WEB_ATLAS_MAX)
    {
        return -1;
    }
    atlas->px[id] = lh_ptr_rcast(lh_byte_t, lh_runtime_allocator_alloc(n));
    if (atlas->px[id] == lh_null)
    {
        return -1;
    }
    lh_memory_copy(atlas->px[id], n, lh_cast_const(lh_ptr, px), n);
    atlas->w[id] = w;
    atlas->h[id] = h;
    atlas->len[id] = n;
    atlas->hash[id] = hash;
    atlas->slot[slot] = lh_cast_static(int, id)+ 1;
    ++atlas->n;
    return (int)id;
}

/** Which covers the reader holds: the caller's set, or one owned by this frame. */
static lh_ui_canvas_web_glyphs_t *
web_glyphs(lh_ui_canvas_web_t *self)
{
    if (self->glyphs == lh_null)
    {
        self->glyphs = lh_ptr_rcast(lh_ui_canvas_web_glyphs_t, lh_runtime_allocator_alloc(sizeof(*self->glyphs)));
        self->glyphs_owned = (self->glyphs != lh_null) ? lh_bool_true : lh_bool_false;
        lh_ui_canvas_web_glyphs_reset(self->glyphs);
    }
    return self->glyphs;
}

/** Free the set ::web_glyphs made for this frame, if it made one. */
static lh_void
web_glyphs_free(lh_ui_canvas_web_t *self)
{
    if (self->glyphs_owned)
    {
        lh_runtime_allocator_free(self->glyphs);
    }
    self->glyphs = lh_null;
    self->glyphs_owned = lh_bool_false;
}

/** The id of @p px, and whether the reader still needs its cover. -1 when the
 * table is full: the caller sends the cover inline, every time. */
static int
web_atlas_intern(lh_ui_canvas_web_t *self, lh_byte_cptr px, int w, int h, int *is_new)
{
    const unsigned n = lh_cast_static(unsigned, w)* lh_cast_static(unsigned, h);
    const unsigned hash = web_atlas_hash(px, n, w, h);
    web_atlas_t *atlas = lh_addr_of(g_web_atlas);
    lh_ui_canvas_web_glyphs_t *glyphs;
    unsigned slot = 0;
    int id;

    *is_new = 1;
    id = web_atlas_find(atlas, px, n, w, h, hash, lh_addr_of(slot));
    if (id < 0)
    {
        id = web_atlas_add(atlas, px, n, w, h, hash, slot);
        if (id < 0)
        {
            return -1;
        }
    }
    glyphs = web_glyphs(self);
    if (glyphs == lh_null)
    {
        return id;
    }
    if (glyphs->sent[id] != 0u)
    {
        *is_new = 0;
    }
    glyphs->sent[id] = 1u;
    return id;
}

/** Empty @p self: no buffer, no ops, sized @p width x @p height. */
static lh_void
web_reset(lh_ui_canvas_web_t *self, unsigned width, unsigned height)
{
    self->mem = lh_null;
    self->cap = 0;
    self->len = 0;
    self->width = width;
    self->height = height;
    self->ops = 0;
    self->comma = 0;
    self->ok = lh_bool_false;
    self->glyphs = lh_null;
    self->glyphs_owned = lh_bool_false;
}

/** Append the frame header and open the `ops` array. */
static lh_void
web_head(lh_ui_canvas_web_t *self)
{
    web_lit(self, "{\"kind\":\"lh_ui_canvas\",\"width\":");
    web_int(self, lh_cast_static(long, self->width));
    web_lit(self, ",\"height\":");
    web_int(self, lh_cast_static(long, self->height));
    web_lit(self, ",\"ops\":[");
}

/** Backend `begin`: nothing to do, the frame is opened by ::lh_ui_canvas_web_open_mem. */
static lh_void
web_begin(lh_ptr context)
{
    (void)context;
}

/** Backend `end`: nothing to do, the frame is closed by ::lh_ui_canvas_web_close. */
static lh_void
web_end(lh_ptr context, const lh_ui_rects_t *drawn)
{
    (void)context;
    (void)drawn;
}

/** Backend `clear`: record a `clear` op. */
static lh_void
web_clear(lh_ptr context, const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);

    web_comma(self);
    web_lit(self, "{\"op\":\"clear\",\"color\":");
    web_color(self, color);
    web_lit(self, "}");
}

/** Backend `fill_rect`: record a `fill_rect` op. */
static lh_void
web_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);

    web_comma(self);
    web_lit(self, "{\"op\":\"fill_rect\",\"rect\":");
    web_rect(self, rect);
    web_lit(self, ",\"color\":");
    web_color(self, color);
    web_lit(self, "}");
}

/** Backend `fill_round_rect`: record a `fill_round_rect` op. */
static lh_bool_t
web_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                    const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);

    web_comma(self);
    web_lit(self, "{\"op\":\"fill_round_rect\",\"rect\":");
    web_rect(self, rect);
    web_lit(self, ",\"radius\":");
    web_int(self, lh_cast_static(long, radius));
    web_lit(self, ",\"color\":");
    web_color(self, color);
    web_lit(self, "}");
    return lh_bool_true;
}

/** Backend `set_clip`: ignored, the ops already arrive cut by the canvas. */
static lh_void
web_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip)
{
    (void)context;
    (void)clip;
}

/** Backend `shadow`: record a `shadow` op. */
static lh_bool_t
web_shadow(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, const lh_ui_shadow_t *shadow)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);
    lh_ui_color_t color;
    lh_ui_shadow_get_color(shadow, lh_addr_of(color));

    web_comma(self);
    web_lit(self, "{\"op\":\"shadow\",\"rect\":");
    web_rect(self, rect);
    web_lit(self, ",\"radius\":");
    web_int(self, lh_cast_static(long, radius));
    web_lit(self, ",\"spread\":");
    web_int(self, lh_cast_static(long, lh_ui_shadow_get_spread(shadow)));
    web_lit(self, ",\"offset\":[");
    web_int(self, lh_cast_static(long, lh_ui_shadow_get_offset_x(shadow)));
    web_lit(self, ",");
    web_int(self, lh_cast_static(long, lh_ui_shadow_get_offset_y(shadow)));
    web_lit(self, "],\"color\":");
    web_color(self, lh_addr_of(color));
    web_lit(self, "}");
    return lh_bool_true;
}

/** A mask too big for the intern table: its coverage goes inline, a row at a time. */
static lh_void
web_cover_inline(lh_ui_canvas_web_t *self, const lh_ui_mask_t *mask, int w, int h)
{
    lh_byte_t row[WEB_ATLAS_PIX];
    int x0;
    int y;

    for (y = 0; y < h; ++y)
    {
        for (x0 = 0; x0 < w; x0 += WEB_ATLAS_PIX)
        {
            const int x1 = (w - x0 > WEB_ATLAS_PIX) ? x0 + WEB_ATLAS_PIX : w;

            lh_ui_mask_coverage_run(mask, x0, x1, y, row);
            web_hex(self, row, lh_cast_static(unsigned, (x1 - x0)));
        }
    }
}

/** Backend `fill_mask`: a `fill_mask` op with an interned cover id, and the cover
 * when the reader lacks it. */
static lh_bool_t
web_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
              const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);
    const int w = lh_cast_static(int, lh_ui_mask_get_width(mask));
    const int h = lh_cast_static(int, lh_ui_mask_get_height(mask));
    const unsigned n = (w > 0 && h > 0) ? lh_cast_static(unsigned, w)* lh_cast_static(unsigned, h): 0u;
    lh_byte_t px[WEB_ATLAS_PIX];
    int is_new = 1;
    int id = -1;
    int y;

    web_comma(self);
    web_lit(self, "{\"op\":\"fill_mask\",\"origin\":[");
    web_int(self, lh_cast_static(long, lh_ui_point_get_x(origin)));
    web_lit(self, ",");
    web_int(self, lh_cast_static(long, lh_ui_point_get_y(origin)));
    web_lit(self, "],\"size\":[");
    web_int(self, lh_cast_static(long, w));
    web_lit(self, ",");
    web_int(self, lh_cast_static(long, h));
    web_lit(self, "],\"color\":");
    web_color(self, color);
    if (n == 0u || n > lh_cast_static(unsigned, WEB_ATLAS_PIX))
    {
        web_lit(self, ",\"cover\":\"");
        if (n != 0u)
        {
            web_cover_inline(self, mask, w, h);
        }
        web_lit(self, "\"}");
        return lh_bool_true;
    }
    for (y = 0; y < h; ++y)
    {
        lh_ui_mask_coverage_run(mask, 0, w, y, px + lh_cast_static(unsigned, y)* lh_cast_static(unsigned, w));
    }
    id = web_atlas_intern(self, px, w, h, lh_addr_of(is_new));
    if (id >= 0)
    {
        web_lit(self, ",\"m\":");
        web_int(self, lh_cast_static(long, id));
    }
    if (id < 0 || is_new)
    {
        web_lit(self, ",\"cover\":\"");
        web_hex(self, px, n);
        web_lit(self, "\"");
    }
    web_lit(self, "}");
    return lh_bool_true;
}

const lh_ui_canvas_backend_t lh_ui_canvas_backend_web = {
    /* begin */ web_begin,
    /* begin_area */ lh_null,
    /* end */ web_end,
    /* clear */ web_clear,
    /* fill_rect */ web_fill_rect,
    /* fill_round_rect */ web_fill_round_rect,
    /* set_clip */ web_set_clip,
    /* fill_mask */ web_fill_mask,
    /* shadow */ web_shadow,
    /* blur */ lh_null,
    /* glass */ lh_null,
};

lh_bool_t
lh_ui_canvas_web_open_mem(lh_ui_canvas_web_t *self, char *buf, lh_usize_t cap, unsigned width,
                          unsigned height)
{
    if (self == lh_null || buf == lh_null || cap < 64u)
    {
        return lh_bool_false;
    }
    lh_memory_set(self, sizeof(*self), 0);
    web_reset(self, width, height);
    self->mem = buf;
    self->cap = cap;
    self->ok = lh_bool_true;
    web_head(self);
    return self->ok;
}

lh_bool_t
lh_ui_canvas_web_close(lh_ui_canvas_web_t *self)
{
    lh_bool_t ok;

    if (self == lh_null)
    {
        return lh_bool_false;
    }
    ok = self->ok;
    if (ok)
    {
        web_lit(self, "]}");
        ok = self->ok;
    }
    if (self->mem != lh_null)
    {
        self->mem[self->len] = 0;
    }
    web_glyphs_free(self);
    self->ok = ok;
    return ok;
}

lh_void
lh_ui_canvas_web_glyphs_reset(lh_ui_canvas_web_glyphs_t *self)
{
    if (self != lh_null)
    {
        lh_memory_set(self->sent, sizeof(self->sent), 0);
    }
}

lh_void
lh_ui_canvas_web_set_glyphs(lh_ui_canvas_web_t *self, lh_ui_canvas_web_glyphs_t *glyphs)
{
    if (self == lh_null)
    {
        return;
    }
    web_glyphs_free(self);
    self->glyphs = glyphs;
}

const char *
lh_ui_canvas_web_bytes(const lh_ui_canvas_web_t *self)
{
    if (self == lh_null || self->mem == lh_null)
    {
        return "";
    }
    return self->mem;
}

lh_usize_t
lh_ui_canvas_web_length(const lh_ui_canvas_web_t *self)
{
    if (self == lh_null)
    {
        return 0;
    }
    return self->len;
}
