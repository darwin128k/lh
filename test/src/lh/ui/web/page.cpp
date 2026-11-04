/**
 * @file page.cpp
 * @brief The player page: every script part, the key table from the enum, the document
 *        around the script, and the whole page as the player serves it.
 */

#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/os/key.h>
#include <lh/ui/web/player/page.h>
#include <lh/core/web/html/reader.h>
#include <lh/core/web/js/writer.h>
#include <lh/core/web/xml/writer.h>

#include <string>
#include <vector>

namespace
{

std::string
text_of(const lh_str_t *text)
{
    return std::string(lh_str_get_data(text) ? lh_str_get_data(text) : "", lh_str_get_size(text));
}

/* @p node written as JavaScript, then freed. */
std::string
js(lh_web_js_node_t *node)
{
    lh_web_js_writer_t writer;
    lh_str_t out;

    lh_str_init(&out);
    lh_web_js_writer_init(&writer, &out);
    EXPECT_TRUE(lh_web_js_writer_try_write(&writer, node));
    std::string text = text_of(&out);
    lh_str_deinit(&out);
    lh_web_js_node_destroy(node);
    return text;
}

/* The expression @p node written alone, then freed. */
std::string
js_expression(lh_web_js_node_t *node)
{
    lh_web_js_writer_t writer;
    lh_str_t out;

    lh_str_init(&out);
    lh_web_js_writer_init(&writer, &out);
    EXPECT_TRUE(lh_web_js_writer_try_write_expression(&writer, node, lh_web_js_precedence_lowest));
    std::string text = text_of(&out);
    lh_str_deinit(&out);
    lh_web_js_node_destroy(node);
    return text;
}

/* The player as text, built into as much room as the host gives it (48 KB). */
std::string
page_text()
{
    std::vector<lh_byte_t> mem(48u * 1024u);
    lh_usize_t len = 0u;

    EXPECT_EQ(lh_ui_web_player_page_build(mem.data(), mem.size(), lh_addr_of(len)), lh_bool_true);
    return std::string(reinterpret_cast<const char *>(mem.data()), len);
}

/* The slice of @p page from @p from up to @p to. */
std::string
between(const std::string &page, const std::string &from, const std::string &to)
{
    const std::string::size_type start = page.find(from);
    const std::string::size_type end = page.find(to, start);

    EXPECT_NE(start, std::string::npos) << from;
    EXPECT_NE(end, std::string::npos) << to;
    return page.substr(start, end - start);
}

/* One script part and the first line it writes. */
struct part
{
    lh_web_js_node_t *(*create)(lh_void);
    const char *first_line;
};

const part m_parts[] = {
    {lh_ui_web_player_page_create_rgba, "function rgba(c){"},
    {lh_ui_web_player_page_create_state,
     "var atlas=[],tints={},ntint=0,fw=980,fh=620,cssS=1,lastD=null,pend=null,raf=0,v3on=0;"},
    {lh_ui_web_player_page_create_scale_mode,
     "var scaleMode=/scale=fit/.test(location.search)?'fit':'layout',lastZW=0,lastZH=0;"},
    {lh_ui_web_player_page_create_hexv, "function hexv(c){"},
    {lh_ui_web_player_page_create_decode_cover, "function decodeCover(w,h,cv){"},
    {lh_ui_web_player_page_create_ingest, "function ingest(d){"},
    {lh_ui_web_player_page_create_tint, "function tint(a,c){"},
    {lh_ui_web_player_page_create_glyph, "function glyph(op){"},
    {lh_ui_web_player_page_create_round, "function round(x,r,rad){"},
    {lh_ui_web_player_page_create_pane3d, "function pane3d(d){"},
    {lh_ui_web_player_page_create_fit, "function fit(){"},
    {lh_ui_web_player_page_create_play, "function play(d){"},
    {lh_ui_web_player_page_create_sized, "function sized(d){"},
    {lh_ui_web_player_page_create_show, "function show(d){"},
    {lh_ui_web_player_page_create_pos, "function pos(e){"},
    {lh_ui_web_player_page_create_view_size, "function viewSize(){"},
    {lh_ui_web_player_page_create_report_size, "function reportSize(force){"},
    {lh_ui_web_player_page_create_on_resize, "window.onresize=function(){"},
    {lh_ui_web_player_page_create_fetch_state, "var busy=0,rev=-1;"},
    {lh_ui_web_player_page_create_load, "function load(){"},
    {lh_ui_web_player_page_create_poll, "function poll(){"},
    {lh_ui_web_player_page_create_emit, "function emit(line){"},
    {lh_ui_web_player_page_create_canvas, "var c=document.getElementById('c');"},
    {lh_ui_web_player_page_create_pointer_state, "var held=0,mv=null,mvRaf=0;"},
    {lh_ui_web_player_page_create_flush_move, "function flushMove(){"},
    {lh_ui_web_player_page_create_on_mouse_down, "c.onmousedown=function(e){"},
    {lh_ui_web_player_page_create_on_mouse_up, "window.onmouseup=function(e){"},
    {lh_ui_web_player_page_create_on_mouse_move, "window.onmousemove=function(e){"},
    {lh_ui_web_player_page_create_on_wheel, "c.onwheel=function(e){"},
    {lh_ui_web_player_page_create_on_key_down, "document.onkeydown=function(e){"},
    {lh_ui_web_player_page_create_on_key_up, "document.onkeyup=function(e){"},
    {lh_ui_web_player_page_create_focus, "c.focus();"},
    {lh_ui_web_player_page_create_socket_state, "var sock=null,retry=200,pollId=0,retryId=0;"},
    {lh_ui_web_player_page_create_stop_poll, "function stopPoll(){"},
    {lh_ui_web_player_page_create_start_poll, "function startPoll(){"},
    {lh_ui_web_player_page_create_arm_retry, "function armRetry(){"},
    {lh_ui_web_player_page_create_open_ws, "function openWs(){"},
    {lh_ui_web_player_page_create_start, "if(scaleMode==='layout'){"},
};

} // namespace

