/**
 * @file host.c
 * @brief Implementation of `lh/ui/web/host.h`.
 */

#include <lh/ui/web/host.h>
#include <lh/ui/web/ws.h>

#include <lh/cast/const.h>
#include <lh/crypto/sha.h>
#include <lh/memory.h>
#include <lh/net/ip.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/null.h>
#include <lh/os/alloc.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/result.h>
#include <lh/os/tick.h>
#include <lh/runtime/allocator.h>
#include <lh/runtime/logger.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/text.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/cast/static.h>

#define HOST_WS_MAX 8u
#define HOST_WS_RX (LH_UI_WEB_WS_MAX_MSG + 32u)
#define HOST_WS_QMAX (4u * 1024u * 1024u)
#define HOST_WS_PING_MS 15000u
#define HOST_WS_DEAD_MS 45000u
/* How long the loop sleeps when nothing at all happens: the tick's period. */
#define HOST_TICK_MS 20u
/* How long an accepted connection may take to send its request line. */
#define HOST_REQUEST_MS 2000u

typedef struct host_ws_q
{
    struct host_ws_q *next;
    unsigned char *bytes;
    unsigned len;
    unsigned off;
} host_ws_q_t;

typedef struct host_ws_cli
{
    lh_os_net_socket_t sock;
    unsigned char *rx;
    unsigned rx_len;
    unsigned char *msg;
    unsigned msg_len;
    unsigned msg_cap;
    unsigned msg_op;
    lh_bool_t msg_on;
    host_ws_q_t *qhead;
    host_ws_q_t *qtail;
    unsigned queued;
    unsigned seen;
    unsigned last_rx_ms;
    unsigned last_ping_ms;
    lh_bool_t used;
    lh_bool_t closing;
    lh_bool_t synced; /* got the last frame sent, so it holds every cover that frame assumed */
} host_ws_cli_t;

/* What the stream of WS frames shares between iterations. */
typedef struct host_ws_stream
{
    lh_ui_canvas_web_glyphs_t glyphs; /* covers every synced client already holds */
    char *last;                       /* bytes of the last frame sent */
    lh_usize_t last_len;
    lh_usize_t last_cap;
} host_ws_stream_t;

