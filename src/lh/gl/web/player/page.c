/**
 * @file page.c
 * @brief Implementation of `lh/gl/web/player/page.h`: the document and the script assembled from
 * their parts, and writing them.
 */

#include <lh/gl/web/player/page.h>

#include <lh/core/assert.h>

#include <lh/core/web/xml/writer.h>
#include <lh/core/web/js/writer.h>

lh_web_xml_node_t *
lh_gl_web_player_page_create_head(lh_void)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create_element(lh_str_view_lit("head"));

    lh_web_xml_node_push_child(
        self, lh_web_xml_node_add_attr(lh_web_xml_node_create_empty(lh_str_view_lit("meta")),
                                       lh_str_view_lit("charset"), lh_str_view_lit("utf-8")));
    lh_web_xml_node_push_child(
        self,
        lh_web_xml_node_add_attr(
            lh_web_xml_node_add_attr(lh_web_xml_node_create_empty(lh_str_view_lit("meta")),
                                     lh_str_view_lit("name"), lh_str_view_lit("viewport")),
            lh_str_view_lit("content"), lh_str_view_lit("width=device-width,initial-scale=1")));
    lh_web_xml_node_push_child(
        self, lh_web_xml_node_push_child(lh_web_xml_node_create_element(lh_str_view_lit("title")),
                                         lh_web_xml_node_create_text(lh_str_view_lit("lh_ui"))));
    lh_web_xml_node_push_child(
        self, lh_web_xml_node_push_child(
                  lh_web_xml_node_create_element(lh_str_view_lit("style")),
                  lh_web_xml_node_create_raw(lh_str_view_lit(
                      "html,body{margin:0;height:100%;background:#edf0f5;overflow:hidden}"
                      "#c{display:block;position:absolute;cursor:pointer;outline:none}"
                      "#v3d{position:absolute;display:none;pointer-events:none}"))));
    return self;
}

lh_web_xml_node_t *
lh_gl_web_player_page_create_body(lh_str_view_t script)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create_element(lh_str_view_lit("body"));
    lh_web_xml_node_t *canvas = lh_web_xml_node_create_element(lh_str_view_lit("canvas"));

    lh_web_xml_node_add_attr(canvas, lh_str_view_lit("id"), lh_str_view_lit("c"));
    lh_web_xml_node_add_attr(canvas, lh_str_view_lit("width"), lh_str_view_lit("980"));
    lh_web_xml_node_add_attr(canvas, lh_str_view_lit("height"), lh_str_view_lit("620"));
    lh_web_xml_node_add_attr(canvas, lh_str_view_lit("tabindex"), lh_str_view_lit("0"));
    lh_web_xml_node_push_child(self, canvas);
    lh_web_xml_node_push_child(
        self, lh_web_xml_node_add_attr(lh_web_xml_node_create_element(lh_str_view_lit("canvas")),
                                       lh_str_view_lit("id"), lh_str_view_lit("v3d")));
    /* The script is a raw child: ::lh_web_js_writer_try_write escapes every `<` a string holds
       and keeps `</` and `<!` out of code, so the text cannot close the element early. */
    lh_web_xml_node_push_child(
        self,
        lh_web_xml_node_push_child(
            lh_web_xml_node_push_child(lh_web_xml_node_create_element(lh_str_view_lit("script")),
                                       lh_web_xml_node_create_text(lh_str_view_lit("\n"))),
            lh_web_xml_node_create_raw(script)));
    return self;
}

lh_web_js_node_t *
lh_gl_web_player_page_create_script(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_program);

    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_rgba());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_state());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_scale_mode());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_hexv());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_decode_cover());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_ingest());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_tint());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_glyph());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_round());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_pane3d());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_fit());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_play());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_sized());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_show());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_pos());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_view_size());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_report_size());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_on_resize());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_fetch_state());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_load());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_poll());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_emit());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_canvas());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_pointer_state());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_flush_move());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_on_mouse_down());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_on_mouse_up());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_on_mouse_move());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_on_wheel());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_keys());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_mods());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_on_key_down());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_on_key_up());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_focus());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_socket_state());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_stop_poll());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_start_poll());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_arm_retry());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_open_ws());
    lh_web_js_node_push_statement(self, lh_gl_web_player_page_create_start());
    return self;
}

