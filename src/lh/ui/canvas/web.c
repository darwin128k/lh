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
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

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
    web_write(self, ",\"cover\":\"");
    for (y = 0; y < h; ++y)
    {
        for (x = 0; x < w; ++x)
        {
            web_write(self, "%02x", (unsigned)lh_ui_mask_get_coverage(mask, x, y));
        }
    }
    web_write(self, "\"}");
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
    /* shadow */ lh_null,
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
