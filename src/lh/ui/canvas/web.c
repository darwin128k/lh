/**
 * @file web.c
 * @brief Implementation of `lh/ui/canvas/web.h`.
 */

#include <lh/ui/canvas/web.h>

#include <lh/null.h>
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

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
web_write(lh_ui_canvas_web_t *self, const char *fmt, ...)
{
    va_list ap;
    int n;

    if (self->out == lh_null && self->mem == lh_null)
    {
        self->ok = lh_bool_false;
        return;
    }
    if (!self->ok)
    {
        return;
    }
    va_start(ap, fmt);
    if (self->out != lh_null)
    {
        n = vfprintf(self->out, fmt, ap);
        if (n < 0)
        {
            self->ok = lh_bool_false;
        }
    }
    else
    {
        if (self->len >= self->cap)
        {
            self->ok = lh_bool_false;
        }
        else
        {
            n = vsnprintf(self->mem + self->len, (size_t)(self->cap - self->len), fmt, ap);
            if (n < 0 || (lh_usize_t)n >= self->cap - self->len)
            {
                self->ok = lh_bool_false;
            }
            else
            {
                self->len += (lh_usize_t)n;
            }
        }
    }
    va_end(ap);
}

static void
web_comma(lh_ui_canvas_web_t *self)
{
    if (!self->ok)
    {
        return;
    }
    if (self->comma)
    {
        web_write(self, ",");
    }
    self->comma = 1;
    ++self->ops;
}

static void
web_color(lh_ui_canvas_web_t *self, const lh_ui_color_t *color)
{
    web_write(self, "[%u,%u,%u,%u]", (unsigned)lh_ui_color_get_r(color),
              (unsigned)lh_ui_color_get_g(color), (unsigned)lh_ui_color_get_b(color),
              (unsigned)lh_ui_color_get_a(color));
}

static void
web_rect(lh_ui_canvas_web_t *self, const lh_ui_rect_t *rect)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);

    web_write(self, "[%d,%d,%d,%d]", (int)lh_ui_point_get_x(origin), (int)lh_ui_point_get_y(origin),
              (int)lh_ui_size_get_width(size), (int)lh_ui_size_get_height(size));
}

#define WEB_ATLAS_MAX 256
#define WEB_ATLAS_PIX 4096

typedef struct web_atlas
{
    unsigned n;
    int w[WEB_ATLAS_MAX];
    int h[WEB_ATLAS_MAX];
    unsigned len[WEB_ATLAS_MAX];
    unsigned char *px[WEB_ATLAS_MAX];
} web_atlas_t;

typedef struct web_frame
{
    unsigned char sent[WEB_ATLAS_MAX];
} web_frame_t;

/* Cover identity is process-wide so `m` does not reshuffle when the tree
 * changes. `self->atlas` is only which ids this JSON already carried. */
static web_atlas_t g_web_atlas;

static void
web_atlas_free(lh_ui_canvas_web_t *self)
{
    free(self->atlas);
    self->atlas = lh_null;
}

static int
web_atlas_intern(lh_ui_canvas_web_t *self, const lh_ui_mask_t *mask, int w, int h, int *is_new)
{
    unsigned char tmp[WEB_ATLAS_PIX];
    const unsigned n = (unsigned)w * (unsigned)h;
    web_atlas_t *atlas = lh_addr_of(g_web_atlas);
    web_frame_t *frame;
    unsigned i;
    int y;
    int id = -1;

    *is_new = 0;
    if (w <= 0 || h <= 0 || n > (unsigned)WEB_ATLAS_PIX)
    {
        return -1;
    }
    for (y = 0; y < h; ++y)
    {
        lh_ui_mask_coverage_run(mask, 0, w, y, tmp + (unsigned)y * (unsigned)w);
    }
    for (i = 0; i < atlas->n; ++i)
    {
        if (atlas->w[i] == w && atlas->h[i] == h && atlas->len[i] == n &&
            memcmp(atlas->px[i], tmp, (size_t)n) == 0)
        {
            id = (int)i;
            break;
        }
    }
    if (id < 0)
    {
        if (atlas->n >= WEB_ATLAS_MAX)
        {
            return -1;
        }
        atlas->px[atlas->n] = (unsigned char *)malloc((size_t)n);
        if (atlas->px[atlas->n] == lh_null)
        {
            return -1;
        }
        memcpy(atlas->px[atlas->n], tmp, (size_t)n);
        atlas->w[atlas->n] = w;
        atlas->h[atlas->n] = h;
        atlas->len[atlas->n] = n;
        id = (int)atlas->n++;
    }
    frame = (web_frame_t *)self->atlas;
    if (frame == lh_null)
    {
        frame = (web_frame_t *)calloc(1, sizeof(*frame));
        if (frame == lh_null)
        {
            return id;
        }
        self->atlas = frame;
    }
    if (frame->sent[id] == 0u)
    {
        frame->sent[id] = 1u;
        *is_new = 1;
    }
    return id;
}