static const char lh_ui_web_page[] =
    "<!doctype html><html lang=\"ru\"><head><meta charset=\"utf-8\">"
    "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
    "<title>lh_ui</title>"
    "<style>html,body{margin:0;height:100%;background:#edf0f5;overflow:hidden}"
    "#c{display:block;position:absolute;cursor:pointer;outline:none}"
    "#v3d{position:absolute;display:none;pointer-events:none}</style></head><body>"
    "<canvas id=\"c\" width=\"980\" height=\"620\" tabindex=\"0\"></canvas>"
    "<canvas id=\"v3d\"></canvas>"
    "<script>\n"
    "function rgba(c){return 'rgba('+c[0]+','+c[1]+','+c[2]+','+(c[3]/255)+')';}\n"
    /* atlas: covers by id, kept for the life of the page (the host sends each
       once per connection). tints: one coloured copy per (id, colour), kept
       across frames too -- rebuilding them every frame was most of a frame. */
    "var atlas=[],tints={},ntint=0,fw=980,fh=620,cssS=1,lastD=null,pend=null,raf=0,v3on=0;\n"
    "function hexv(c){return c<58?c-48:(c|32)-87;}\n"
    "function decodeCover(w,h,cv){var im=document.createElement('canvas');im.width=w;im.height=h;"
    "if(w<=0||h<=0)return im;var g=im.getContext('2d'),id=g.createImageData(w,h),p=id.data,n=w*h,i,j,k;"
    "for(i=0,j=0;i<n;i++,j+=4){k=i*2;p[j]=p[j+1]=p[j+2]=255;"
    "p[j+3]=k+1<cv.length?(hexv(cv.charCodeAt(k))<<4)|hexv(cv.charCodeAt(k+1)):0;}"
    "g.putImageData(id,0,0);return im;}\n"
    /* Every frame that arrives is ingested, even one that is never drawn: a
       cover is only sent once and a skipped frame may be the one carrying it. */
    "function ingest(d){var ops=d.ops||[],i,op;for(i=0;i<ops.length;i++){op=ops[i];"
    "if(op.cover!==undefined&&typeof op.m==='number')atlas[op.m]=decodeCover(op.size[0],op.size[1],op.cover);}}\n"
    "function tint(a,c){var t=document.createElement('canvas');t.width=a.width;t.height=a.height;"
    "if(!a.width||!a.height)return t;"
    "var g=t.getContext('2d');g.fillStyle=rgba(c);g.fillRect(0,0,t.width,t.height);"
    "g.globalCompositeOperation='destination-in';g.drawImage(a,0,0);return t;}\n"
    "function glyph(op){var c=op.color;"
    "if(typeof op.m!=='number'){return tint(decodeCover(op.size[0],op.size[1],op.cover||''),c);}"
    "var k=op.m+','+c[0]+','+c[1]+','+c[2]+','+c[3],t=tints[k];if(t)return t;"
    "var a=atlas[op.m];if(!a)return null;"
    "if(ntint>=8192){tints={};ntint=0;}t=tint(a,c);tints[k]=t;++ntint;return t;}\n"
    "function round(x,r,rad){x.beginPath();if(x.roundRect)x.roundRect(r[0],r[1],r[2],r[3],rad||0);else x.rect(r[0],r[1],r[2],r[3]);}\n"
    "function pane3d(d){var v=document.getElementById('v3d'),r=d.viewport3d,c=document.getElementById('c');"
    "if(!r){if(v3on){v.style.display='none';v3on=0;}return;}v3on=1;"
    "var box=c.getBoundingClientRect(),dpr=window.devicePixelRatio||1;"
    "v.style.display='block';v.style.left=(box.left+r[0]*cssS)+'px';v.style.top=(box.top+r[1]*cssS)+'px';"
    "v.style.width=(r[2]*cssS)+'px';v.style.height=(r[3]*cssS)+'px';"
    "v.width=Math.max(1,Math.round(r[2]*cssS*dpr));v.height=Math.max(1,Math.round(r[3]*cssS*dpr));"
    "if(!v.gl)v.gl=v.getContext('webgl')||v.getContext('experimental-webgl');}\n"
    "function fit(){"
    "var c=document.getElementById('c'),vw=window.innerWidth,vh=window.innerHeight;"
    "var s=Math.min(vw/fw,vh/fh);if(!isFinite(s)||s<=0)s=1;cssS=s;"
    "var dpr=window.devicePixelRatio||1;"
    "var bw=Math.max(1,Math.round(fw*s*dpr)),bh=Math.max(1,Math.round(fh*s*dpr));"
    "if(c.width!==bw)c.width=bw;if(c.height!==bh)c.height=bh;"
    "c.style.width=Math.round(fw*s)+'px';c.style.height=Math.round(fh*s)+'px';"
    "c.style.left=Math.round((vw-fw*s)/2)+'px';c.style.top=Math.round((vh-fh*s)/2)+'px';"
    "return c.getContext('2d');}\n"
    /* Setting fillStyle parses a colour string; most neighbouring ops share one. */
    "function play(d){"
    "lastD=d;fw=d.width;fh=d.height;"
    "var x=fit(),ops=d.ops||[],i,op,s,fs=null,dpr=window.devicePixelRatio||1;"
    "x.setTransform(cssS*dpr,0,0,cssS*dpr,0,0);"
    "pane3d(d);"
    "for(i=0;i<ops.length;i++){op=ops[i];switch(op.op){"
    "case 'fill_mask':{var g=glyph(op);if(g)x.drawImage(g,op.origin[0],op.origin[1]);break;}"
    "case 'fill_rect':s=rgba(op.color);if(s!==fs){x.fillStyle=s;fs=s;}x.fillRect(op.rect[0],op.rect[1],op.rect[2],op.rect[3]);break;"
    "case 'fill_round_rect':s=rgba(op.color);if(s!==fs){x.fillStyle=s;fs=s;}round(x,op.rect,op.radius||0);x.fill();break;"
    "case 'clear':s=rgba(op.color);if(s!==fs){x.fillStyle=s;fs=s;}x.fillRect(0,0,fw,fh);break;"
    "case 'shadow':x.save();x.shadowColor=rgba(op.color);x.shadowBlur=op.spread||0;"
    "x.shadowOffsetX=op.offset?op.offset[0]:0;x.shadowOffsetY=op.offset?op.offset[1]:0;"
    "x.fillStyle=rgba(op.color);round(x,op.rect,op.radius||0);x.fill();x.restore();fs=null;break;"
    "}}"
    "}\n"
    /* Frames that arrive faster than the screen refreshes: draw the newest once. */
    "function show(d){ingest(d);pend=d;if(!raf)raf=requestAnimationFrame(function(){raf=0;var p=pend;pend=null;if(p)play(p);});}\n"
    "function pos(e){var c=document.getElementById('c'),r=c.getBoundingClientRect();"
    "return {x:Math.round((e.clientX-r.left)*fw/r.width),y:Math.round((e.clientY-r.top)*fh/r.height)};"
    "}\n"
    "window.onresize=function(){if(lastD)play(lastD);};\n"
    "var busy=0,rev=-1;\n"
    "function load(){if(busy)return busy;busy=fetch('/ui.json').then(function(r){return r.json();}).then(show).then(function(){busy=0;},function(){busy=0;});return busy;}\n"
    "function poll(){fetch('/ui.rev').then(function(r){return r.text();}).then(function(t){var n=+t;if(n!==rev){rev=n;load();}});}\n"
    "function emit(line){if(sock&&sock.readyState===1){sock.send(line);return;}"
    "var p=line.split(' ');"
    "if(p[0]==='p')return fetch('/ui/press?x='+p[1]+'&y='+p[2]).then(function(){rev=-1;return load();});"
    "if(p[0]==='r')return fetch('/ui/release?x='+p[1]+'&y='+p[2]).then(function(){rev=-1;return load();});"
    "if(p[0]==='w')return fetch('/ui/wheel?x='+p[1]+'&y='+p[2]+'&dx='+p[3]+'&dy='+p[4]).then(function(){rev=-1;return load();});"
    "if(p[0]==='k')return fetch('/ui/key?k='+p[1]).then(function(){rev=-1;return load();});"
    "if(p[0]==='t')return fetch('/ui/text?c='+p[1]).then(function(){rev=-1;return load();});"
    "}\n"
    "var c=document.getElementById('c');\n"
    "c.onmousedown=function(e){var p=pos(e);emit('p '+p.x+' '+p.y);};\n"
    "c.onmouseup=function(e){var p=pos(e);emit('r '+p.x+' '+p.y);};\n"
    "c.onwheel=function(e){e.preventDefault();var p=pos(e);"
    "var dy=e.deltaY===0?0:(e.deltaY>0?-1:1);var dx=e.deltaX===0?0:(e.deltaX>0?-1:1);"
    "if(e.shiftKey&&dy){dx=dy;dy=0;}emit('w '+p.x+' '+p.y+' '+dx+' '+dy);};\n"
    "var keys={Tab:1,Enter:2,Escape:3,Backspace:4,Delete:5,ArrowLeft:7,ArrowRight:8,ArrowUp:9,ArrowDown:10,Home:11,End:12,PageUp:13,PageDown:14};\n"
    "document.onkeydown=function(e){"
    "var k=keys[e.key];"
    "if(k){e.preventDefault();emit('k '+k);return;}"
    "if(e.key&&e.key.length===1&&!e.ctrlKey&&!e.altKey&&!e.metaKey){"
    "e.preventDefault();emit('t '+e.key.codePointAt(0));}"
    "};\n"
    "c.focus();\n"
    "var sock=null,retry=200,pollId=0,retryId=0;\n"
    "function stopPoll(){if(pollId){clearInterval(pollId);pollId=0;}}\n"
    "function startPoll(){if(!pollId)pollId=setInterval(poll,80);}\n"
    "function armRetry(){if(retryId)return;retryId=setTimeout(function(){retryId=0;openWs();},retry);"
    "retry=Math.min(retry*2,5000);}\n"
    "function openWs(){"
    "if(sock&&(sock.readyState===0||sock.readyState===1))return;"
    "try{sock=new WebSocket((location.protocol==='https:'?'wss://':'ws://')+location.host+'/ws');}"
    "catch(e){sock=null;startPoll();armRetry();return;}"
    "sock.onopen=function(){retry=200;stopPoll();};"
    "sock.onmessage=function(ev){show(JSON.parse(ev.data));};"
    "sock.onclose=function(){sock=null;startPoll();armRetry();};"
    "sock.onerror=function(){};"
    "}\n"
    "load();\n"
    "openWs();\n"
    "</script></body></html>";