TEST(web_page, key_names_are_the_browsers)
{
    lh_str_view_t name = lh_ui_web_player_page_get_key_name(lh_os_key_space);

    EXPECT_EQ(std::string(lh_str_view_get_data(&name), lh_str_view_get_size(&name)), " ");
    name = lh_ui_web_player_page_get_key_name(lh_os_key_page_down);
    EXPECT_EQ(std::string(lh_str_view_get_data(&name), lh_str_view_get_size(&name)), "PageDown");
    name = lh_ui_web_player_page_get_key_name(lh_os_key_other);
    EXPECT_EQ(lh_str_view_get_size(&name), 0U);
    EXPECT_TRUE(lh_ui_web_player_page_is_modifier(lh_os_key_shift));
    EXPECT_TRUE(lh_ui_web_player_page_is_modifier(lh_os_key_alt));
    EXPECT_FALSE(lh_ui_web_player_page_is_modifier(lh_os_key_tab));
    EXPECT_FALSE(lh_ui_web_player_page_is_modifier(lh_os_key_other));
}

TEST(web_page, key_table_carries_every_key_by_its_enum_value)
{
    EXPECT_EQ(
        js_expression(lh_ui_web_player_page_create_key_table(lh_bool_false)),
        "{Tab:" + std::to_string(lh_os_key_tab) + ",Enter:" + std::to_string(lh_os_key_enter) +
            ",Escape:" + std::to_string(lh_os_key_escape) + ",Backspace:" +
            std::to_string(lh_os_key_backspace) + ",Delete:" + std::to_string(lh_os_key_delete) +
            ",[' ']:" + std::to_string(lh_os_key_space) + ",ArrowLeft:" +
            std::to_string(lh_os_key_left) + ",ArrowRight:" + std::to_string(lh_os_key_right) +
            ",ArrowUp:" + std::to_string(lh_os_key_up) +
            ",ArrowDown:" + std::to_string(lh_os_key_down) +
            ",Home:" + std::to_string(lh_os_key_home) + ",End:" + std::to_string(lh_os_key_end) +
            ",PageUp:" + std::to_string(lh_os_key_page_up) + ",PageDown:" +
            std::to_string(lh_os_key_page_down) + ",Shift:" + std::to_string(lh_os_key_shift) +
            ",Control:" + std::to_string(lh_os_key_control) +
            ",Alt:" + std::to_string(lh_os_key_alt) + "}");
    EXPECT_EQ(js_expression(lh_ui_web_player_page_create_key_table(lh_bool_true)),
              "{Shift:" + std::to_string(lh_os_key_shift) +
                  ",Control:" + std::to_string(lh_os_key_control) +
                  ",Alt:" + std::to_string(lh_os_key_alt) + "}");
    EXPECT_EQ(js(lh_ui_web_player_page_create_keys()).rfind("var keys={Tab:", 0), 0U);
    EXPECT_EQ(js(lh_ui_web_player_page_create_mods()).rfind("var mods={Shift:", 0), 0U);
}

