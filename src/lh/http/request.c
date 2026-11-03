#include <lh/http/request.h>
#include <lh/assert.h>
#include <lh/char/map.h>
#include <lh/http/method.h>
#include <lh/str/split/once.h>

const lh_str_view_t *
lh_http_request_get_method(const lh_http_request_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->method);
}

lh_void
lh_http_request_set_method(lh_http_request_t *self, lh_str_view_t method)
{
    lh_assert_runtime_ref(self);
    self->method = method;
}

const lh_str_view_t *
lh_http_request_get_target(const lh_http_request_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->target);
}

lh_void
lh_http_request_set_target(lh_http_request_t *self, lh_str_view_t target)
{
    lh_assert_runtime_ref(self);
    self->target = target;
}

lh_u8_t
lh_http_request_get_version_minor(const lh_http_request_t *self)
{
    lh_assert_runtime_ref(self);
    return self->version_minor;
}

lh_void
lh_http_request_set_version_minor(lh_http_request_t *self, lh_u8_t version_minor)
{
    lh_assert_runtime_ref(self);
    self->version_minor = version_minor;
}

lh_usize_t
lh_http_request_get_header_count(const lh_http_request_t *self)
{
    lh_assert_runtime_ref(self);
    return self->header_count;
}

lh_void
lh_http_request_set_header_count(lh_http_request_t *self, lh_usize_t header_count)
{
    lh_assert_runtime_ref(self);
    self->header_count = header_count;
}

lh_str_view_t
lh_http_request_get_path(const lh_http_request_t *self)
{
    const lh_str_view_t *target = lh_http_request_get_target(self);
    lh_str_view_t path;
    lh_str_view_t query;

    if (!lh_str_view_split_once(target, lh_char_map_question, lh_addr_of(path), lh_addr_of(query)))
    {
        lh_str_view_assign(lh_addr_of(path), target);
    }
    return path;
}

lh_usize_t
lh_http_request_get_query_value_size(const lh_http_request_t *self, lh_str_cptr key,
                                     lh_usize_t key_len, lh_str_cptr *value)
{
    if (lh_math_is_zero(key_len))
    {
        return 0U;
    }
    lh_str_view_t query;
    {
        lh_str_view_t path;
        if (!lh_str_view_split_once(lh_http_request_get_target(self), lh_char_map_question,
                                    lh_addr_of(path), lh_addr_of(query)))
        {
            return 0U;
        }
    }
    if (lh_str_view_is_empty(lh_addr_of(query)))
    {
        return 0U;
    }
    const lh_str_ptr text = lh_str_view_get_data(lh_addr_of(query));
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(query));
    lh_usize_t offset = 0U;

    for (;;)
    {
        const lh_str_ptr at =
            lh_str_ptr_find(text + offset, size - offset, key, key_len, lh_bool_false);
        if (lh_ptr_is_null(at))
        {
            return 0U;
        }
        offset = lh_math_add(lh_ptr_udiff(at, text), key_len);
        if (at != text && at[-1] != lh_char_map_ampersand)
        {
            continue;
        }
        lh_ptr_deref(value) = text + offset;
        const lh_str_ptr end =
            lh_str_ptr_find_of_char(text + offset, size - offset, lh_char_map_ampersand);
        return lh_null_ne(end) ? lh_ptr_udiff(end, text + offset) : size - offset;
    }
}

lh_bool_t
lh_http_request_is_method(const lh_http_request_t *self, lh_http_method_kind_t method)
{
    const lh_str_view_t *actual = lh_http_request_get_method(self);
    const lh_str_view_t name = lh_http_method_get_name(method);
    return !lh_str_view_is_empty(lh_addr_of(name)) &&
           lh_str_view_equals(actual, lh_addr_of(name), lh_bool_false);
}