/* A literal and its length, for the wrappers that take both. */
#define host_lit(s) (s), lh_cast_static(lh_usize_t, (sizeof(s) - 1u))

static lh_void
host_send_all(lh_os_net_socket_t *client, const char *p, lh_usize_t left)
{
    while (left > 0u)
    {
        const lh_ssize_t w = lh_os_net_socket_send(client, lh_cast_const(lh_ptr, p), left);

        if (w <= 0)
        {
            break;
        }
        p += lh_cast_static(lh_usize_t, w);
        left -= lh_cast_static(lh_usize_t, w);
    }
}

static lh_void
host_reply(lh_os_net_socket_t *client, unsigned code, const char *type, const char *body, lh_usize_t len)
{
    char head[256];
    const char *reason =
        (code == 200u) ? "OK" : ((code == 304u) ? "Not Modified" : ((code == 500u) ? "Error" : "Not Found"));
    const lh_usize_t n = lh_str_ptr_format_text(head, sizeof(head),
                                                "HTTP/1.1 %u %s\r\nContent-Type: %s\r\nContent-Length: %u\r\n"
                                                "Connection: close\r\nCache-Control: no-store\r\n\r\n",
                                                lh_cast_static(lh_uint_t, code), reason, type, lh_cast_static(lh_uint_t, len));

    host_send_all(client, head, n);
    host_send_all(client, body, len);
}

static lh_void
host_reply_text(lh_os_net_socket_t *client, unsigned code, const char *text, lh_usize_t len)
{
    host_reply(client, code, "text/plain; charset=utf-8", text, len);
}

/* Does @p p (of @p len bytes) start with @p lit? */
static lh_bool_t
host_starts(const char *p, lh_usize_t len, const char *lit, lh_usize_t lit_len)
{
    return (len >= lit_len &&
            lh_memory_compare(lh_cast_const(lh_ptr, p), lit_len, lh_cast_const(lh_ptr, lit), lit_len) == lh_null)
               ? lh_bool_true
               : lh_bool_false;
}

/* The first whole @p needle in @p p, or lh_null. lh_memory_find compares a
   shortened needle at the tail, so a match running past the end is no match. */
static const char *
host_find(const char *p, lh_usize_t len, const char *needle, lh_usize_t needle_len)
{
    const char *at = lh_ptr_rcast(const char, lh_memory_find(lh_cast_const(lh_ptr, p), len,
                                                              lh_cast_const(lh_ptr, needle), needle_len));

    return (at != lh_null && lh_cast_static(lh_usize_t, (at - p))+ needle_len <= len) ? at : lh_null;
}

/* `GET <path>` or `POST <path>` at the start of the request. */
static lh_bool_t
host_route(const char *req, lh_usize_t len, const char *path, lh_usize_t path_len)
{
    if (host_starts(req, len, host_lit("GET ")))
    {
        return host_starts(req + 4u, len - 4u, path, path_len);
    }
    if (host_starts(req, len, host_lit("POST ")))
    {
        return host_starts(req + 5u, len - 5u, path, path_len);
    }
    return lh_bool_false;
}

/* The value of query parameter @p key (written `x=`), as text up to the next
   `&`, space or line end. Only a key right after `?` or `&` counts, so `x=`
   is never found inside `dx=`. */
static lh_usize_t
host_query(const char *req, lh_usize_t len, const char *key, lh_usize_t key_len, const char **value)
{
    lh_usize_t off = 0u;

    for (;;)
    {
        const char *at = host_find(req + off, len - off, key, key_len);
        lh_usize_t n = 0u;

        if (at == lh_null)
        {
            return 0u;
        }
        off = lh_cast_static(lh_usize_t, (at - req))+ key_len;
        if (at == req || (at[-1] != '?' && at[-1] != '&'))
        {
            continue;
        }
        while (off + n < len && req[off + n] != '&' && req[off + n] != ' ' && req[off + n] != '\r' &&
               req[off + n] != '\n')
        {
            ++n;
        }
        *value = req + off;
        return n;
    }
}

static int
host_query_int(const char *req, lh_usize_t len, const char *key, lh_usize_t key_len)
{
    const char *value = lh_null;
    const lh_usize_t n = host_query(req, len, key, key_len, lh_addr_of(value));
    lh_sint_t v = 0;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%d", lh_addr_of(v)) == 0u)
    {
        return 0;
    }
    return (int)v;
}

static unsigned
host_query_uint(const char *req, lh_usize_t len, const char *key, lh_usize_t key_len)
{
    const char *value = lh_null;
    const lh_usize_t n = host_query(req, len, key, key_len, lh_addr_of(value));
    lh_uint_t v = 0u;

    if (n == 0u || lh_str_ptr_parse_text(value, n, "%u", lh_addr_of(v)) == 0u)
    {
        return 0u;
    }
    return (unsigned)v;
}

static lh_ui_point_t
host_point(int x, int y)
{
    lh_ui_point_t point;

    lh_ui_point_init(lh_addr_of(point), lh_cast_static(lh_ui_scalar_t, x), lh_cast_static(lh_ui_scalar_t, y));
    return point;
}

