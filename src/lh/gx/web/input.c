/**
 * @file input.c
 * @brief Implementation of `lh/gx/web/input.h`.
 */

#include <lh/gx/web/input.h>

#include <lh/core/char/map.h>
#include <lh/core/str/parse/sllong.h>
#include <lh/core/str/parse/uint.h>
#include <lh/core/str/split/next.h>
#include <lh/core/util/return.h>

/* The browser's `KeyboardEvent.key` of every ::lh_gx_key_t, in the enum's
   order; the first, ::lh_gx_key_none, has no name. */
static const lh_char_t *const m_gx_web_input_key_names[] = {
    lh_null, "ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown", "Home",
    "End",   "Backspace", "Delete",     "Enter",   "Escape",    "Tab"};

lh_bool_t
lh_gx_web_input_try_get_kind(lh_char_t code, lh_gx_event_kind_t *out)
{
    switch (code)
    {
    case lh_char_map_small_p:
        lh_ptr_deref(out) = lh_gx_event_kind_press;
        return lh_bool_true;
    case lh_char_map_small_m:
        lh_ptr_deref(out) = lh_gx_event_kind_move;
        return lh_bool_true;
    case lh_char_map_small_r:
        lh_ptr_deref(out) = lh_gx_event_kind_release;
        return lh_bool_true;
    case lh_char_map_small_c:
        lh_ptr_deref(out) = lh_gx_event_kind_cancel;
        return lh_bool_true;
    case lh_char_map_small_k:
        lh_ptr_deref(out) = lh_gx_event_kind_key;
        return lh_bool_true;
    case lh_char_map_small_t:
        lh_ptr_deref(out) = lh_gx_event_kind_text;
        return lh_bool_true;
    default:
        return lh_bool_false;
    }
}

lh_bool_t
lh_gx_web_input_try_get_key(const lh_char_t *name, lh_usize_t size, lh_gx_key_t *out)
{
    lh_str_view_t given;

    lh_str_view_init_by_size(lh_addr_of(given), name, size);
    for (lh_usize_t i = 1U; i < lh_array_ptr_get_size(m_gx_web_input_key_names); ++i)
    {
        lh_str_view_t known;

        lh_str_view_init(lh_addr_of(known), m_gx_web_input_key_names[i]);
        if (lh_str_view_equals(lh_addr_of(given), lh_addr_of(known), lh_bool_false))
        {
            lh_ptr_deref(out) = lh_cast_static(lh_gx_key_t, i);
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_bool_t
lh_gx_web_input_try_parse_scalar(const lh_char_t *text, lh_usize_t size, lh_gx_scalar_t *out)
{
    lh_sllong_t value = 0;

    lh_return_if(
        !lh_str_ptr_parse_sllong(text, size, LH_S32_T_MIN, LH_S32_T_MAX, lh_addr_of(value)),
        lh_bool_false);
    lh_ptr_deref(out) = lh_gx_scalar_of(value);
    return lh_bool_true;
}

lh_bool_t
lh_gx_web_input_try_parse_code(const lh_char_t *text, lh_usize_t size, lh_u32_t *out)
{
    lh_uint_t value = 0U;

    lh_return_if(!lh_str_ptr_parse_uint(text, size, 0x10FFFFU, lh_addr_of(value)), lh_bool_false);
    lh_return_if(!lh_gx_event_is_text_code(lh_cast_static(lh_u32_t, value)), lh_bool_false);
    lh_ptr_deref(out) = lh_cast_static(lh_u32_t, value);
    return lh_bool_true;
}

lh_bool_t
lh_gx_web_input_try_parse(const lh_char_t *text, lh_usize_t size, lh_gx_event_t *out)
{
    lh_usize_t pos = 0U;
    const lh_char_t *field = lh_null;
    lh_usize_t field_size = 0U;
    lh_bool_t more = lh_bool_false;
    lh_gx_event_kind_t kind = lh_gx_event_kind_cancel;

    lh_return_if(size == 0U, lh_bool_false);
    lh_return_if(!lh_str_ptr_split_next(text, size, lh_char_map_space, lh_addr_of(pos),
                                        lh_addr_of(field), lh_addr_of(field_size),
                                        lh_addr_of(more)),
                 lh_bool_false);
    lh_return_if(field_size != 1U || !lh_gx_web_input_try_get_kind(field[0], lh_addr_of(kind)),
                 lh_bool_false);
    if (kind == lh_gx_event_kind_cancel)
    {
        lh_return_if(more, lh_bool_false);
        lh_gx_event_init_kind(out, kind);
        return lh_bool_true;
    }
    /* A key and a text have one field more; the pointer, two. */
    const lh_usize_t count =
        (kind == lh_gx_event_kind_key || kind == lh_gx_event_kind_text) ? 1U : 2U;
    const lh_char_t *fields[2];
    lh_usize_t sizes[2];

    for (lh_usize_t i = 0U; i < count; ++i)
    {
        lh_return_if(!more, lh_bool_false);
        lh_return_if(!lh_str_ptr_split_next(text, size, lh_char_map_space, lh_addr_of(pos),
                                            fields + i, sizes + i, lh_addr_of(more)),
                     lh_bool_false);
    }
    lh_return_if(more, lh_bool_false);
    if (kind == lh_gx_event_kind_key)
    {
        lh_gx_key_t key = lh_gx_key_none;

        lh_return_if(!lh_gx_web_input_try_get_key(fields[0], sizes[0], lh_addr_of(key)),
                     lh_bool_false);
        lh_gx_event_init_key(out, key);
        return lh_bool_true;
    }
    if (kind == lh_gx_event_kind_text)
    {
        lh_u32_t code = 0U;

        lh_return_if(!lh_gx_web_input_try_parse_code(fields[0], sizes[0], lh_addr_of(code)),
                     lh_bool_false);
        lh_gx_event_init_text(out, code);
        return lh_bool_true;
    }
    lh_gx_scalar_t xy[2];

    for (lh_usize_t i = 0U; i < 2U; ++i)
    {
        lh_return_if(!lh_gx_web_input_try_parse_scalar(fields[i], sizes[i], xy + i), lh_bool_false);
    }
    lh_gx_point_t at;

    lh_gx_point_init(lh_addr_of(at), xy[0], xy[1]);
    lh_gx_event_init(out, kind, at);
    return lh_bool_true;
}