TEST(web_page, every_part_writes_its_statement)
{
    for (const part &item : m_parts)
    {
        const std::string text = js(item.create());
        const std::string first_line = item.first_line;

        EXPECT_EQ(text.substr(0, text.find('\n')), first_line);
    }
}

TEST(web_page, script_is_every_part_in_order)
{
    lh_web_js_node_t *script = lh_ui_web_player_page_create_script();
    lh_str_t out;

    /* The parts above plus the two key tables. */
    EXPECT_EQ(lh_web_js_node_get_size(script), sizeof(m_parts) / sizeof(m_parts[0]) + 2U);
    lh_web_js_node_destroy(script);
    lh_str_init(&out);
    ASSERT_TRUE(lh_ui_web_player_page_try_write_script(&out));
    const std::string text = text_of(&out);
    lh_str_deinit(&out);
    std::string::size_type at = 0U;
    for (const part &item : m_parts)
    {
        at = text.find(std::string(item.first_line) + "\n", at);
        EXPECT_NE(at, std::string::npos) << item.first_line;
    }
}

TEST(web_page, document_wraps_the_script)
{
    lh_str_t out;

    lh_str_init(&out);
    ASSERT_TRUE(lh_ui_web_player_page_try_write_document(&out, lh_str_view_lit("f();\n")));
    EXPECT_EQ(
        text_of(&out),
        "<!doctype html><html lang=\"ru\"><head><meta charset=\"utf-8\"><meta "
        "name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>lh_ui</title>"
        "<style>html,body{margin:0;height:100%;background:#edf0f5;overflow:hidden}"
        "#c{display:block;position:absolute;cursor:pointer;outline:none}"
        "#v3d{position:absolute;display:none;pointer-events:none}</style></head><body>"
        "<canvas id=\"c\" width=\"980\" height=\"620\" tabindex=\"0\"></canvas>"
        "<canvas id=\"v3d\"></canvas><script>\nf();\n</script></body></html>");
    lh_str_deinit(&out);
}

TEST(web_page, build_is_the_written_page_and_checks_room)
{
    const std::string page = page_text();
    lh_str_t out;

    lh_str_init(&out);
    lh_ui_web_player_page_write(&out);
    EXPECT_EQ(text_of(&out), page);
    lh_str_deinit(&out);

    std::vector<lh_byte_t> mem(page.size());
    lh_usize_t len = 1U;
    /* No room for the terminator. */
    EXPECT_FALSE(lh_ui_web_player_page_build(mem.data(), mem.size(), &len));
    EXPECT_EQ(len, 0U);
    EXPECT_FALSE(lh_ui_web_player_page_build(nullptr, 1024U, &len));
    mem.resize(page.size() + 1U);
    EXPECT_TRUE(lh_ui_web_player_page_build(mem.data(), mem.size(), nullptr));
    EXPECT_EQ(mem.back(), 0U);
}

TEST(web_page, keys_go_down_and_up_with_a_direction)
{
    const std::string page = page_text();

    EXPECT_NE(page.find("document.onkeydown"), std::string::npos);
    EXPECT_NE(page.find("document.onkeyup"), std::string::npos);
    EXPECT_NE(page.find("emit('k '+k+' 1')"), std::string::npos);
    EXPECT_NE(page.find("emit('k '+k+' 0')"), std::string::npos);
    /* The HTTP fallback carries the direction too. */
    EXPECT_NE(page.find("'&p='"), std::string::npos);
}

TEST(web_page, a_held_button_sends_moves_and_the_last_before_the_release)
{
    const std::string page = page_text();
    const std::string::size_type up = page.find("window.onmouseup");

    EXPECT_NE(page.find("window.onmousemove"), std::string::npos);
    EXPECT_NE(page.find("emit('m '+"), std::string::npos);
    EXPECT_NE(page.find("/ui/move?x="), std::string::npos);
    ASSERT_NE(up, std::string::npos);
    EXPECT_LT(page.find("flushMove()", up), page.find("emit('r '", up));
}

