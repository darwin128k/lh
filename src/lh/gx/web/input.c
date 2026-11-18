/**
 * @file input.c
 * @brief Implementation of `lh/gx/web/input.h`.
 */

#include <lh/gx/web/input.h>

#include <lh/core/char/map.h>
#include <lh/core/str/parse/sllong.h>
#include <lh/core/str/split/next.h>
#include <lh/core/util/return.h>

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
    default:
        return lh_bool_false;
    }
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
    lh_gx_point_t at;

    lh_gx_point_init(lh_addr_of(at), 0, 0);
    if (kind == lh_gx_event_kind_cancel)
    {
        lh_return_if(more, lh_bool_false);
        lh_gx_event_init(out, kind, at);
        return lh_bool_true;
    }
    lh_gx_scalar_t xy[2];

    for (lh_usize_t i = 0U; i < 2U; ++i)
    {
        lh_return_if(!more, lh_bool_false);
        lh_return_if(!lh_str_ptr_split_next(text, size, lh_char_map_space, lh_addr_of(pos),
                                            lh_addr_of(field), lh_addr_of(field_size),
                                            lh_addr_of(more)),
                     lh_bool_false);
        lh_return_if(!lh_gx_web_input_try_parse_scalar(field, field_size, xy + i), lh_bool_false);
    }
    lh_return_if(more, lh_bool_false);
    lh_gx_point_init(lh_addr_of(at), xy[0], xy[1]);
    lh_gx_event_init(out, kind, at);
    return lh_bool_true;
}