static lh_void
host_press(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, int x, int y)
{
    if (hooks != lh_null && hooks->press != lh_null)
    {
        hooks->press(x, y, hooks->ctx);
        return;
    }
    lh_ui_view_press(view, host_point(x, y));
}

static lh_void
host_release(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, int x, int y)
{
    if (hooks != lh_null && hooks->release != lh_null)
    {
        hooks->release(x, y, hooks->ctx);
        return;
    }
    (void)lh_ui_view_release(view, host_point(x, y));
}

static lh_void
host_wheel(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, int x, int y, int dx, int dy)
{
    if (hooks != lh_null && hooks->wheel != lh_null)
    {
        hooks->wheel(x, y, dx, dy, hooks->ctx);
        return;
    }
    (void)lh_ui_view_wheel(view, host_point(x, y), lh_cast_static(lh_ui_scalar_t, dx), lh_cast_static(lh_ui_scalar_t, dy));
}

static lh_void
host_key(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, int k)
{
    if (k < lh_cast_static(int, lh_key_other)|| k > lh_cast_static(int, lh_key_alt))
    {
        k = lh_cast_static(int, lh_key_other);
    }
    if (hooks != lh_null && hooks->key != lh_null)
    {
        hooks->key(lh_cast_static(lh_key_t, k), lh_bool_true, hooks->ctx);
        return;
    }
    (void)lh_ui_view_key(view, lh_cast_static(lh_key_t, k), lh_bool_true);
}

static lh_void
host_text(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, lh_u32_t code)
{
    if (code == 0u)
    {
        return;
    }
    if (hooks != lh_null && hooks->text != lh_null)
    {
        hooks->text(code, hooks->ctx);
        return;
    }
    (void)lh_ui_view_text(view, code);
}

static lh_void
host_bump(unsigned *gen)
{
    if (gen != lh_null)
    {
        ++*gen;
    }
}

static lh_void
host_b64_sha1(const unsigned char *in, char *out)
{
    static const char tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned i;
    unsigned o = 0;

    for (i = 0; i < 18u; i += 3u)
    {
        const unsigned n = (lh_cast_static(unsigned, in[i])<< 16) | (lh_cast_static(unsigned, in[i + 1u])<< 8) | lh_cast_static(unsigned, in[i + 2u]);

        out[o++] = tab[(n >> 18) & 63u];
        out[o++] = tab[(n >> 12) & 63u];
        out[o++] = tab[(n >> 6) & 63u];
        out[o++] = tab[n & 63u];
    }
    {
        const unsigned n = (lh_cast_static(unsigned, in[18])<< 16) | (lh_cast_static(unsigned, in[19])<< 8);

        out[o++] = tab[(n >> 18) & 63u];
        out[o++] = tab[(n >> 12) & 63u];
        out[o++] = tab[(n >> 6) & 63u];
        out[o++] = '=';
        out[o] = 0;
    }
}

static lh_bool_t
host_ws_upgrade(lh_os_net_socket_t *client, const char *req, lh_usize_t len)
{
    static const char guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const char *k = host_find(req, len, host_lit("Sec-WebSocket-Key:"));
    char cat[80];
    unsigned char digest[LH_CRYPTO_SHA1_DIGEST_SIZE];
    char accept[32];
    char head[280];
    lh_usize_t n = 0u;
    lh_usize_t h;

    if (k == lh_null)
    {
        k = host_find(req, len, host_lit("sec-websocket-key:"));
    }
    if (k == lh_null)
    {
        return lh_bool_false;
    }
    k += sizeof("Sec-WebSocket-Key:") - 1u;
    while (k < req + len && *k == ' ')
    {
        ++k;
    }
    while (k + n < req + len && k[n] != '\r' && k[n] != '\n' && n < sizeof(cat) - sizeof(guid))
    {
        cat[n] = k[n];
        ++n;
    }
    lh_memory_copy(cat + n, sizeof(cat) - n, lh_cast_const(lh_ptr, guid), sizeof(guid) - 1u);
    n += sizeof(guid) - 1u;
    if (lh_crypto_sha_hash(lh_crypto_sha_kind_1, cat, n, digest, sizeof(digest)) != LH_CRYPTO_SHA1_DIGEST_SIZE)
    {
        return lh_bool_false;
    }
    host_b64_sha1(digest, accept);
    h = lh_str_ptr_format_text(head, sizeof(head),
                               "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\n"
                               "Connection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n",
                               accept);
    if (h == 0u)
    {
        return lh_bool_false;
    }
    host_send_all(client, head, h);
    (void)lh_os_net_socket_set_blocking(client, lh_bool_false);
    return lh_bool_true;
}