TEST(web_page, tint_draws_the_mask_in_one_colour)
{
    const std::string tint = between(page_text(), "function tint(a,c){", "function glyph(");

    EXPECT_NE(tint.find("var t=document.createElement('canvas');\n"), std::string::npos);
    EXPECT_NE(tint.find("t.width=a.width;\nt.height=a.height;\n"), std::string::npos);
    EXPECT_NE(tint.find("var g=t.getContext('2d');\n"), std::string::npos);
    EXPECT_NE(tint.find("g.fillStyle=rgba(c);\n"), std::string::npos);
    EXPECT_NE(tint.find("g.fillRect(0,0,t.width,t.height);\n"), std::string::npos);
    EXPECT_NE(tint.find("g.globalCompositeOperation='destination-in';\n"), std::string::npos);
    EXPECT_NE(tint.find("g.drawImage(a,0,0);\n"), std::string::npos);
}

TEST(web_page, decode_cover_fills_the_alpha_then_uploads)
{
    const std::string function =
        between(page_text(), "function decodeCover(w,h,cv){", "function ingest(");

    EXPECT_NE(function.find("if(w<=0||h<=0){\nreturn im;\n}"), std::string::npos);
    EXPECT_NE(function.find("var id=g.createImageData(w,h);\n"), std::string::npos);
    const std::string::size_type loop = function.find("for(i=0,j=0;i<n;i++,j+=4){\n");
    const std::string::size_type upload = function.find("g.putImageData(id,0,0);\n");
    ASSERT_NE(loop, std::string::npos);
    ASSERT_NE(upload, std::string::npos);
    EXPECT_LT(loop, upload);
    EXPECT_NE(
        function.find("p[j+3]=k+1<cv.length?hexv(cv.charCodeAt(k))<<4|hexv(cv.charCodeAt(k+1)):0;"),
        std::string::npos);
    EXPECT_NE(function.find("return im;", upload), std::string::npos);
}

TEST(web_page, round_selects_rounded_or_plain_canvas_path)
{
    const std::string function =
        between(page_text(), "function round(x,r,rad){", "function pane3d(");

    EXPECT_NE(function.find("x.beginPath();\nif(x.roundRect){\n"), std::string::npos);
    EXPECT_NE(function.find("x.roundRect(r[0],r[1],r[2],r[3],rad||0);\n}else{\n"),
              std::string::npos);
    EXPECT_NE(function.find("x.rect(r[0],r[1],r[2],r[3]);\n"), std::string::npos);
}

TEST(web_page, play_switches_on_the_op)
{
    const std::string function = between(page_text(), "function play(d){", "function sized(");

    EXPECT_NE(function.find("switch(op.op){\ncase 'fill_mask':\n{\nvar g=glyph(op);\n"),
              std::string::npos);
    EXPECT_NE(function.find("case 'shadow':\n"), std::string::npos);
    EXPECT_NE(function.find("x.shadowOffsetX=op.offset?op.offset[0]:0;\n"), std::string::npos);
}

TEST(web_page, open_ws_falls_back_to_polling)
{
    const std::string function =
        between(page_text(), "function openWs(){", "if(scaleMode==='layout'){");

    EXPECT_NE(
        function.find("try{\nsock=new WebSocket((location.protocol==='https:'?'wss://':'ws://')"
                      "+location.host+'/ws');\n}catch(e){\nsock=null;\nstartPoll();\n"),
        std::string::npos);
}

TEST(web_page, the_page_reads_back_as_html_and_writes_the_same_bytes)
{
    const std::string page = page_text();
    lh_web_html_reader_t reader;
    lh_web_xml_node_t doc;
    lh_web_xml_writer_t writer;
    lh_str_view_t view;
    lh_str_t out;

    lh_str_view_init_by_size(&view, page.data(), page.size());
    lh_web_xml_node_init(&doc);
    lh_web_html_reader_init(&reader, view);
    ASSERT_TRUE(lh_web_html_reader_read(&reader, &doc));
    lh_str_init(&out);
    lh_web_xml_writer_init(&writer, &out);
    lh_web_xml_writer_set_dialect(&writer, lh_web_xml_dialect_html);
    EXPECT_TRUE(lh_web_xml_writer_try_write(&writer, &doc));
    EXPECT_EQ(std::string(lh_str_get_data(&out), lh_str_get_size(&out)), page);
    lh_str_deinit(&out);
    lh_web_xml_node_deinit(&doc);
}
