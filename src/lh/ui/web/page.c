/**
 * @file page.c
 * @brief Canvas2D player: HTML entities + JS constructor, then serialize.
 *
 * Markup is ::lh_ui_web_html; the script body is ::lh_ui_web_js / ::lh_ui_web_x.
 * Dense loops still use ::lh_ui_web_buf_raw until they become statement entities.
 */

#include <lh/ui/web/page.h>
#include <lh/char/map.h>

#include <lh/assert/static.h>
#include <lh/os/key.h>
#include <lh/str/format/text.h>
#include <lh/ui/web/html.h>
#include <lh/ui/web/js.h>
#include <lh/ui/web/js/x.h>

/** Append string literal @p s to @p j, its length taken at compile time. */
#define page_lit(j, s) lh_ui_web_buf_lit((j), (s), lh_cast_static(lh_usize_t, (sizeof(s) - 1u)))

/* -- keys ------------------------------------------------------------------ */

/** Room for one key table as JS text, braces and NUL included. */
#define PAGE_KEY_TABLE_CAP 512u

/** A browser `KeyboardEvent.key` and the ::lh_os_key_t the player sends for it. */
typedef struct page_key
{
    lh_str_cptr name;   /**< `e.key` as the browser spells it */
    lh_os_key_t key;    /**< what the host is sent */
    lh_bool_t modifier; /**< held with other keys, so the browser keeps its default */
} page_key_t;           /**< A browser key name and the ::lh_os_key_t for it. */

/** Every ::lh_os_key_t but ::lh_os_key_other, by the name a browser gives it. The player's
   table is written from this, so the numbers on the wire are the enum's own; a key added to
   the enum without a name here stops the build at the check below. */
static const page_key_t page_keys[] = {
    {"Tab", lh_os_key_tab, lh_bool_false},        {"Enter", lh_os_key_enter, lh_bool_false},
    {"Escape", lh_os_key_escape, lh_bool_false},  {"Backspace", lh_os_key_backspace, lh_bool_false},
    {"Delete", lh_os_key_delete, lh_bool_false},  {" ", lh_os_key_space, lh_bool_false},
    {"ArrowLeft", lh_os_key_left, lh_bool_false}, {"ArrowRight", lh_os_key_right, lh_bool_false},
    {"ArrowUp", lh_os_key_up, lh_bool_false},     {"ArrowDown", lh_os_key_down, lh_bool_false},
    {"Home", lh_os_key_home, lh_bool_false},      {"End", lh_os_key_end, lh_bool_false},
    {"PageUp", lh_os_key_page_up, lh_bool_false}, {"PageDown", lh_os_key_page_down, lh_bool_false},
    {"Shift", lh_os_key_shift, lh_bool_true},     {"Control", lh_os_key_control, lh_bool_true},
    {"Alt", lh_os_key_alt, lh_bool_true},
};

/** Entries in ::page_keys. */
#define PAGE_KEY_COUNT (sizeof(page_keys) / sizeof(page_keys[0]))

lh_assert_static(PAGE_KEY_COUNT == lh_cast_static(lh_usize_t, lh_os_key_alt),
                 "page_keys must name every lh_os_key_t but lh_os_key_other");

lh_void
lh_ui_web_page_emit_key_table(lh_ui_web_buf_t *j, lh_str_cptr name, lh_bool_t modifiers_only)
{
    lh_char_t table[PAGE_KEY_TABLE_CAP];
    lh_usize_t len = 0u;
    lh_usize_t i;

    table[len++] = lh_char_map_lbrace;
    for (i = 0u; i < PAGE_KEY_COUNT; ++i)
    {
        lh_usize_t n;

        if (modifiers_only && !page_keys[i].modifier)
        {
            continue;
        }
        n = lh_str_ptr_format_text(table + len, sizeof(table) - len, "%s'%s':%u",
                                   (len > 1u) ? "," : "", page_keys[i].name,
                                   lh_cast_static(lh_uint_t, page_keys[i].key));
        if (n == 0u || len + n + 2u > sizeof(table))
        {
            lh_ui_web_buf_fail(j);
            return;
        }
        len += n;
    }
    table[len++] = lh_char_map_rbrace;
    table[len] = lh_char_map_nul;
    lh_ui_web_js_var(j, name, table);
}

/* -- page ------------------------------------------------------------------ */