/* One input from the player: `p x y`, `r x y`, `w x y dx dy`, `k key`, `t code`. */
static lh_void
host_event_line(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, unsigned *gen, const char *line,
                lh_usize_t len)
{
    lh_sint_t x = 0;
    lh_sint_t y = 0;
    lh_sint_t dx = 0;
    lh_sint_t dy = 0;
    lh_sint_t k = 0;
    lh_uint_t code = 0u;

    if (lh_str_ptr_parse_text(line, len, "p %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        host_press(view, hooks, lh_cast_static(int, x), lh_cast_static(int, y));
    }
    else if (lh_str_ptr_parse_text(line, len, "r %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        host_release(view, hooks, lh_cast_static(int, x), lh_cast_static(int, y));
    }
    else if (lh_str_ptr_parse_text(line, len, "w %d %d %d %d", lh_addr_of(x), lh_addr_of(y), lh_addr_of(dx),
                                   lh_addr_of(dy)) != 0u)
    {
        host_wheel(view, hooks, lh_cast_static(int, x), lh_cast_static(int, y), lh_cast_static(int, dx), lh_cast_static(int, dy));
    }
    else if (lh_str_ptr_parse_text(line, len, "k %d", lh_addr_of(k)) != 0u)
    {
        host_key(view, hooks, lh_cast_static(int, k));
    }
    else if (lh_str_ptr_parse_text(line, len, "t %u", lh_addr_of(code)) != 0u)
    {
        host_text(view, hooks, lh_cast_static(lh_u32_t, code));
    }
    else
    {
        return;
    }
    host_bump(gen);
}

static lh_void
host_ws_drop(host_ws_cli_t *c)
{
    host_ws_q_t *q;

    if (c == lh_null || !c->used)
    {
        return;
    }
    lh_os_net_socket_close(lh_addr_of(c->sock));
    lh_os_net_socket_init(lh_addr_of(c->sock));
    q = c->qhead;
    while (q != lh_null)
    {
        host_ws_q_t *n = q->next;

        lh_runtime_allocator_free(q->bytes);
        lh_runtime_allocator_free(q);
        q = n;
    }
    lh_runtime_allocator_free(c->rx);
    lh_runtime_allocator_free(c->msg);
    c->rx = lh_null;
    c->msg = lh_null;
    c->rx_len = 0u;
    c->msg_len = 0u;
    c->msg_cap = 0u;
    c->msg_on = lh_bool_false;
    c->qhead = lh_null;
    c->qtail = lh_null;
    c->queued = 0u;
    c->used = lh_bool_false;
    c->closing = lh_bool_false;
}

static lh_bool_t
host_ws_enqueue(host_ws_cli_t *c, unsigned opcode, const void *payload, lh_usize_t plen, lh_bool_t front)
{
    const lh_usize_t need = lh_ui_web_ws_frame_size(plen);
    host_ws_q_t *q;
    unsigned char *bytes;

    if (c == lh_null || !c->used)
    {
        return lh_bool_false;
    }
    if (need == 0u || c->queued + lh_cast_static(unsigned, need)> HOST_WS_QMAX)
    {
        host_ws_drop(c);
        return lh_bool_false;
    }
    q = lh_ptr_rcast(host_ws_q_t, lh_os_alloc(sizeof(*q)));
    bytes = lh_ptr_rcast(unsigned char, lh_os_alloc(need));
    if (q == lh_null || bytes == lh_null ||
        lh_ui_web_ws_write(bytes, need, opcode, lh_bool_true, payload, plen) != need)
    {
        lh_runtime_allocator_free(q);
        lh_runtime_allocator_free(bytes);
        host_ws_drop(c);
        return lh_bool_false;
    }
    q->bytes = bytes;
    q->len = lh_cast_static(unsigned, need);
    q->off = 0u;
    if (front)
    {
        q->next = c->qhead;
        c->qhead = q;
        if (c->qtail == lh_null)
        {
            c->qtail = q;
        }
    }
    else
    {
        q->next = lh_null;
        if (c->qtail != lh_null)
        {
            c->qtail->next = q;
        }
        else
        {
            c->qhead = q;
        }
        c->qtail = q;
    }
    c->queued += lh_cast_static(unsigned, need);
    return lh_bool_true;
}

static lh_void
host_ws_flush(host_ws_cli_t *c)
{
    while (c != lh_null && c->used && c->qhead != lh_null)
    {
        host_ws_q_t *q = c->qhead;
        const lh_ssize_t w =
            lh_os_net_socket_send(lh_addr_of(c->sock), q->bytes + q->off, lh_cast_static(lh_usize_t, (q->len - q->off)));

        if (w == LH_OS_RESULT_PENDING)
        {
            return;
        }
        if (w <= 0)
        {
            host_ws_drop(c);
            return;
        }
        q->off += lh_cast_static(unsigned, w);
        c->queued -= lh_cast_static(unsigned, w);
        if (q->off >= q->len)
        {
            c->qhead = q->next;
            if (c->qhead == lh_null)
            {
                c->qtail = lh_null;
            }
            lh_runtime_allocator_free(q->bytes);
            lh_runtime_allocator_free(q);
        }
    }
    if (c != lh_null && c->used && c->closing && c->qhead == lh_null)
    {
        host_ws_drop(c);
    }
}

static lh_bool_t
host_ws_msg_add(host_ws_cli_t *c, const unsigned char *p, lh_usize_t n)
{
    unsigned char *grow;

    if (c->msg_len + lh_cast_static(unsigned, n)> LH_UI_WEB_WS_MAX_MSG)
    {
        return lh_bool_false;
    }
    if (c->msg_len + lh_cast_static(unsigned, n)> c->msg_cap)
    {
        const unsigned cap = c->msg_len + lh_cast_static(unsigned, n);
        const unsigned next = (c->msg_cap == 0u) ? 256u : (c->msg_cap * 2u);
        const unsigned want = (cap > next) ? cap : next;

        grow = lh_ptr_rcast(unsigned char, lh_runtime_allocator_realloc(c->msg, c->msg_cap, want));
        if (grow == lh_null)
        {
            return lh_bool_false;
        }
        c->msg = grow;
        c->msg_cap = want;
    }
    lh_memory_copy(c->msg + c->msg_len, c->msg_cap - c->msg_len, lh_cast_const(lh_ptr, p), n);
    c->msg_len += lh_cast_static(unsigned, n);
    return lh_bool_true;
}

static lh_void
host_ws_ingest(host_ws_cli_t *c, lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, unsigned *gen,
               unsigned now)
{
    while (c->used && c->rx_len >= 2u)
    {
        unsigned opcode = 0;
        lh_bool_t fin = lh_bool_false;
        const unsigned char *payload = lh_null;
        lh_usize_t plen = 0;
        const unsigned char *mask = lh_null;
        const lh_usize_t n = lh_ui_web_ws_read(c->rx, c->rx_len, lh_addr_of(opcode), lh_addr_of(fin),
                                               lh_addr_of(payload), lh_addr_of(plen), lh_addr_of(mask));
        unsigned char *body;

        if (n == 0u)
        {
            return;
        }
        if (n == lh_cast_static(lh_usize_t, -1))
        {
            host_ws_drop(c);
            return;
        }
        body = lh_cast_const(unsigned char *, payload);
        if (mask != lh_null)
        {
            lh_ui_web_ws_unmask(body, plen, mask);
        }
        c->last_rx_ms = now;
        if (opcode >= 8u)
        {
            if (!fin || plen > 125u)
            {
                host_ws_drop(c);
                return;
            }
            if (opcode == LH_UI_WEB_WS_PING)
            {
                if (!host_ws_enqueue(c, LH_UI_WEB_WS_PONG, body, plen, lh_bool_true))
                {
                    return;
                }
            }
            else if (opcode == LH_UI_WEB_WS_CLOSE)
            {
                c->closing = lh_bool_true;
                if (!host_ws_enqueue(c, LH_UI_WEB_WS_CLOSE, body, plen, lh_bool_true))
                {
                    return;
                }
            }
        }
        else if (opcode == LH_UI_WEB_WS_CONT)
        {
            if (!c->msg_on || !host_ws_msg_add(c, body, plen))
            {
                host_ws_drop(c);
                return;
            }
            if (fin)
            {
                if (c->msg_op == LH_UI_WEB_WS_TEXT)
                {
                    host_event_line(view, hooks, gen, lh_ptr_rcast(const char, c->msg), c->msg_len);
                }
                c->msg_on = lh_bool_false;
                c->msg_len = 0u;
            }
        }
        else if (opcode == LH_UI_WEB_WS_TEXT || opcode == LH_UI_WEB_WS_BIN)
        {
            if (c->msg_on)
            {
                host_ws_drop(c);
                return;
            }
            if (fin)
            {
                if (opcode == LH_UI_WEB_WS_TEXT)
                {
                    host_event_line(view, hooks, gen, lh_ptr_rcast(const char, body), plen);
                }
            }
            else
            {
                c->msg_on = lh_bool_true;
                c->msg_op = opcode;
                c->msg_len = 0u;
                if (!host_ws_msg_add(c, body, plen))
                {
                    host_ws_drop(c);
                    return;
                }
            }
        }
        else
        {
            host_ws_drop(c);
            return;
        }
        lh_memory_move(c->rx, HOST_WS_RX, c->rx + n, lh_cast_static(lh_usize_t, (c->rx_len - lh_cast_static(unsigned, n))));
        c->rx_len -= lh_cast_static(unsigned, n);
    }
}

static lh_void
host_ws_attach(host_ws_cli_t *slots, lh_os_net_socket_t *client, unsigned now)
{
    unsigned i;

    for (i = 0u; i < HOST_WS_MAX; ++i)
    {
        host_ws_cli_t *c = slots + i;

        if (c->used)
        {
            continue;
        }
        c->rx = lh_ptr_rcast(unsigned char, lh_os_alloc(HOST_WS_RX));
        if (c->rx == lh_null)
        {
            lh_os_net_socket_close(client);
            return;
        }
        c->sock = *client;
        c->rx_len = 0u;
        c->msg = lh_null;
        c->msg_len = 0u;
        c->msg_cap = 0u;
        c->msg_on = lh_bool_false;
        c->qhead = lh_null;
        c->qtail = lh_null;
        c->queued = 0u;
        c->seen = 0u;
        c->last_rx_ms = now;
        c->last_ping_ms = now;
        c->used = lh_bool_true;
        c->closing = lh_bool_false;
        c->synced = lh_bool_false;
        return;
    }
    lh_os_net_socket_close(client);
}

static lh_void
host_ws_pump(host_ws_cli_t *c, lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, unsigned *gen,
             unsigned now)
{
    lh_ssize_t got;

    if (!c->used)
    {
        return;
    }
    if ((now - c->last_rx_ms) > HOST_WS_DEAD_MS)
    {
        host_ws_drop(c);
        return;
    }
    if (!c->closing && (now - c->last_ping_ms) >= HOST_WS_PING_MS)
    {
        if (!host_ws_enqueue(c, LH_UI_WEB_WS_PING, lh_null, 0u, lh_bool_true))
        {
            return;
        }
        c->last_ping_ms = now;
    }
    if (c->rx_len >= HOST_WS_RX)
    {
        host_ws_drop(c);
        return;
    }
    got = lh_os_net_socket_recv(lh_addr_of(c->sock), c->rx + c->rx_len, HOST_WS_RX - c->rx_len);
    if (got == LH_OS_RESULT_PENDING)
    {
        host_ws_flush(c);
        return;
    }
    if (got <= 0)
    {
        host_ws_drop(c);
        return;
    }
    c->rx_len += lh_cast_static(unsigned, got);
    host_ws_ingest(c, view, hooks, gen, now);
    if (c->used)
    {
        host_ws_flush(c);
    }
}

static lh_bool_t
host_draw(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
          lh_ui_canvas_web_t *web, lh_ui_canvas_web_glyphs_t *glyphs)
{
    lh_ui_canvas_t canvas;

    if (!lh_ui_canvas_web_open_mem(web, json, json_cap, width, height))
    {
        return lh_bool_false;
    }
    lh_ui_canvas_web_set_glyphs(web, glyphs);
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_web), web);
    lh_ui_view_set_canvas(view, lh_addr_of(canvas));
    lh_ui_view_draw(view, lh_null);
    lh_ui_view_set_canvas(view, lh_null);
    return lh_ui_canvas_web_close(web);
}

