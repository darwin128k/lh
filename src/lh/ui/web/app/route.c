/**
 * @file route.c
 * @brief Implementation of `lh/ui/web/app/route.h`.
 */

#include <lh/ui/web/app/route.h>

#include <lh/util/str/ptr.h>

const lh_str_ptr
lh_ui_web_route_get_method(const lh_ui_web_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->method;
}

lh_void
lh_ui_web_route_set_method(lh_ui_web_route_t *self, const lh_str_ptr method, lh_usize_t method_len)
{
    lh_assert_runtime_ref(self);
    self->method = method;
    self->method_len = method_len;
}

lh_usize_t
lh_ui_web_route_get_method_len(const lh_ui_web_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->method_len;
}

lh_void
lh_ui_web_route_set_method_len(lh_ui_web_route_t *self, lh_usize_t method_len)
{
    lh_assert_runtime_ref(self);
    self->method_len = method_len;
}

const lh_str_ptr
lh_ui_web_route_get_path(const lh_ui_web_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->path;
}

lh_void
lh_ui_web_route_set_path(lh_ui_web_route_t *self, const lh_str_ptr path, lh_usize_t path_len)
{
    lh_assert_runtime_ref(self);
    self->path = path;
    self->path_len = path_len;
}

lh_usize_t
lh_ui_web_route_get_path_len(const lh_ui_web_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->path_len;
}

lh_void
lh_ui_web_route_set_path_len(lh_ui_web_route_t *self, lh_usize_t path_len)
{
    lh_assert_runtime_ref(self);
    self->path_len = path_len;
}

lh_ui_web_route_fn
lh_ui_web_route_get_handler(const lh_ui_web_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handler;
}

lh_void
lh_ui_web_route_set_handler(lh_ui_web_route_t *self, lh_ui_web_route_fn handler)
{
    lh_assert_runtime_ref(self);
    self->handler = handler;
}

lh_bool_t
lh_ui_web_route_match(const lh_ui_web_route_t *self, const lh_str_ptr req, lh_usize_t len)
{
    lh_usize_t off = 0u;

    if (lh_ui_web_route_get_method_len(self) > 0u)
    {
        const lh_usize_t method_len = lh_ui_web_route_get_method_len(self);

        if (!lh_str_ptr_starts_with_by_size(req, len, lh_ui_web_route_get_method(self), method_len,
                                           lh_bool_false))
        {
            return lh_bool_false;
        }
        off = method_len;
        if (off >= len || req[off] != ' ')
        {
            return lh_bool_false;
        }
        ++off;
    }
    {
        const lh_usize_t path_len = lh_ui_web_route_get_path_len(self);

        if (!lh_str_ptr_starts_with_by_size(req + off, len - off, lh_ui_web_route_get_path(self),
                                            path_len, lh_bool_false))
        {
            return lh_bool_false;
        }
        off += path_len;
    }
    /* The path ends at the space before the HTTP version, at the `?` of the
       query, or at the end of the request. */
    return (off >= len || req[off] == ' ' || req[off] == '?') ? lh_bool_true : lh_bool_false;
}