lh_bool_t
lh_ui_web_page_build(lh_byte_ptr mem, lh_usize_t cap, lh_usize_t *out_len)
{
    lh_ui_web_buf_t *j;
    lh_ui_web_buf_t js;
    lh_ui_web_html_t doc_type;
    lh_ui_web_html_t el_html;
    lh_ui_web_html_t el_head;
    lh_ui_web_html_t el_body;
    lh_ui_web_html_t el_script;
    lh_ui_web_html_t el_meta_cs;
    lh_ui_web_html_t el_meta_vp;
    lh_ui_web_html_t el_title;
    lh_ui_web_html_t el_style;
    lh_ui_web_html_t el_canvas_c;
    lh_ui_web_html_t el_canvas_v3d;
    lh_ui_web_html_t title_text;
    lh_ui_web_html_t style_raw;
    lh_ui_web_html_attr_t attr_lang;
    lh_ui_web_html_attr_t attr_charset;
    lh_ui_web_html_attr_t attr_vp_name;
    lh_ui_web_html_attr_t attr_vp_content;
    lh_ui_web_html_attr_t attr_c_id;
    lh_ui_web_html_attr_t attr_c_w;
    lh_ui_web_html_attr_t attr_c_h;
    lh_ui_web_html_attr_t attr_c_tab;
    lh_ui_web_html_attr_t attr_v3d_id;
    lh_ui_web_html_attr_t meta_cs_attrs[1];
    lh_ui_web_html_attr_t meta_vp_attrs[2];
    lh_ui_web_html_attr_t canvas_c_attrs[4];
    lh_ui_web_html_attr_t canvas_v3d_attrs[1];
    lh_ui_web_html_attr_t html_attrs[1];
    const lh_ui_web_html_t *head_kids[4];
    const lh_ui_web_html_t *title_kids[1];
    const lh_ui_web_html_t *style_kids[1];

    lh_ui_web_buf_init(lh_addr_of(js), mem, cap);
    j = lh_addr_of(js);
    if (out_len != lh_null)
    {
        lh_ptr_deref(out_len) = 0u;
    }

    /* -- markup tree (construct, then serialize) ------------------------------- */
    lh_ui_web_html_doctype(lh_addr_of(doc_type));
    lh_ui_web_html_attr("lang", "ru", lh_addr_of(attr_lang));
    html_attrs[0] = attr_lang;
    lh_ui_web_html_tag("html", html_attrs, 1u, lh_null, 0u, lh_bool_false, lh_addr_of(el_html));

    lh_ui_web_html_attr("charset", "utf-8", lh_addr_of(attr_charset));
    meta_cs_attrs[0] = attr_charset;
    lh_ui_web_html_tag("meta", meta_cs_attrs, 1u, lh_null, 0u, lh_bool_true,
                       lh_addr_of(el_meta_cs));

    lh_ui_web_html_attr("name", "viewport", lh_addr_of(attr_vp_name));
    lh_ui_web_html_attr("content", "width=device-width,initial-scale=1",
                        lh_addr_of(attr_vp_content));
    meta_vp_attrs[0] = attr_vp_name;
    meta_vp_attrs[1] = attr_vp_content;
    lh_ui_web_html_tag("meta", meta_vp_attrs, 2u, lh_null, 0u, lh_bool_true,
                       lh_addr_of(el_meta_vp));

    lh_ui_web_html_text("lh_ui", lh_addr_of(title_text));
    title_kids[0] = lh_addr_of(title_text);
    lh_ui_web_html_tag("title", lh_null, 0u, title_kids, 1u, lh_bool_false, lh_addr_of(el_title));

    lh_ui_web_html_raw("html,body{margin:0;height:100%;background:#edf0f5;overflow:hidden}"
                       "#c{display:block;position:absolute;cursor:pointer;outline:none}"
                       "#v3d{position:absolute;display:none;pointer-events:none}",
                       lh_addr_of(style_raw));
    style_kids[0] = lh_addr_of(style_raw);
    lh_ui_web_html_tag("style", lh_null, 0u, style_kids, 1u, lh_bool_false, lh_addr_of(el_style));

    head_kids[0] = lh_addr_of(el_meta_cs);
    head_kids[1] = lh_addr_of(el_meta_vp);
    head_kids[2] = lh_addr_of(el_title);
    head_kids[3] = lh_addr_of(el_style);
    lh_ui_web_html_tag("head", lh_null, 0u, head_kids, 4u, lh_bool_false, lh_addr_of(el_head));

    lh_ui_web_html_attr("id", "c", lh_addr_of(attr_c_id));
    lh_ui_web_html_attr("width", "980", lh_addr_of(attr_c_w));
    lh_ui_web_html_attr("height", "620", lh_addr_of(attr_c_h));
    lh_ui_web_html_attr("tabindex", "0", lh_addr_of(attr_c_tab));
    canvas_c_attrs[0] = attr_c_id;
    canvas_c_attrs[1] = attr_c_w;
    canvas_c_attrs[2] = attr_c_h;
    canvas_c_attrs[3] = attr_c_tab;
    lh_ui_web_html_tag("canvas", canvas_c_attrs, 4u, lh_null, 0u, lh_bool_false,
                       lh_addr_of(el_canvas_c));

    lh_ui_web_html_attr("id", "v3d", lh_addr_of(attr_v3d_id));
    canvas_v3d_attrs[0] = attr_v3d_id;
    lh_ui_web_html_tag("canvas", canvas_v3d_attrs, 1u, lh_null, 0u, lh_bool_false,
                       lh_addr_of(el_canvas_v3d));

    lh_ui_web_html_tag("body", lh_null, 0u, lh_null, 0u, lh_bool_false, lh_addr_of(el_body));
    lh_ui_web_html_tag("script", lh_null, 0u, lh_null, 0u, lh_bool_false, lh_addr_of(el_script));

    lh_ui_web_html_emit(j, lh_addr_of(doc_type));
    lh_ui_web_html_open(j, lh_addr_of(el_html));
    lh_ui_web_html_emit(j, lh_addr_of(el_head));
    lh_ui_web_html_open(j, lh_addr_of(el_body));
    lh_ui_web_html_emit(j, lh_addr_of(el_canvas_c));
    lh_ui_web_html_emit(j, lh_addr_of(el_canvas_v3d));
    lh_ui_web_html_open(j, lh_addr_of(el_script));
    lh_ui_web_buf_lit(j, "\n", 1u);

    /* -- rgba ------------------------------------------------------------------ */
    lh_ui_web_js_fun_open(j, "rgba", "c");
    {
        lh_ui_web_x_t xc;
        lh_ui_web_x_t i0;
        lh_ui_web_x_t i1;
        lh_ui_web_x_t i2;
        lh_ui_web_x_t i3;
        lh_ui_web_x_t c0;
        lh_ui_web_x_t c1;
        lh_ui_web_x_t c2;
        lh_ui_web_x_t c3;
        lh_ui_web_x_t n255;
        lh_ui_web_x_t alpha;
        lh_ui_web_x_t s_rgba;
        lh_ui_web_x_t s_comma;
        lh_ui_web_x_t s_end;
        lh_ui_web_x_t t0;
        lh_ui_web_x_t t1;
        lh_ui_web_x_t t2;
        lh_ui_web_x_t t3;
        lh_ui_web_x_t t4;
        lh_ui_web_x_t t5;
        lh_ui_web_x_t t6;
        lh_ui_web_x_t t7;

        lh_ui_web_x_id("c", lh_addr_of(xc));
        lh_ui_web_x_num(0, lh_addr_of(i0));
        lh_ui_web_x_num(1, lh_addr_of(i1));
        lh_ui_web_x_num(2, lh_addr_of(i2));
        lh_ui_web_x_num(3, lh_addr_of(i3));
        lh_ui_web_x_idx(lh_addr_of(xc), lh_addr_of(i0), lh_addr_of(c0));
        lh_ui_web_x_idx(lh_addr_of(xc), lh_addr_of(i1), lh_addr_of(c1));
        lh_ui_web_x_idx(lh_addr_of(xc), lh_addr_of(i2), lh_addr_of(c2));
        lh_ui_web_x_idx(lh_addr_of(xc), lh_addr_of(i3), lh_addr_of(c3));
        lh_ui_web_x_num(255, lh_addr_of(n255));
        lh_ui_web_x_bin(lh_addr_of(c3), "/", lh_addr_of(n255), lh_addr_of(alpha));
        lh_ui_web_x_str("rgba(", lh_addr_of(s_rgba));
        lh_ui_web_x_str(",", lh_addr_of(s_comma));
        lh_ui_web_x_str(")", lh_addr_of(s_end));
        lh_ui_web_x_bin(lh_addr_of(s_rgba), "+", lh_addr_of(c0), lh_addr_of(t0));
        lh_ui_web_x_bin(lh_addr_of(t0), "+", lh_addr_of(s_comma), lh_addr_of(t1));
        lh_ui_web_x_bin(lh_addr_of(t1), "+", lh_addr_of(c1), lh_addr_of(t2));
        lh_ui_web_x_bin(lh_addr_of(t2), "+", lh_addr_of(s_comma), lh_addr_of(t3));
        lh_ui_web_x_bin(lh_addr_of(t3), "+", lh_addr_of(c2), lh_addr_of(t4));
        lh_ui_web_x_bin(lh_addr_of(t4), "+", lh_addr_of(s_comma), lh_addr_of(t5));
        lh_ui_web_x_bin(lh_addr_of(t5), "+", lh_addr_of(alpha), lh_addr_of(t6));
        lh_ui_web_x_bin(lh_addr_of(t6), "+", lh_addr_of(s_end), lh_addr_of(t7));
        lh_ui_web_js_ret_x(j, lh_addr_of(t7));
    }
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_vars(
        j, "atlas=[],tints={},ntint=0,fw=980,fh=620,cssS=1,lastD=null,pend=null,raf=0,v3on=0");
    lh_ui_web_js_vars(
        j, "scaleMode=(/scale=fit/.test(location.search)?'fit':'layout'),lastZW=0,lastZH=0");

    /* -- hexv ------------------------------------------------------------------ */
    lh_ui_web_js_fun_open(j, "hexv", "c");
    {
        lh_ui_web_x_t xc;
        lh_ui_web_x_t n58;
        lh_ui_web_x_t n48;
        lh_ui_web_x_t n32;
        lh_ui_web_x_t n87;
        lh_ui_web_x_t lt;
        lh_ui_web_x_t sub48;
        lh_ui_web_x_t or32;
        lh_ui_web_x_t sub87;
        lh_ui_web_x_t tern;

        lh_ui_web_x_id("c", lh_addr_of(xc));
        lh_ui_web_x_num(58, lh_addr_of(n58));
        lh_ui_web_x_num(48, lh_addr_of(n48));
        lh_ui_web_x_num(32, lh_addr_of(n32));
        lh_ui_web_x_num(87, lh_addr_of(n87));
        lh_ui_web_x_bin(lh_addr_of(xc), "<", lh_addr_of(n58), lh_addr_of(lt));
        lh_ui_web_x_bin(lh_addr_of(xc), "-", lh_addr_of(n48), lh_addr_of(sub48));
        lh_ui_web_x_bin(lh_addr_of(xc), "|", lh_addr_of(n32), lh_addr_of(or32));
        lh_ui_web_x_bin(lh_addr_of(or32), "-", lh_addr_of(n87), lh_addr_of(sub87));
        lh_ui_web_x_tern(lh_addr_of(lt), lh_addr_of(sub48), lh_addr_of(sub87), lh_addr_of(tern));
        lh_ui_web_js_ret_x(j, lh_addr_of(tern));
    }
    lh_ui_web_js_fun_close(j);

    /* -- decodeCover ----------------------------------------------------------- */
    lh_ui_web_js_fun_open(j, "decodeCover", "w,h,cv");
    lh_ui_web_js_var(j, "im", "document.createElement('canvas')");
    lh_ui_web_js_assign(j, "im.width", "w");
    lh_ui_web_js_assign(j, "im.height", "h");
    lh_ui_web_js_if(j, "w<=0||h<=0");
    lh_ui_web_js_ret(j, "im");
    lh_ui_web_js_end(j);
    lh_ui_web_js_vars(j, "g=im.getContext('2d'),id=g.createImageData(w,h),p=id.data,n=w*h,i,j,k");
    /* Pixel loop stays dense — expanding it gains nothing readable. */
    lh_ui_web_buf_raw(
        j, "for(i=0,j=0;i<n;i++,j+=4){k=i*2;p[j]=p[j+1]=p[j+2]=255;"
           "p[j+3]=k+1<cv.length?(hexv(cv.charCodeAt(k))<<4)|hexv(cv.charCodeAt(k+1)):0;}\n");
    lh_ui_web_js_stmt(j, "g.putImageData(id,0,0)");
    lh_ui_web_js_ret(j, "im");
    lh_ui_web_js_fun_close(j);

    /* -- ingest ---------------------------------------------------------------- */
    lh_ui_web_js_fun_open(j, "ingest", "d");
    lh_ui_web_js_vars(j, "ops=d.ops||[],i,op");
    lh_ui_web_js_for(j, "i=0;i<ops.length;i++");
    lh_ui_web_js_assign(j, "op", "ops[i]");
    lh_ui_web_js_if(j, "op.cover!==undefined&&typeof op.m==='number'");
    lh_ui_web_js_assign(j, "atlas[op.m]", "decodeCover(op.size[0],op.size[1],op.cover)");
    lh_ui_web_js_end(j);
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    /* -- tint ------------------------------------------------------------------ */
    lh_ui_web_js_fun_open(j, "tint", "a,c");
    lh_ui_web_js_var(j, "t", "document.createElement('canvas')");
    lh_ui_web_js_assign(j, "t.width", "a.width");
    lh_ui_web_js_assign(j, "t.height", "a.height");
    lh_ui_web_js_if(j, "!a.width||!a.height");
    lh_ui_web_js_ret(j, "t");
    lh_ui_web_js_end(j);
    lh_ui_web_js_var(j, "g", "t.getContext('2d')");
    lh_ui_web_js_assign(j, "g.fillStyle", "rgba(c)");
    lh_ui_web_js_stmt(j, "g.fillRect(0,0,t.width,t.height)");
    lh_ui_web_js_assign(j, "g.globalCompositeOperation", "'destination-in'");
    lh_ui_web_js_stmt(j, "g.drawImage(a,0,0)");
    lh_ui_web_js_ret(j, "t");
    lh_ui_web_js_fun_close(j);

    /* -- glyph ----------------------------------------------------------------- */
    lh_ui_web_js_fun_open(j, "glyph", "op");
    lh_ui_web_js_var(j, "c", "op.color");
    lh_ui_web_js_if(j, "typeof op.m!=='number'");
    lh_ui_web_js_ret(j, "tint(decodeCover(op.size[0],op.size[1],op.cover||''),c)");
    lh_ui_web_js_end(j);
    lh_ui_web_js_vars(j, "k=op.m+','+c[0]+','+c[1]+','+c[2]+','+c[3],t=tints[k]");
    lh_ui_web_js_if(j, "t");
    lh_ui_web_js_ret(j, "t");
    lh_ui_web_js_end(j);
    lh_ui_web_js_var(j, "a", "atlas[op.m]");
    lh_ui_web_js_if(j, "!a");
    lh_ui_web_js_ret(j, "null");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "ntint>=8192");
    lh_ui_web_js_assign(j, "tints", "{}");
    lh_ui_web_js_assign(j, "ntint", "0");
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "t", "tint(a,c)");
    lh_ui_web_js_assign(j, "tints[k]", "t");
    lh_ui_web_js_stmt(j, "++ntint");
    lh_ui_web_js_ret(j, "t");
    lh_ui_web_js_fun_close(j);

    /* -- round ----------------------------------------------------------------- */
    lh_ui_web_js_fun_open(j, "round", "x,r,rad");
    lh_ui_web_js_stmt(j, "x.beginPath()");
    lh_ui_web_js_if(j, "x.roundRect");
    lh_ui_web_js_stmt(j, "x.roundRect(r[0],r[1],r[2],r[3],rad||0)");
    lh_ui_web_js_else(j);
    lh_ui_web_js_stmt(j, "x.rect(r[0],r[1],r[2],r[3])");
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    /* -- pane3d ---------------------------------------------------------------- */
    lh_ui_web_js_fun_open(j, "pane3d", "d");
    lh_ui_web_js_vars(
        j, "v=document.getElementById('v3d'),r=d.viewport3d,c=document.getElementById('c')");
    lh_ui_web_js_if(j, "!r");
    lh_ui_web_js_if(j, "v3on");
    lh_ui_web_js_assign(j, "v.style.display", "'none'");
    lh_ui_web_js_assign(j, "v3on", "0");
    lh_ui_web_js_end(j);
    lh_ui_web_js_ret(j, lh_null);
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "v3on", "1");
    lh_ui_web_js_vars(j, "box=c.getBoundingClientRect(),dpr=window.devicePixelRatio||1");
    lh_ui_web_js_assign(j, "v.style.display", "'block'");
    lh_ui_web_js_assign(j, "v.style.left", "(box.left+r[0]*cssS)+'px'");
    lh_ui_web_js_assign(j, "v.style.top", "(box.top+r[1]*cssS)+'px'");
    lh_ui_web_js_assign(j, "v.style.width", "(r[2]*cssS)+'px'");
    lh_ui_web_js_assign(j, "v.style.height", "(r[3]*cssS)+'px'");
    lh_ui_web_js_assign(j, "v.width", "Math.max(1,Math.round(r[2]*cssS*dpr))");
    lh_ui_web_js_assign(j, "v.height", "Math.max(1,Math.round(r[3]*cssS*dpr))");
    lh_ui_web_js_if(j, "!v.gl");
    lh_ui_web_js_assign(j, "v.gl", "v.getContext('webgl')||v.getContext('experimental-webgl')");
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    /* -- fit ------------------------------------------------------------------- */
    lh_ui_web_js_fun_open(j, "fit", "");
    lh_ui_web_js_vars(j,
                      "c=document.getElementById('c'),vw=window.innerWidth,vh=window.innerHeight,"
                      "dpr=window.devicePixelRatio||1,s,bw,bh");
    lh_ui_web_js_if(j, "scaleMode==='layout'");
    lh_ui_web_js_assign(j, "cssS", "1");
    lh_ui_web_js_assign(j, "bw", "Math.max(1,Math.round(fw*dpr))");
    lh_ui_web_js_assign(j, "bh", "Math.max(1,Math.round(fh*dpr))");
    lh_ui_web_js_if(j, "c.width!==bw");
    lh_ui_web_js_assign(j, "c.width", "bw");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "c.height!==bh");
    lh_ui_web_js_assign(j, "c.height", "bh");
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "c.style.width", "fw+'px'");
    lh_ui_web_js_assign(j, "c.style.height", "fh+'px'");
    lh_ui_web_js_assign(j, "c.style.left", "'0'");
    lh_ui_web_js_assign(j, "c.style.top", "'0'");
    lh_ui_web_js_ret(j, "c.getContext('2d')");
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "s", "Math.min(vw/fw,vh/fh)");
    lh_ui_web_js_if(j, "!isFinite(s)||s<=0");
    lh_ui_web_js_assign(j, "s", "1");
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "cssS", "s");
    lh_ui_web_js_assign(j, "bw", "Math.max(1,Math.round(fw*s*dpr))");
    lh_ui_web_js_assign(j, "bh", "Math.max(1,Math.round(fh*s*dpr))");
    lh_ui_web_js_if(j, "c.width!==bw");
    lh_ui_web_js_assign(j, "c.width", "bw");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "c.height!==bh");
    lh_ui_web_js_assign(j, "c.height", "bh");
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "c.style.width", "Math.round(fw*s)+'px'");
    lh_ui_web_js_assign(j, "c.style.height", "Math.round(fh*s)+'px'");
    lh_ui_web_js_assign(j, "c.style.left", "Math.round((vw-fw*s)/2)+'px'");
    lh_ui_web_js_assign(j, "c.style.top", "Math.round((vh-fh*s)/2)+'px'");
    lh_ui_web_js_ret(j, "c.getContext('2d')");
    lh_ui_web_js_fun_close(j);

    /* -- play (switch stays one raw block) ------------------------------------- */
    lh_ui_web_js_fun_open(j, "play", "d");
    lh_ui_web_js_assign(j, "lastD", "d");
    lh_ui_web_js_assign(j, "fw", "d.width");
    lh_ui_web_js_assign(j, "fh", "d.height");
    lh_ui_web_js_vars(j, "x=fit(),ops=d.ops||[],i,op,s,fs=null,dpr=window.devicePixelRatio||1");
    lh_ui_web_js_stmt(j, "x.setTransform(cssS*dpr,0,0,cssS*dpr,0,0)");
    lh_ui_web_js_stmt(j, "pane3d(d)");
    lh_ui_web_buf_raw(
        j,
        "for(i=0;i<ops.length;i++){op=ops[i];switch(op.op){"
        "case 'fill_mask':{var g=glyph(op);if(g)x.drawImage(g,op.origin[0],op.origin[1]);break;}"
        "case 'fill_rect':s=rgba(op.color);if(s!==fs){x.fillStyle=s;fs=s;}"
        "x.fillRect(op.rect[0],op.rect[1],op.rect[2],op.rect[3]);break;"
        "case 'fill_round_rect':s=rgba(op.color);if(s!==fs){x.fillStyle=s;fs=s;}"
        "round(x,op.rect,op.radius||0);x.fill();break;"
        "case 'clear':s=rgba(op.color);if(s!==fs){x.fillStyle=s;fs=s;}x.fillRect(0,0,fw,fh);break;"
        "case 'shadow':x.save();x.shadowColor=rgba(op.color);x.shadowBlur=op.spread||0;"
        "x.shadowOffsetX=op.offset?op.offset[0]:0;x.shadowOffsetY=op.offset?op.offset[1]:0;"
        "x.fillStyle=rgba(op.color);round(x,op.rect,op.radius||0);x.fill();x.restore();fs=null;"
        "break;"
        "}}\n");
    lh_ui_web_js_fun_close(j);

    /* -- sized / show / pos / viewSize / reportSize ---------------------------- */
    lh_ui_web_js_fun_open(j, "sized", "d");
    {
        lh_ui_web_x_t scale;
        lh_ui_web_x_t layout;
        lh_ui_web_x_t ne;
        lh_ui_web_x_t last_zw;
        lh_ui_web_x_t not_zw;
        lh_ui_web_x_t skip;
        lh_ui_web_x_t one;
        lh_ui_web_x_t zero;
        lh_ui_web_x_t xd;
        lh_ui_web_x_t dw;
        lh_ui_web_x_t dh;
        lh_ui_web_x_t n980;
        lh_ui_web_x_t n620;
        lh_ui_web_x_t last_zh;
        lh_ui_web_x_t eq_w;
        lh_ui_web_x_t eq_h;
        lh_ui_web_x_t ne_zw;
        lh_ui_web_x_t ne_zh;
        lh_ui_web_x_t wrong_z;
        lh_ui_web_x_t design;
        lh_ui_web_x_t reject;

        lh_ui_web_x_id("scaleMode", lh_addr_of(scale));
        lh_ui_web_x_str("layout", lh_addr_of(layout));
        lh_ui_web_x_bin(lh_addr_of(scale), "!==", lh_addr_of(layout), lh_addr_of(ne));
        lh_ui_web_x_id("lastZW", lh_addr_of(last_zw));
        lh_ui_web_x_un("!", lh_addr_of(last_zw), lh_addr_of(not_zw));
        lh_ui_web_x_bin(lh_addr_of(ne), "||", lh_addr_of(not_zw), lh_addr_of(skip));
        lh_ui_web_x_num(1, lh_addr_of(one));
        lh_ui_web_js_if_x(j, lh_addr_of(skip));
        lh_ui_web_js_ret_x(j, lh_addr_of(one));
        lh_ui_web_js_end(j);

        lh_ui_web_x_id("d", lh_addr_of(xd));
        lh_ui_web_x_dot(lh_addr_of(xd), "width", lh_addr_of(dw));
        lh_ui_web_x_dot(lh_addr_of(xd), "height", lh_addr_of(dh));
        lh_ui_web_x_num(980, lh_addr_of(n980));
        lh_ui_web_x_num(620, lh_addr_of(n620));
        lh_ui_web_x_id("lastZH", lh_addr_of(last_zh));
        lh_ui_web_x_bin(lh_addr_of(dw), "===", lh_addr_of(n980), lh_addr_of(eq_w));
        lh_ui_web_x_bin(lh_addr_of(dh), "===", lh_addr_of(n620), lh_addr_of(eq_h));
        lh_ui_web_x_bin(lh_addr_of(last_zw), "!==", lh_addr_of(n980), lh_addr_of(ne_zw));
        lh_ui_web_x_bin(lh_addr_of(last_zh), "!==", lh_addr_of(n620), lh_addr_of(ne_zh));
        lh_ui_web_x_bin(lh_addr_of(ne_zw), "||", lh_addr_of(ne_zh), lh_addr_of(wrong_z));
        lh_ui_web_x_bin(lh_addr_of(eq_w), "&&", lh_addr_of(eq_h), lh_addr_of(design));
        lh_ui_web_x_bin(lh_addr_of(design), "&&", lh_addr_of(wrong_z), lh_addr_of(reject));
        lh_ui_web_x_num(0, lh_addr_of(zero));
        lh_ui_web_js_if_x(j, lh_addr_of(reject));
        lh_ui_web_js_ret_x(j, lh_addr_of(zero));
        lh_ui_web_js_end(j);
        lh_ui_web_js_ret_x(j, lh_addr_of(one));
    }
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "show", "d");
    {
        lh_ui_web_x_t xd;
        lh_ui_web_x_t ingest;
        lh_ui_web_x_t sized;
        lh_ui_web_x_t not_sized;
        lh_ui_web_x_t pend;
        lh_ui_web_x_t raf;
        lh_ui_web_x_t not_raf;
        const lh_ui_web_x_t *ingest_args[1];
        const lh_ui_web_x_t *sized_args[1];

        lh_ui_web_x_id("d", lh_addr_of(xd));
        ingest_args[0] = lh_addr_of(xd);
        lh_ui_web_x_call_id("ingest", ingest_args, 1u, lh_addr_of(ingest));
        lh_ui_web_js_stmt_x(j, lh_addr_of(ingest));
        sized_args[0] = lh_addr_of(xd);
        lh_ui_web_x_call_id("sized", sized_args, 1u, lh_addr_of(sized));
        lh_ui_web_x_un("!", lh_addr_of(sized), lh_addr_of(not_sized));
        lh_ui_web_js_if_x(j, lh_addr_of(not_sized));
        lh_ui_web_js_ret_x(j, lh_null);
        lh_ui_web_js_end(j);
        lh_ui_web_x_id("pend", lh_addr_of(pend));
        lh_ui_web_js_assign_x(j, lh_addr_of(pend), lh_addr_of(xd));
        lh_ui_web_x_id("raf", lh_addr_of(raf));
        lh_ui_web_x_un("!", lh_addr_of(raf), lh_addr_of(not_raf));
        lh_ui_web_js_if_x(j, lh_addr_of(not_raf));
        lh_ui_web_js_assign(
            j, "raf",
            "requestAnimationFrame(function(){raf=0;var p=pend;pend=null;if(p)play(p);})");
        lh_ui_web_js_end(j);
    }
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "pos", "e");
    lh_ui_web_js_vars(j, "c=document.getElementById('c'),r=c.getBoundingClientRect()");
    {
        lh_ui_web_x_t xe;
        lh_ui_web_x_t xr;
        lh_ui_web_x_t xfw;
        lh_ui_web_x_t xfh;
        lh_ui_web_x_t client_x;
        lh_ui_web_x_t client_y;
        lh_ui_web_x_t left;
        lh_ui_web_x_t top;
        lh_ui_web_x_t width;
        lh_ui_web_x_t height;
        lh_ui_web_x_t dx;
        lh_ui_web_x_t dy;
        lh_ui_web_x_t nx1;
        lh_ui_web_x_t nx2;
        lh_ui_web_x_t ny1;
        lh_ui_web_x_t ny2;
        lh_ui_web_x_t math;
        lh_ui_web_x_t math_round;
        lh_ui_web_x_t round_x;
        lh_ui_web_x_t round_y;
        lh_ui_web_x_t obj;
        const lh_str_ptr keys[2];
        const lh_ui_web_x_t *vals[2];
        const lh_ui_web_x_t *rx_args[1];
        const lh_ui_web_x_t *ry_args[1];

        lh_ui_web_x_id("e", lh_addr_of(xe));
        lh_ui_web_x_id("r", lh_addr_of(xr));
        lh_ui_web_x_id("fw", lh_addr_of(xfw));
        lh_ui_web_x_id("fh", lh_addr_of(xfh));
        lh_ui_web_x_dot(lh_addr_of(xe), "clientX", lh_addr_of(client_x));
        lh_ui_web_x_dot(lh_addr_of(xe), "clientY", lh_addr_of(client_y));
        lh_ui_web_x_dot(lh_addr_of(xr), "left", lh_addr_of(left));
        lh_ui_web_x_dot(lh_addr_of(xr), "top", lh_addr_of(top));
        lh_ui_web_x_dot(lh_addr_of(xr), "width", lh_addr_of(width));
        lh_ui_web_x_dot(lh_addr_of(xr), "height", lh_addr_of(height));
        lh_ui_web_x_bin(lh_addr_of(client_x), "-", lh_addr_of(left), lh_addr_of(dx));
        lh_ui_web_x_bin(lh_addr_of(client_y), "-", lh_addr_of(top), lh_addr_of(dy));
        lh_ui_web_x_bin(lh_addr_of(dx), "*", lh_addr_of(xfw), lh_addr_of(nx1));
        lh_ui_web_x_bin(lh_addr_of(nx1), "/", lh_addr_of(width), lh_addr_of(nx2));
        lh_ui_web_x_bin(lh_addr_of(dy), "*", lh_addr_of(xfh), lh_addr_of(ny1));
        lh_ui_web_x_bin(lh_addr_of(ny1), "/", lh_addr_of(height), lh_addr_of(ny2));
        lh_ui_web_x_id("Math", lh_addr_of(math));
        lh_ui_web_x_dot(lh_addr_of(math), "round", lh_addr_of(math_round));
        rx_args[0] = lh_addr_of(nx2);
        ry_args[0] = lh_addr_of(ny2);
        lh_ui_web_x_call(lh_addr_of(math_round), rx_args, 1u, lh_addr_of(round_x));
        lh_ui_web_x_call(lh_addr_of(math_round), ry_args, 1u, lh_addr_of(round_y));
        keys[0] = "x";
        keys[1] = "y";
        vals[0] = lh_addr_of(round_x);
        vals[1] = lh_addr_of(round_y);
        lh_ui_web_x_obj(keys, vals, 2u, lh_addr_of(obj));
        lh_ui_web_js_ret_x(j, lh_addr_of(obj));
    }
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "viewSize", "");
    {
        lh_ui_web_x_t win;
        lh_ui_web_x_t iw;
        lh_ui_web_x_t ih;
        lh_ui_web_x_t zero;
        lh_ui_web_x_t w_init;
        lh_ui_web_x_t h_init;
        lh_ui_web_x_t xw;
        lh_ui_web_x_t xh;
        lh_ui_web_x_t one;
        lh_ui_web_x_t w_lt;
        lh_ui_web_x_t h_lt;
        lh_ui_web_x_t obj;
        const lh_str_ptr keys[2];
        const lh_ui_web_x_t *vals[2];

        lh_ui_web_x_id("window", lh_addr_of(win));
        lh_ui_web_x_dot(lh_addr_of(win), "innerWidth", lh_addr_of(iw));
        lh_ui_web_x_dot(lh_addr_of(win), "innerHeight", lh_addr_of(ih));
        lh_ui_web_x_num(0, lh_addr_of(zero));
        lh_ui_web_x_bin(lh_addr_of(iw), "|", lh_addr_of(zero), lh_addr_of(w_init));
        lh_ui_web_x_bin(lh_addr_of(ih), "|", lh_addr_of(zero), lh_addr_of(h_init));
        lh_ui_web_js_var_x(j, "w", lh_addr_of(w_init));
        lh_ui_web_js_var_x(j, "h", lh_addr_of(h_init));
        lh_ui_web_x_id("w", lh_addr_of(xw));
        lh_ui_web_x_id("h", lh_addr_of(xh));
        lh_ui_web_x_num(1, lh_addr_of(one));
        lh_ui_web_x_bin(lh_addr_of(xw), "<", lh_addr_of(one), lh_addr_of(w_lt));
        lh_ui_web_js_if_x(j, lh_addr_of(w_lt));
        lh_ui_web_js_assign_x(j, lh_addr_of(xw), lh_addr_of(one));
        lh_ui_web_js_end(j);
        lh_ui_web_x_bin(lh_addr_of(xh), "<", lh_addr_of(one), lh_addr_of(h_lt));
        lh_ui_web_js_if_x(j, lh_addr_of(h_lt));
        lh_ui_web_js_assign_x(j, lh_addr_of(xh), lh_addr_of(one));
        lh_ui_web_js_end(j);
        keys[0] = "w";
        keys[1] = "h";
        vals[0] = lh_addr_of(xw);
        vals[1] = lh_addr_of(xh);
        lh_ui_web_x_obj(keys, vals, 2u, lh_addr_of(obj));
        lh_ui_web_js_ret_x(j, lh_addr_of(obj));
    }
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "reportSize", "force");
    {
        lh_ui_web_x_t scale;
        lh_ui_web_x_t layout;
        lh_ui_web_x_t ne;
        lh_ui_web_x_t view;
        lh_ui_web_x_t xs;
        lh_ui_web_x_t force;
        lh_ui_web_x_t not_force;
        lh_ui_web_x_t sw;
        lh_ui_web_x_t sh;
        lh_ui_web_x_t last_zw;
        lh_ui_web_x_t last_zh;
        lh_ui_web_x_t eq_w;
        lh_ui_web_x_t eq_h;
        lh_ui_web_x_t same;
        lh_ui_web_x_t skip;
        lh_ui_web_x_t s_z;
        lh_ui_web_x_t s_sp;
        lh_ui_web_x_t t0;
        lh_ui_web_x_t t1;
        lh_ui_web_x_t t2;
        lh_ui_web_x_t emit;
        const lh_ui_web_x_t *emit_args[1];

        lh_ui_web_x_id("scaleMode", lh_addr_of(scale));
        lh_ui_web_x_str("layout", lh_addr_of(layout));
        lh_ui_web_x_bin(lh_addr_of(scale), "!==", lh_addr_of(layout), lh_addr_of(ne));
        lh_ui_web_js_if_x(j, lh_addr_of(ne));
        lh_ui_web_js_ret_x(j, lh_null);
        lh_ui_web_js_end(j);
        lh_ui_web_x_call_id("viewSize", lh_null, 0u, lh_addr_of(view));
        lh_ui_web_js_var_x(j, "s", lh_addr_of(view));
        lh_ui_web_x_id("s", lh_addr_of(xs));
        lh_ui_web_x_id("force", lh_addr_of(force));
        lh_ui_web_x_un("!", lh_addr_of(force), lh_addr_of(not_force));
        lh_ui_web_x_dot(lh_addr_of(xs), "w", lh_addr_of(sw));
        lh_ui_web_x_dot(lh_addr_of(xs), "h", lh_addr_of(sh));
        lh_ui_web_x_id("lastZW", lh_addr_of(last_zw));
        lh_ui_web_x_id("lastZH", lh_addr_of(last_zh));
        lh_ui_web_x_bin(lh_addr_of(sw), "===", lh_addr_of(last_zw), lh_addr_of(eq_w));
        lh_ui_web_x_bin(lh_addr_of(sh), "===", lh_addr_of(last_zh), lh_addr_of(eq_h));
        lh_ui_web_x_bin(lh_addr_of(eq_w), "&&", lh_addr_of(eq_h), lh_addr_of(same));
        lh_ui_web_x_bin(lh_addr_of(not_force), "&&", lh_addr_of(same), lh_addr_of(skip));
        lh_ui_web_js_if_x(j, lh_addr_of(skip));
        lh_ui_web_js_ret_x(j, lh_null);
        lh_ui_web_js_end(j);
        lh_ui_web_js_assign_x(j, lh_addr_of(last_zw), lh_addr_of(sw));
        lh_ui_web_js_assign_x(j, lh_addr_of(last_zh), lh_addr_of(sh));
        lh_ui_web_x_str("z ", lh_addr_of(s_z));
        lh_ui_web_x_str(" ", lh_addr_of(s_sp));
        lh_ui_web_x_bin(lh_addr_of(s_z), "+", lh_addr_of(sw), lh_addr_of(t0));
        lh_ui_web_x_bin(lh_addr_of(t0), "+", lh_addr_of(s_sp), lh_addr_of(t1));
        lh_ui_web_x_bin(lh_addr_of(t1), "+", lh_addr_of(sh), lh_addr_of(t2));
        emit_args[0] = lh_addr_of(t2);
        lh_ui_web_x_call_id("emit", emit_args, 1u, lh_addr_of(emit));
        lh_ui_web_js_stmt_x(j, lh_addr_of(emit));
    }
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_assign(
        j, "window.onresize",
        "function(){if(scaleMode==='layout')reportSize();else if(lastD)play(lastD);}");
    lh_ui_web_js_vars(j, "busy=0,rev=-1");

    /* -- load / poll / emit ---------------------------------------------------- */
    lh_ui_web_js_fun_open(j, "load", "");
    lh_ui_web_js_if(j, "busy");
    lh_ui_web_js_ret(j, "busy");
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "busy",
                        "fetch('/ui.json').then(function(r){return r.json();})"
                        ".then(show).then(function(){busy=0;},function(){busy=0;})");
    lh_ui_web_js_ret(j, "busy");
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "poll", "");
    lh_ui_web_js_stmt(j, "fetch('/ui.rev').then(function(r){return r.text();}).then(function(t){"
                         "var n=+t;if(n!==rev){rev=n;load();}})");
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "emit", "line");
    lh_ui_web_js_if(j, "sock&&sock.readyState===1");
    lh_ui_web_js_stmt(j, "sock.send(line)");
    lh_ui_web_js_ret(j, lh_null);
    lh_ui_web_js_end(j);
    lh_ui_web_js_var(j, "p", "line.split(' ')");
    lh_ui_web_js_if(j, "p[0]==='p'");
    lh_ui_web_js_ret(
        j, "fetch('/ui/press?x='+p[1]+'&y='+p[2]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='r'");
    lh_ui_web_js_ret(
        j, "fetch('/ui/release?x='+p[1]+'&y='+p[2]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='w'");
    lh_ui_web_js_ret(j, "fetch('/ui/wheel?x='+p[1]+'&y='+p[2]+'&dx='+p[3]+'&dy='+p[4])"
                        ".then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='m'");
    lh_ui_web_js_ret(
        j, "fetch('/ui/move?x='+p[1]+'&y='+p[2]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='k'");
    lh_ui_web_js_ret(j, "fetch('/ui/key?k='+p[1]+(p.length>2?'&p='+p[2]:''))"
                        ".then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='t'");
    lh_ui_web_js_ret(j, "fetch('/ui/text?c='+p[1]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='z'");
    lh_ui_web_js_ret(
        j, "fetch('/ui/size?w='+p[1]+'&h='+p[2]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    /* -- canvas handlers + websocket ------------------------------------------- */
    lh_ui_web_js_var(j, "c", "document.getElementById('c')");
    lh_ui_web_js_vars(j, "held=0,mv=null,mvRaf=0");
    lh_ui_web_js_fun_open(j, "flushMove", "");
    lh_ui_web_js_if(j, "mvRaf");
    lh_ui_web_js_stmt(j, "cancelAnimationFrame(mvRaf)");
    lh_ui_web_js_assign(j, "mvRaf", "0");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "mv");
    lh_ui_web_js_stmt(j, "emit('m '+mv.x+' '+mv.y)");
    lh_ui_web_js_assign(j, "mv", "null");
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);
    lh_ui_web_js_prop(j, "c", "onmousedown",
                      "function(e){var p=pos(e);held=1;emit('p '+p.x+' '+p.y);}");
    /* Up and move on the window, not the canvas: a drag that leaves the canvas is still the
       drag, as a captured pointer is on the desktop. Moves go only while a button is held, at
       most one per animation frame, and the last one goes before the release. */
    lh_ui_web_js_assign(j, "window.onmouseup",
                        "function(e){if(!held)return;held=0;flushMove();var p=pos(e);"
                        "emit('r '+p.x+' '+p.y);}");
    lh_ui_web_js_assign(j, "window.onmousemove",
                        "function(e){if(!held)return;if(e.buttons===0){window.onmouseup(e);return;}"
                        "mv=pos(e);if(!mvRaf)mvRaf=requestAnimationFrame(function(){mvRaf=0;"
                        "flushMove();});}");
    lh_ui_web_js_prop(
        j, "c", "onwheel",
        "function(e){e.preventDefault();var p=pos(e);"
        "var dy=e.deltaY===0?0:(e.deltaY>0?-1:1);var dx=e.deltaX===0?0:(e.deltaX>0?-1:1);"
        "if(e.shiftKey&&dy){dx=dy;dy=0;}emit('w '+p.x+' '+p.y+' '+dx+' '+dy);}");

    /* A key goes as `k <key> 1` down and `k <key> 0` up, and a key that also types (Space)
       sends its text after it, as the desktop's key-down and character messages do. A modifier
       keeps the browser's default; every other key the player takes. */
    lh_ui_web_page_emit_key_table(j, "keys", lh_bool_false);
    lh_ui_web_page_emit_key_table(j, "mods", lh_bool_true);
    lh_ui_web_js_assign(j, "document.onkeydown",
                        "function(e){var k=keys[e.key],m=mods[e.key];"
                        "var t=e.key&&e.key.length===1&&!e.ctrlKey&&!e.altKey&&!e.metaKey;"
                        "if((k&&!m)||t)e.preventDefault();"
                        "if(k)emit('k '+k+' 1');"
                        "if(t)emit('t '+e.key.codePointAt(0));}");
    lh_ui_web_js_assign(j, "document.onkeyup",
                        "function(e){var k=keys[e.key];if(!k)return;"
                        "if(!mods[e.key])e.preventDefault();emit('k '+k+' 0');}");
    lh_ui_web_js_stmt(j, "c.focus()");
    lh_ui_web_js_vars(j, "sock=null,retry=200,pollId=0,retryId=0");

    lh_ui_web_js_fun_open(j, "stopPoll", "");
    lh_ui_web_js_if(j, "pollId");
    lh_ui_web_js_stmt(j, "clearInterval(pollId)");
    lh_ui_web_js_assign(j, "pollId", "0");
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "startPoll", "");
    lh_ui_web_js_if(j, "!pollId");
    lh_ui_web_js_assign(j, "pollId", "setInterval(poll,80)");
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "armRetry", "");
    lh_ui_web_js_if(j, "retryId");
    lh_ui_web_js_ret(j, lh_null);
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "retryId", "setTimeout(function(){retryId=0;openWs();},retry)");
    lh_ui_web_js_assign(j, "retry", "Math.min(retry*2,5000)");
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "openWs", "");
    lh_ui_web_js_if(j, "sock&&(sock.readyState===0||sock.readyState===1)");
    lh_ui_web_js_ret(j, lh_null);
    lh_ui_web_js_end(j);
    lh_ui_web_buf_raw(j, "try{sock=new WebSocket((location.protocol==='https:'?'wss://':'ws://')"
                         "+location.host+'/ws');}\n"
                         "catch(e){sock=null;startPoll();armRetry();return;}\n");
    lh_ui_web_js_prop(j, "sock", "onopen", "function(){retry=200;stopPoll();reportSize(1);}");
    lh_ui_web_js_prop(j, "sock", "onmessage", "function(ev){show(JSON.parse(ev.data));}");
    lh_ui_web_js_prop(j, "sock", "onclose", "function(){sock=null;startPoll();armRetry();}");
    lh_ui_web_js_prop(j, "sock", "onerror", "function(){}");
    lh_ui_web_js_fun_close(j);

    /* Layout: HTTP size before WS so the first frame is not 980×620. */
    page_lit(
        j,
        "if(scaleMode==='layout'){(function(){var s=viewSize();lastZW=s.w;lastZH=s.h;"
        "fetch('/ui/size?w='+s.w+'&h='+s.h).then(function(){rev=-1;return load();},function(){})."
        "then(function(){openWs();},function(){openWs();});})();}\n"
        "else{load();openWs();}\n");

    lh_ui_web_html_close(j, lh_addr_of(el_script));
    lh_ui_web_html_close(j, lh_addr_of(el_body));
    lh_ui_web_html_close(j, lh_addr_of(el_html));

    lh_ui_web_buf_finish(j);
    if (!lh_ui_web_buf_ok(j))
    {
        return lh_bool_false;
    }
    if (out_len != lh_null)
    {
        lh_ptr_deref(out_len) = lh_ui_web_buf_len(j);
    }
    return lh_bool_true;
}