lh_web_xml_node_t *
lh_gl_web_player_page_create_html(lh_str_view_t script)
{
    lh_web_xml_node_t *self = lh_web_xml_node_create_element(lh_str_view_lit("html"));

    lh_web_xml_node_add_attr(self, lh_str_view_lit("lang"), lh_str_view_lit("ru"));
    lh_web_xml_node_push_child(self, lh_gl_web_player_page_create_head());
    return lh_web_xml_node_push_child(self, lh_gl_web_player_page_create_body(script));
}

lh_bool_t
lh_gl_web_player_page_try_write_script(lh_str_t *out)
{
    lh_web_js_node_t *script = lh_gl_web_player_page_create_script();

    lh_web_js_writer_t writer;
    lh_web_js_writer_init(lh_addr_of(writer), out);
    lh_bool_t written = lh_web_js_writer_try_write(lh_addr_of(writer), script);
    lh_web_js_node_destroy(script);
    return written;
}

lh_bool_t
lh_gl_web_player_page_try_write_document(lh_str_t *out, lh_str_view_t script)
{
    lh_web_xml_node_t *document = lh_web_xml_node_create_document();

    lh_web_xml_node_push_child(document, lh_web_xml_node_create_doctype(lh_str_view_lit("html")));
    lh_web_xml_node_push_child(document, lh_gl_web_player_page_create_html(script));
    lh_web_xml_writer_t writer;
    lh_web_xml_writer_init(lh_addr_of(writer), out);
    lh_web_xml_writer_set_dialect(lh_addr_of(writer), lh_web_xml_dialect_html);

    const lh_bool_t written = lh_web_xml_writer_try_write(lh_addr_of(writer), document);

    lh_web_xml_node_destroy(document);
    return written;
}

lh_bool_t
lh_gl_web_player_page_try_write(lh_str_t *out)
{
    lh_str_t script;

    lh_str_init(lh_addr_of(script));
    lh_bool_t written = lh_gl_web_player_page_try_write_script(lh_addr_of(script));
    if (written)
    {
        lh_str_view_t view;

        lh_str_as_view(lh_addr_of(script), lh_addr_of(view));
        written = lh_gl_web_player_page_try_write_document(out, view);
    }
    lh_str_deinit(lh_addr_of(script));
    return written;
}

lh_void
lh_gl_web_player_page_write(lh_str_t *out)
{
    lh_runtime_check_ifn(lh_gl_web_player_page_try_write(out),
                         lh_runtime_error_code_invalid_argument, "player page cannot be written");
}

lh_bool_t
lh_gl_web_player_page_build(lh_byte_ptr mem, lh_usize_t cap, lh_usize_t *out_len)
{
    if (lh_null_ne(out_len))
    {
        lh_ptr_deref(out_len) = 0U;
    }
    if (lh_null_eq(mem))
    {
        return lh_bool_false;
    }
    lh_str_t page;

    lh_str_init(lh_addr_of(page));
    /* The bytes and the terminator after them must fit. */
    const lh_bool_t built =
        lh_cast_static(lh_bool_t, lh_gl_web_player_page_try_write(lh_addr_of(page)) &&
                                      lh_math_lt(lh_str_get_size(lh_addr_of(page)), cap));
    if (built)
    {
        lh_str_ptr_copy_cstr(lh_ptr_rcast(lh_char_t, mem), cap, lh_str_get_data(lh_addr_of(page)),
                             lh_str_get_size(lh_addr_of(page)));
        if (lh_null_ne(out_len))
        {
            lh_ptr_deref(out_len) = lh_str_get_size(lh_addr_of(page));
        }
    }
    lh_str_deinit(lh_addr_of(page));
    return built;
}
