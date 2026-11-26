/**
 * @file keys.c
 * @brief Implementation of `lh/gl/web/player/page.h`: the keys: their browser names, which are
 * modifiers, and the tables the script reads.
 */

#include <lh/gl/web/player/page.h>

#include <lh/core/assert.h>

/** The browser's `KeyboardEvent.key` for every ::lh_key_t from ::lh_key_tab on, in the
   enum's order; a key added to the enum without a name here stops the build below. */
static const lh_str_cptr m_ui_web_player_page_key_names[] = {
    "Tab",       "Enter",      "Escape",  "Backspace", "Delete", " ",
    "ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown", "Home",   "End",
    "PageUp",    "PageDown",   "Shift",   "Control",   "Alt"};

lh_assert_static(lh_array_ptr_get_size(m_ui_web_player_page_key_names) ==
                     lh_cast_static(lh_usize_t, lh_key_alt),
                 "m_ui_web_player_page_key_names must name every lh_key_t but lh_key_other");

lh_str_view_t
lh_gl_web_player_page_get_key_name(lh_key_t key)
{
    lh_str_view_t name;

    if (lh_math_eq(key, lh_key_other) || lh_math_gt(key, lh_key_alt))
    {
        lh_str_view_init_empty(lh_addr_of(name));
        return name;
    }
    lh_str_view_init(lh_addr_of(name), m_ui_web_player_page_key_names[lh_math_sub_one(key)]);
    return name;
}

lh_bool_t
lh_gl_web_player_page_is_modifier(lh_key_t key)
{
    return lh_cast_static(lh_bool_t, lh_math_ge(key, lh_key_shift) && !lh_math_gt(key, lh_key_alt));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_key_table(lh_bool_t modifiers_only)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_object);

    for (lh_key_t key = lh_key_tab; !lh_math_gt(key, lh_key_alt);
         key = lh_cast_static(lh_key_t, lh_math_add_one(key)))
    {
        if (modifiers_only && !lh_gl_web_player_page_is_modifier(key))
        {
            continue;
        }
        lh_web_js_node_push_back(
            self, lh_web_js_node_create_property(lh_gl_web_player_page_get_key_name(key),
                                                 lh_web_js_node_create_number(key)));
    }
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_keys(lh_void)
{
    return lh_web_js_node_create_var(lh_str_view_lit("keys"),
                                     lh_gl_web_player_page_create_key_table(lh_bool_false));
}

lh_web_js_node_t *
lh_gl_web_player_page_create_mods(lh_void)
{
    return lh_web_js_node_create_var(lh_str_view_lit("mods"),
                                     lh_gl_web_player_page_create_key_table(lh_bool_true));
}