static lh_void
web_reset(lh_ui_canvas_web_t *self, unsigned width, unsigned height)
{
    self->out = lh_null;
    self->mem = lh_null;
    self->cap = 0;
    self->len = 0;
    self->width = width;
    self->height = height;
    self->ops = 0;
    self->comma = 0;
    self->ok = lh_bool_false;
    self->atlas = lh_null;
}

static lh_void
web_begin(lh_ptr context)
{
    (void)context;
}

static lh_void
web_end(lh_ptr context, const lh_ui_rects_t *drawn)
{
    (void)context;
    (void)drawn;
}

static lh_void
web_clear(lh_ptr context, const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);

    web_comma(self);
    web_write(self, "{\"op\":\"clear\",\"color\":");
    web_color(self, color);
    web_write(self, "}");
}

static lh_void
web_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);

    web_comma(self);
    web_write(self, "{\"op\":\"fill_rect\",\"rect\":");
    web_rect(self, rect);
    web_write(self, ",\"color\":");
    web_color(self, color);
    web_write(self, "}");
}

static lh_bool_t
web_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                    const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);

    web_comma(self);
    web_write(self, "{\"op\":\"fill_round_rect\",\"rect\":");
    web_rect(self, rect);
    web_write(self, ",\"radius\":%d,\"color\":", (int)radius);
    web_color(self, color);
    web_write(self, "}");
    return lh_bool_true;
}

static lh_void
web_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip)
{
    (void)context;
    (void)clip;
}

static lh_bool_t
web_shadow(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, const lh_ui_shadow_t *shadow)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);
    const lh_ui_color_t color = lh_ui_shadow_get_color(shadow);

    web_comma(self);
    web_write(self, "{\"op\":\"shadow\",\"rect\":");
    web_rect(self, rect);
    web_write(self, ",\"radius\":%d,\"spread\":%d,\"offset\":[%d,%d],\"color\":", (int)radius,
              (int)lh_ui_shadow_get_spread(shadow), (int)lh_ui_shadow_get_offset_x(shadow),
              (int)lh_ui_shadow_get_offset_y(shadow));
    web_color(self, lh_addr_of(color));
    web_write(self, "}");
    return lh_bool_true;
}

static lh_bool_t
web_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
              const lh_ui_color_t *color)
{
    lh_ui_canvas_web_t *self = lh_ptr_rcast(lh_ui_canvas_web_t, context);
    const int w = (int)lh_ui_mask_get_width(mask);
    const int h = (int)lh_ui_mask_get_height(mask);
    int y;
    int x;

    web_comma(self);
    web_write(self, "{\"op\":\"fill_mask\",\"origin\":[%d,%d],\"size\":[%d,%d],\"color\":",
              (int)lh_ui_point_get_x(origin), (int)lh_ui_point_get_y(origin), w, h);
    web_color(self, color);
    {
        int is_new = 0;
        const int id = web_atlas_intern(self, mask, w, h, lh_addr_of(is_new));

        if (id >= 0)
        {
            web_write(self, ",\"m\":%d", id);
        }
        if (id < 0 || is_new)
        {
            web_write(self, ",\"cover\":\"");
            for (y = 0; y < h; ++y)
            {
                for (x = 0; x < w; ++x)
                {
                    web_write(self, "%02x", (unsigned)lh_ui_mask_get_coverage(mask, x, y));
                }
            }
            web_write(self, "\"");
        }
    }
    web_write(self, "}");
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
lh_ui_canvas_web_open(lh_ui_canvas_web_t *self, const char *path, unsigned width, unsigned height)
{
    if (self == lh_null)
    {
        return lh_bool_false;
    }
    memset(self, 0, sizeof(*self));
    web_reset(self, width, height);
    if (path == lh_null || path[0] == '\0')
    {
        return lh_bool_false;
    }
    self->out = fopen(path, "wb");
    if (self->out == lh_null)
    {
        return lh_bool_false;
    }
    self->ok = lh_bool_true;
    web_write(self, "{\"kind\":\"lh_ui_canvas\",\"width\":%u,\"height\":%u,\"ops\":[", width,
              height);
    return self->ok;
}

lh_bool_t
lh_ui_canvas_web_open_mem(lh_ui_canvas_web_t *self, char *buf, lh_usize_t cap, unsigned width,
                          unsigned height)
{
    if (self == lh_null || buf == lh_null || cap < 64u)
    {
        return lh_bool_false;
    }
    memset(self, 0, sizeof(*self));
    web_reset(self, width, height);
    self->mem = buf;
    self->cap = cap;
    self->ok = lh_bool_true;
    web_write(self, "{\"kind\":\"lh_ui_canvas\",\"width\":%u,\"height\":%u,\"ops\":[", width,
              height);
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
        web_write(self, "]}");
        ok = self->ok;
    }
    if (self->out != lh_null)
    {
        if (fclose(self->out) != 0)
        {
            ok = lh_bool_false;
        }
        self->out = lh_null;
    }
    web_atlas_free(self);
    self->ok = ok;
    return ok;
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