/* An input over HTTP: the same five as a WS line, with the numbers in the query. */
static lh_bool_t
host_handle_input(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, const char *req, lh_usize_t len)
{
    const int x = host_query_int(req, len, host_lit("x="));
    const int y = host_query_int(req, len, host_lit("y="));

    if (host_route(req, len, host_lit("/ui/click")))
    {
        host_press(view, hooks, x, y);
        host_release(view, hooks, x, y);
    }
    else if (host_route(req, len, host_lit("/ui/press")))
    {
        host_press(view, hooks, x, y);
    }
    else if (host_route(req, len, host_lit("/ui/release")))
    {
        host_release(view, hooks, x, y);
    }
    else if (host_route(req, len, host_lit("/ui/wheel")))
    {
        host_wheel(view, hooks, x, y, host_query_int(req, len, host_lit("dx=")),
                   host_query_int(req, len, host_lit("dy=")));
    }
    else if (host_route(req, len, host_lit("/ui/key")))
    {
        host_key(view, hooks, host_query_int(req, len, host_lit("k=")));
    }
    else if (host_route(req, len, host_lit("/ui/text")))
    {
        host_text(view, hooks, lh_cast_static(lh_u32_t, host_query_uint(req, len, host_lit("c="))));
    }
    else
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

static lh_bool_t
host_handle(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
            const lh_ui_web_host_hooks_t *hooks, unsigned *gen, lh_os_net_socket_t *client)
{
    char req[1024];
    lh_os_net_socket_t *const watch[1] = {client};
    lh_ui_canvas_web_t web;
    lh_ssize_t got;
    lh_usize_t len;

    /* Accepted the moment it connected, so the request may not be here yet; and
       a socket accepted from a non-blocking listener is non-blocking on Windows,
       so a bare recv would answer "not yet" and drop the request. */
    (void)lh_os_net_socket_set_blocking(client, lh_bool_true);
    (void)lh_os_net_socket_set_no_delay(client, lh_bool_true);
    if (lh_os_net_socket_wait(watch, 1u, lh_null, 0u, HOST_REQUEST_MS) <= 0)
    {
        return lh_bool_false;
    }
    got = lh_os_net_socket_recv(client, req, sizeof(req));
    if (got <= 0)
    {
        return lh_bool_false;
    }
    len = lh_cast_static(lh_usize_t, got);
    if (host_route(req, len, host_lit("/ws")))
    {
        return host_ws_upgrade(client, req, len);
    }
    if (host_handle_input(view, hooks, req, len))
    {
        host_bump(gen);
        host_reply_text(client, 200u, host_lit("ok\n"));
        return lh_bool_false;
    }
    if (host_route(req, len, host_lit("/ui.rev")))
    {
        char body[32];
        const lh_usize_t n = lh_str_ptr_format_text(body, sizeof(body), "%u\n", lh_cast_static(lh_uint_t, (gen != lh_null ? *gen : 0u)));

        host_reply_text(client, 200u, body, n);
        return lh_bool_false;
    }
    if (host_route(req, len, host_lit("/ui.json")))
    {
        if (gen != lh_null && host_query_uint(req, len, host_lit("rev=")) == *gen)
        {
            host_reply_text(client, 304u, host_lit(""));
            return lh_bool_false;
        }
        if (!host_draw(view, width, height, json, json_cap, lh_addr_of(web), lh_null))
        {
            host_reply_text(client, 500u, host_lit("draw failed\n"));
            return lh_bool_false;
        }
        host_reply(client, 200u, "application/json; charset=utf-8", lh_ui_canvas_web_bytes(lh_addr_of(web)),
                   lh_ui_canvas_web_length(lh_addr_of(web)));
        return lh_bool_false;
    }
    if (host_route(req, len, host_lit("/ ")) || host_route(req, len, host_lit("/index.html")))
    {
        host_reply(client, 200u, "text/html; charset=utf-8", host_lit(lh_ui_web_page));
        return lh_bool_false;
    }
    host_reply_text(client, 404u, host_lit("not found\n"));
    return lh_bool_false;
}

/* Is @p c waiting for a frame it can take now? A client still sending the last
   one is skipped and gets the newest when it is done. */
static lh_bool_t
host_ws_wants(const host_ws_cli_t *c, unsigned gen)
{
    return (c->used && !c->closing && c->seen != gen && c->qhead == lh_null) ? lh_bool_true : lh_bool_false;
}

static lh_void
host_ws_remember(host_ws_stream_t *stream, const char *body, lh_usize_t len)
{
    if (len > stream->last_cap)
    {
        char *grow = lh_ptr_rcast(char, lh_runtime_allocator_realloc(stream->last, stream->last_cap, len));

        if (grow == lh_null)
        {
            stream->last_len = 0u;
            return;
        }
        stream->last = grow;
        stream->last_cap = len;
    }
    lh_memory_copy(stream->last, stream->last_cap, lh_cast_const(lh_ptr, body), len);
    stream->last_len = len;
}

static lh_bool_t
host_ws_same(const host_ws_stream_t *stream, const char *body, lh_usize_t len)
{
    return (len > 0u && len == stream->last_len &&
            lh_memory_compare(lh_cast_const(lh_ptr, body), len, stream->last, len) == lh_null)
               ? lh_bool_true
               : lh_bool_false;
}

/* Draw once for every client that wants a frame and send it. Covers go once per
   connection: a client that missed a frame, or is new, resets the set so this
   one carries them all. A frame equal to the last one sent is not sent again. */
static lh_void
host_ws_frame(host_ws_cli_t *cli, host_ws_stream_t *stream, lh_ui_view_t *view, unsigned width,
              unsigned height, char *json, lh_usize_t json_cap, unsigned gen)
{
    lh_ui_canvas_web_t web;
    lh_bool_t want = lh_bool_false;
    lh_bool_t same;
    const char *body;
    lh_usize_t len;
    unsigned i;

    for (i = 0u; i < HOST_WS_MAX; ++i)
    {
        if (host_ws_wants(cli + i, gen))
        {
            want = lh_bool_true;
            if (!cli[i].synced)
            {
                lh_ui_canvas_web_glyphs_reset(lh_addr_of(stream->glyphs));
            }
        }
    }
    if (!want || !host_draw(view, width, height, json, json_cap, lh_addr_of(web), lh_addr_of(stream->glyphs)))
    {
        return;
    }
    body = lh_ui_canvas_web_bytes(lh_addr_of(web));
    len = lh_ui_canvas_web_length(lh_addr_of(web));
    same = host_ws_same(stream, body, len);
    for (i = 0u; i < HOST_WS_MAX; ++i)
    {
        host_ws_cli_t *c = cli + i;

        if (!c->used)
        {
            continue;
        }
        if (!host_ws_wants(c, gen))
        {
            /* Not sent this one: any cover it introduced, this client lacks. */
            if (!same)
            {
                c->synced = lh_bool_false;
            }
            continue;
        }
        if (same && c->synced)
        {
            c->seen = gen;
            continue;
        }
        if (host_ws_enqueue(c, LH_UI_WEB_WS_TEXT, body, len, lh_bool_false))
        {
            c->seen = gen;
            c->synced = lh_bool_true;
            host_ws_flush(c);
        }
    }
    if (!same)
    {
        host_ws_remember(stream, body, len);
    }
}

static lh_void
host_accept_all(lh_os_net_socket_t *listen_fd, host_ws_cli_t *cli, lh_ui_view_t *view, unsigned width,
                unsigned height, char *json, lh_usize_t json_cap, const lh_ui_web_host_hooks_t *hooks,
                unsigned *gen, unsigned now)
{
    for (;;)
    {
        lh_os_net_socket_t client;
        lh_net_ip4_socket_addr_t peer;

        lh_os_net_socket_init(lh_addr_of(client));
        if (!lh_os_net_socket_accept(listen_fd, lh_addr_of(client), lh_addr_of(peer)))
        {
            return;
        }
        if (host_handle(view, width, height, json, json_cap, hooks, gen, lh_addr_of(client)))
        {
            host_ws_attach(cli, lh_addr_of(client), now);
        }
        else
        {
            lh_os_net_socket_close(lh_addr_of(client));
        }
    }
}

/* Sleep until a connection, a client's input or room for its queued output --
   or @p timeout_ms, whichever is first. This is what answers an input in the
   time it takes to draw it, rather than in up to one tick. A wait that fails
   (a client socket gone bad under it) waits on the listener alone, so the loop
   still sleeps instead of spinning. */
static lh_void
host_wait(lh_os_net_socket_t *listen_fd, host_ws_cli_t *cli, unsigned timeout_ms)
{
    lh_os_net_socket_t *read[HOST_WS_MAX + 1u];
    lh_os_net_socket_t *write[HOST_WS_MAX];
    lh_usize_t nr = 0u;
    lh_usize_t nw = 0u;
    unsigned i;

    read[nr++] = listen_fd;
    for (i = 0u; i < HOST_WS_MAX; ++i)
    {
        if (!cli[i].used)
        {
            continue;
        }
        read[nr++] = lh_addr_of(cli[i].sock);
        if (cli[i].qhead != lh_null)
        {
            write[nw++] = lh_addr_of(cli[i].sock);
        }
    }
    if (lh_os_net_socket_wait(read, nr, write, nw, lh_cast_static(lh_u32_t, timeout_ms)) == LH_OS_RESULT_INVALID)
    {
        (void)lh_os_net_socket_wait(read, 1u, lh_null, 0u, lh_cast_static(lh_u32_t, timeout_ms));
    }
}

lh_bool_t
lh_ui_web_host_run(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
                   lh_net_port_t port, const lh_ui_web_host_hooks_t *hooks)
{
    lh_os_net_socket_t listen_fd;
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t addr;

    if (view == lh_null || json == lh_null || json_cap < 64u)
    {
        return lh_bool_false;
    }
    if (!lh_os_net_init())
    {
        return lh_bool_false;
    }
    lh_os_net_socket_init(lh_addr_of(listen_fd));
    if (!lh_os_net_socket_open(lh_addr_of(listen_fd), lh_os_system_net_socket_type_tcp))
    {
        return lh_bool_false;
    }
    (void)lh_os_net_socket_set_reuse_addr(lh_addr_of(listen_fd), lh_bool_true);
    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), port);
    if (!lh_os_net_socket_bind(lh_addr_of(listen_fd), lh_addr_of(addr)) ||
        !lh_os_net_socket_listen(lh_addr_of(listen_fd), 8))
    {
        lh_os_net_socket_close(lh_addr_of(listen_fd));
        return lh_bool_false;
    }
    (void)lh_os_net_socket_set_blocking(lh_addr_of(listen_fd), lh_bool_false);
    lh_logger_notice(lh_runtime_logger(), "[lh] ui web host http://127.0.0.1:%u/", lh_cast_static(lh_uint_t, port));
    {
        unsigned gen = 1u;
        unsigned i;
        host_ws_cli_t cli[HOST_WS_MAX];
        host_ws_stream_t stream;

        lh_memory_set(cli, sizeof(cli), 0);
        lh_memory_set(lh_addr_of(stream), sizeof(stream), 0);
        for (i = 0u; i < HOST_WS_MAX; ++i)
        {
            lh_os_net_socket_init(lh_addr_of(cli[i].sock));
        }
        for (;;)
        {
            lh_ui_rect_t damage;
            const unsigned now = lh_cast_static(unsigned, lh_os_tick_ms());
            unsigned spent;

            if (hooks != lh_null && hooks->tick != lh_null)
            {
                hooks->tick(hooks->ctx);
            }
            host_accept_all(lh_addr_of(listen_fd), cli, view, width, height, json, json_cap, hooks,
                            lh_addr_of(gen), now);
            for (i = 0u; i < HOST_WS_MAX; ++i)
            {
                host_ws_pump(lh_addr_of(cli[i]), view, hooks, lh_addr_of(gen), now);
            }
            /* After the input, not before it: an event and the damage it caused
               are one change and one frame, not two. */
            if (lh_ui_view_take_damage(view, lh_addr_of(damage)))
            {
                ++gen;
            }
            host_ws_frame(cli, lh_addr_of(stream), view, width, height, json, json_cap, gen);
            spent = lh_cast_static(unsigned, lh_os_tick_ms())- now;
            host_wait(lh_addr_of(listen_fd), cli, (spent >= HOST_TICK_MS) ? 0u : HOST_TICK_MS - spent);
        }
    }
}
