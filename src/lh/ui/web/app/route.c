/**
 * @file route.c
 * @brief Implementation of `lh/ui/web/app/route.h`.
 */

#include <lh/ui/web/app/route.h>


const lh_str_ptr
lh_ui_web_route_get_method(const lh_ui_web_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->method;
}

lh_void
lh_ui_web_route_set_method(lh_ui_web_route_t *self, const lh_str_ptr method)
{
    lh_assert_runtime_ref(self);
    self->method = method;
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
lh_ui_web_route_set_path(lh_ui_web_route_t *self, const lh_str_ptr path)
{
    lh_assert_runtime_ref(self);
    self->path = path;
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
lh_ui_web_route_is_match(const lh_ui_web_route_t *self, const lh_http_request_t *request)
{
    const lh_usize_t method_len = lh_ui_web_route_get_method_len(self);
    if (!lh_math_is_zero(method_len))
    {
        const lh_str_view_t method =
            lh_str_view_initializer(lh_ui_web_route_get_method(self), method_len);
        if (!lh_str_view_equals(lh_http_request_get_method(request), lh_addr_of(method),
                                lh_bool_false))
        {
            return lh_bool_false;
        }
    }
    const lh_str_view_t path = lh_http_request_get_path(request);
    const lh_usize_t path_len = lh_ui_web_route_get_path_len(self);
    if (lh_math_is_zero(path_len))
    {
        return lh_str_view_is_empty(lh_addr_of(path));
    }
    const lh_str_view_t expected =
        lh_str_view_initializer(lh_ui_web_route_get_path(self), path_len);
    return lh_str_view_equals(lh_addr_of(path), lh_addr_of(expected), lh_bool_false);
}
