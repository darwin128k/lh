/**
 * @file host.c
 * @brief Implementation of `lh/ui/web/host.h`.
 */

#include <lh/ui/web/host.h>

#include <lh/crypto/sha.h>
#include <lh/net/ip.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/null.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/result.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#    include <windows.h>
#else
#    include <unistd.h>
#endif

static const char lh_ui_web_page[] =
    "<!doctype html><html lang=\"ru\"><head><meta charset=\"utf-8\">"
    "<title>lh_ui</title>"
    "<style>html,body{margin:0;background:#edf0f5;position:relative}"
    "canvas{display:block;cursor:pointer;outline:none}"
    "#v3d{position:absolute;display:none;pointer-events:none}</style></head><body>"
    "<canvas id=\"c\" width=\"980\" height=\"620\" tabindex=\"0\"></canvas>"
    "<canvas id=\"v3d\"></canvas>"
    "<script>\n"
    "function rgba(c){return 'rgba('+c[0]+','+c[1]+','+c[2]+','+(c[3]/255)+')';}\n"
    "var atlas=[],glyphs={};\n"
    "function decodeCover(op){var w=op.size[0],h=op.size[1],cv=op.cover||'',im=document.createElement('canvas');"
    "im.width=w;im.height=h;var g=im.getContext('2d'),id=g.createImageData(w,h);"
    "for(var i=0;i<w*h;i++){var a=parseInt(cv.substr(i*2,2),16);if(isNaN(a))a=0;"
    "id.data[i*4]=255;id.data[i*4+1]=255;id.data[i*4+2]=255;id.data[i*4+3]=a;}"
    "g.putImageData(id,0,0);return im;}\n"
    "function glyph(op){var mid=typeof op.m==='number'?op.m:-1;"
    "if(op.cover){var a=decodeCover(op);if(mid>=0)atlas[mid]=a;}"
    "var a=mid>=0?atlas[mid]:null;if(!a)return null;"
    "var k=mid+','+(op.color||[]).join(',');if(glyphs[k])return glyphs[k];"
    "var t=document.createElement('canvas');t.width=a.width;t.height=a.height;"
    "var g=t.getContext('2d');g.fillStyle=rgba(op.color);g.fillRect(0,0,t.width,t.height);"
    "g.globalCompositeOperation='destination-in';g.drawImage(a,0,0);glyphs[k]=t;return t;}\n"
    "function round(x,r,rad){x.beginPath();if(x.roundRect)x.roundRect(r[0],r[1],r[2],r[3],rad||0);else x.rect(r[0],r[1],r[2],r[3]);}\n"
    "function pane3d(d){var v=document.getElementById('v3d'),r=d.viewport3d;"
    "if(!r){v.style.display='none';return;}"
    "v.style.display='block';v.style.left=r[0]+'px';v.style.top=r[1]+'px';"
    "v.width=r[2];v.height=r[3];v.style.width=r[2]+'px';v.style.height=r[3]+'px';"
    "if(!v.gl)v.gl=v.getContext('webgl')||v.getContext('experimental-webgl');}\n"
    "function play(d){"
    "var c=document.getElementById('c'),x=c.getContext('2d'),ops=d.ops||[],i;"
    "if(c.width!==d.width)c.width=d.width;if(c.height!==d.height)c.height=d.height;"
    "pane3d(d);"
    "for(i=0;i<ops.length;i++){var op=ops[i];"
    "if(op.op==='clear'){x.fillStyle=rgba(op.color);x.fillRect(0,0,c.width,c.height);}"
    "else if(op.op==='fill_rect'){x.fillStyle=rgba(op.color);x.fillRect(op.rect[0],op.rect[1],op.rect[2],op.rect[3]);}"
    "else if(op.op==='fill_round_rect'){x.fillStyle=rgba(op.color);round(x,op.rect,op.radius||0);x.fill();}"
    "else if(op.op==='shadow'){x.save();x.shadowColor=rgba(op.color);x.shadowBlur=op.spread||0;"
    "x.shadowOffsetX=op.offset?op.offset[0]:0;x.shadowOffsetY=op.offset?op.offset[1]:0;"
    "x.fillStyle=rgba(op.color);round(x,op.rect,op.radius||0);x.fill();x.restore();}"
    "else if(op.op==='fill_mask'){var g=glyph(op);if(g)x.drawImage(g,op.origin[0],op.origin[1]);}"
    "}"
    "}\n"
    "function pos(e){var c=document.getElementById('c'),r=c.getBoundingClientRect();"
    "return {x:Math.round((e.clientX-r.left)*c.width/r.width),y:Math.round((e.clientY-r.top)*c.height/r.height)};"
    "}\n"
    "var sock=null,busy=0,rev=-1;\n"
    "function load(){if(busy)return busy;busy=fetch('/ui.json').then(function(r){return r.json();}).then(play).then(function(){busy=0;},function(){busy=0;});return busy;}\n"
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
    "try{sock=new WebSocket((location.protocol==='https:'?'wss://':'ws://')+location.host+'/ws');"
    "sock.onmessage=function(ev){play(JSON.parse(ev.data));};"
    "sock.onclose=function(){sock=null;load();setInterval(poll,80);};"
    "}catch(e){sock=null;load();setInterval(poll,80);}\n"
    "</script></body></html>";

static void
host_idle_ms(unsigned ms)
{
#if defined(_WIN32)
    Sleep(ms);
#else
    usleep((unsigned)(ms * 1000u));
#endif
}

static void
host_send_all(lh_os_net_socket_t *client, const char *p, lh_usize_t left)
{
    while (left > 0u)
    {
        const lh_usize_t chunk = (left > 16384u) ? 16384u : left;
        const lh_ssize_t w = lh_os_net_socket_send(client, (lh_ptr)p, chunk);

        if (w < 0 || w == 0)
        {
            break;
        }
        p += (lh_usize_t)w;
        left -= (lh_usize_t)w;
    }
}

static void
host_reply(lh_os_net_socket_t *client, int code, const char *type, const char *body, lh_usize_t len)
{
    char head[256];
    const char *reason =
        (code == 200) ? "OK" : ((code == 304) ? "Not Modified" : ((code == 500) ? "Error" : "Not Found"));
    int n = snprintf(head, sizeof(head),
                     "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %u\r\n"
                     "Connection: close\r\nCache-Control: no-store\r\n\r\n",
                     code, reason, type, (unsigned)len);

    if (n > 0)
    {
        host_send_all(client, head, (lh_usize_t)n);
    }
    host_send_all(client, body, len);
}

static int
host_query_int(const char *req, const char *key)
{
    const char *p = strstr(req, key);

    if (p == lh_null)
    {
        return 0;
    }
    return atoi(p + strlen(key));
}

static unsigned
host_query_uint(const char *req, const char *key)
{
    const char *p = strstr(req, key);
    long n;

    if (p == lh_null)
    {
        return 0u;
    }
    n = strtol(p + strlen(key), lh_null, 10);
    return n < 0 ? 0u : (unsigned)n;
}

static lh_ui_point_t
host_point(const char *req)
{
    lh_ui_point_t point;

    lh_ui_point_init(lh_addr_of(point), (lh_ui_scalar_t)host_query_int(req, "x="),
                     (lh_ui_scalar_t)host_query_int(req, "y="));
    return point;
}

static void
host_press(lh_ui_view_t *view, const char *req, const lh_ui_web_host_hooks_t *hooks)
{
    const int x = host_query_int(req, "x=");
    const int y = host_query_int(req, "y=");

    if (hooks != lh_null && hooks->press != lh_null)
    {
        hooks->press(x, y, hooks->ctx);
        return;
    }
    lh_ui_view_press(view, host_point(req));
}

static void
host_release(lh_ui_view_t *view, const char *req, const lh_ui_web_host_hooks_t *hooks)
{
    const int x = host_query_int(req, "x=");
    const int y = host_query_int(req, "y=");

    if (hooks != lh_null && hooks->release != lh_null)
    {
        hooks->release(x, y, hooks->ctx);
        return;
    }
    (void)lh_ui_view_release(view, host_point(req));
}

static void
host_wheel(lh_ui_view_t *view, const char *req, const lh_ui_web_host_hooks_t *hooks)
{
    const int x = host_query_int(req, "x=");
    const int y = host_query_int(req, "y=");
    const int dx = host_query_int(req, "dx=");
    const int dy = host_query_int(req, "dy=");

    if (hooks != lh_null && hooks->wheel != lh_null)
    {
        hooks->wheel(x, y, dx, dy, hooks->ctx);
        return;
    }
    (void)lh_ui_view_wheel(view, host_point(req), (lh_ui_scalar_t)dx, (lh_ui_scalar_t)dy);
}

static void
host_key(lh_ui_view_t *view, const char *req, const lh_ui_web_host_hooks_t *hooks)
{
    int k = host_query_int(req, "k=");

    if (k < (int)lh_key_other || k > (int)lh_key_alt)
    {
        k = (int)lh_key_other;
    }
    if (hooks != lh_null && hooks->key != lh_null)
    {
        hooks->key((lh_key_t)k, lh_bool_true, hooks->ctx);
        return;
    }
    (void)lh_ui_view_key(view, (lh_key_t)k, lh_bool_true);
}

static void
host_text(lh_ui_view_t *view, const char *req, const lh_ui_web_host_hooks_t *hooks)
{
    const lh_u32_t code = (lh_u32_t)host_query_uint(req, "c=");

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

static void
host_ok(lh_os_net_socket_t *client)
{
    const char *ok = "ok\n";

    host_reply(client, 200, "text/plain; charset=utf-8", ok, strlen(ok));
}

static void
host_bump(unsigned *gen);

static void
host_b64_sha1(const unsigned char *in, char *out)
{
    static const char tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned i;
    unsigned o = 0;

    for (i = 0; i < 18u; i += 3u)
    {
        const unsigned n = ((unsigned)in[i] << 16) | ((unsigned)in[i + 1u] << 8) | (unsigned)in[i + 2u];

        out[o++] = tab[(n >> 18) & 63u];
        out[o++] = tab[(n >> 12) & 63u];
        out[o++] = tab[(n >> 6) & 63u];
        out[o++] = tab[n & 63u];
    }
    {
        const unsigned n = ((unsigned)in[18] << 16) | ((unsigned)in[19] << 8);

        out[o++] = tab[(n >> 18) & 63u];
        out[o++] = tab[(n >> 12) & 63u];
        out[o++] = tab[(n >> 6) & 63u];
        out[o++] = '=';
        out[o] = 0;
    }
}

static lh_bool_t
host_ws_upgrade(lh_os_net_socket_t *client, const char *req)
{
    const char *k = strstr(req, "Sec-WebSocket-Key:");
    char key[40];
    char cat[80];
    unsigned char digest[LH_CRYPTO_SHA1_DIGEST_SIZE];
    char accept[32];
    char head[280];
    int n = 0;
    int h;

    if (k == lh_null)
    {
        k = strstr(req, "sec-websocket-key:");
    }
    if (k == lh_null)
    {
        return lh_bool_false;
    }
    k = strchr(k, ':');
    if (k == lh_null)
    {
        return lh_bool_false;
    }
    ++k;
    while (*k == ' ')
    {
        ++k;
    }
    while (k[n] != 0 && k[n] != '\r' && k[n] != '\n' && n < (int)sizeof(key) - 1)
    {
        key[n] = k[n];
        ++n;
    }
    key[n] = 0;
    snprintf(cat, sizeof(cat), "%s258EAFA5-E914-47DA-95CA-C5AB0DC85B11", key);
    if (lh_crypto_sha_hash(lh_crypto_sha_kind_1, cat, strlen(cat), digest, sizeof(digest)) !=
        LH_CRYPTO_SHA1_DIGEST_SIZE)
    {
        return lh_bool_false;
    }
    host_b64_sha1(digest, accept);
    h = snprintf(head, sizeof(head),
                 "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\n"
                 "Connection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n",
                 accept);
    if (h <= 0)
    {
        return lh_bool_false;
    }
    host_send_all(client, head, (lh_usize_t)h);
    (void)lh_os_net_socket_set_blocking(client, lh_bool_false);
    return lh_bool_true;
}

static void
host_ws_send_text(lh_os_net_socket_t *client, const char *body, lh_usize_t len)
{
    unsigned char head[10];
    unsigned hlen;

    head[0] = 0x81u;
    if (len < 126u)
    {
        head[1] = (unsigned char)len;
        hlen = 2u;
    }
    else if (len <= 65535u)
    {
        head[1] = 126u;
        head[2] = (unsigned char)(len >> 8);
        head[3] = (unsigned char)len;
        hlen = 4u;
    }
    else
    {
        unsigned i;

        head[1] = 127u;
        for (i = 0; i < 8u; ++i)
        {
            head[9u - i] = (unsigned char)(len >> (8u * i));
        }
        hlen = 10u;
    }
    host_send_all(client, (const char *)head, hlen);
    host_send_all(client, body, len);
}

static void
host_event_line(lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, unsigned *gen, const char *line)
{
    int x = 0;
    int y = 0;
    int dx = 0;
    int dy = 0;
    int k = 0;
    unsigned code = 0;
    char q[80];

    if (sscanf(line, "p %d %d", lh_addr_of(x), lh_addr_of(y)) == 2)
    {
        snprintf(q, sizeof(q), "x=%d&y=%d", x, y);
        host_press(view, q, hooks);
        host_bump(gen);
        return;
    }
    if (sscanf(line, "r %d %d", lh_addr_of(x), lh_addr_of(y)) == 2)
    {
        snprintf(q, sizeof(q), "x=%d&y=%d", x, y);
        host_release(view, q, hooks);
        host_bump(gen);
        return;
    }
    if (sscanf(line, "w %d %d %d %d", lh_addr_of(x), lh_addr_of(y), lh_addr_of(dx), lh_addr_of(dy)) == 4)
    {
        snprintf(q, sizeof(q), "x=%d&y=%d&dx=%d&dy=%d", x, y, dx, dy);
        host_wheel(view, q, hooks);
        host_bump(gen);
        return;
    }
    if (sscanf(line, "k %d", lh_addr_of(k)) == 1)
    {
        snprintf(q, sizeof(q), "k=%d", k);
        host_key(view, q, hooks);
        host_bump(gen);
        return;
    }
    if (sscanf(line, "t %u", lh_addr_of(code)) == 1)
    {
        snprintf(q, sizeof(q), "c=%u", code);
        host_text(view, q, hooks);
        host_bump(gen);
    }
}

static int
host_ws_take(char *buf, unsigned *len, char *out, unsigned out_cap)
{
    const unsigned char *p = (const unsigned char *)buf;
    unsigned opcode;
    unsigned masked;
    unsigned plen;
    unsigned off = 2u;
    unsigned i;
    unsigned char mask[4];

    if (*len < 2u)
    {
        return 0;
    }
    opcode = (unsigned)(p[0] & 0x0fu);
    masked = (unsigned)(p[1] & 0x80u);
    plen = (unsigned)(p[1] & 0x7fu);
    if (plen == 126u)
    {
        if (*len < 4u)
        {
            return 0;
        }
        plen = ((unsigned)p[2] << 8) | (unsigned)p[3];
        off = 4u;
    }
    else if (plen == 127u)
    {
        return -1;
    }
    if (masked != 0u)
    {
        if (*len < off + 4u + plen)
        {
            return 0;
        }
        memcpy(mask, p + off, 4u);
        off += 4u;
    }
    else if (*len < off + plen)
    {
        return 0;
    }
    if (opcode == 8u)
    {
        return -1;
    }
    if (plen >= out_cap)
    {
        return -1;
    }
    for (i = 0; i < plen; ++i)
    {
        unsigned char b = p[off + i];

        if (masked != 0u)
        {
            b = (unsigned char)(b ^ mask[i & 3u]);
        }
        out[i] = (char)b;
    }
    out[plen] = 0;
    memmove(buf, buf + off + plen, (size_t)(*len - off - plen));
    *len -= off + plen;
    if (opcode == 9u)
    {
        return 0;
    }
    return (int)plen;
}

static lh_bool_t
host_draw(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
          lh_ui_canvas_web_t *web)
{
    lh_ui_canvas_t canvas;

    if (!lh_ui_canvas_web_open_mem(web, json, json_cap, width, height))
    {
        return lh_bool_false;
    }
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_web), web);
    lh_ui_view_set_canvas(view, lh_addr_of(canvas));
    lh_ui_view_draw(view, lh_null);
    lh_ui_view_set_canvas(view, lh_null);
    return lh_ui_canvas_web_close(web);
}

static void
host_bump(unsigned *gen)
{
    if (gen != lh_null)
    {
        ++*gen;
    }
}

static lh_bool_t
host_handle(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
            const lh_ui_web_host_hooks_t *hooks, unsigned *gen, lh_os_net_socket_t *client)
{
    char req[1024];
    const lh_ssize_t n = lh_os_net_socket_recv(client, req, sizeof(req) - 1u);
    lh_ui_canvas_web_t web;

    if (n <= 0)
    {
        return lh_bool_false;
    }
    req[n] = 0;
    if (strncmp(req, "GET /ws", 7) == 0)
    {
        return host_ws_upgrade(client, req);
    }
    if (strncmp(req, "GET /ui/click", 13) == 0 || strncmp(req, "POST /ui/click", 14) == 0)
    {
        host_press(view, req, hooks);
        host_release(view, req, hooks);
        host_bump(gen);
        host_ok(client);
        return lh_bool_false;
    }
    if (strncmp(req, "GET /ui/press", 13) == 0 || strncmp(req, "POST /ui/press", 14) == 0)
    {
        host_press(view, req, hooks);
        host_bump(gen);
        host_ok(client);
        return lh_bool_false;
    }
    if (strncmp(req, "GET /ui/release", 15) == 0 || strncmp(req, "POST /ui/release", 16) == 0)
    {
        host_release(view, req, hooks);
        host_bump(gen);
        host_ok(client);
        return lh_bool_false;
    }
    if (strncmp(req, "GET /ui/wheel", 13) == 0 || strncmp(req, "POST /ui/wheel", 14) == 0)
    {
        host_wheel(view, req, hooks);
        host_bump(gen);
        host_ok(client);
        return lh_bool_false;
    }
    if (strncmp(req, "GET /ui/key", 11) == 0 || strncmp(req, "POST /ui/key", 12) == 0)
    {
        host_key(view, req, hooks);
        host_bump(gen);
        host_ok(client);
        return lh_bool_false;
    }
    if (strncmp(req, "GET /ui/text", 12) == 0 || strncmp(req, "POST /ui/text", 13) == 0)
    {
        host_text(view, req, hooks);
        host_bump(gen);
        host_ok(client);
        return lh_bool_false;
    }
    if (strncmp(req, "GET /ui.rev", 11) == 0)
    {
        char body[32];
        int rn = snprintf(body, sizeof(body), "%u\n", gen != lh_null ? *gen : 0u);

        host_reply(client, 200, "text/plain; charset=utf-8", body, rn > 0 ? (lh_usize_t)rn : 0u);
        return lh_bool_false;
    }
    if (strncmp(req, "GET /ui.json", 12) == 0)
    {
        const unsigned since = host_query_uint(req, "rev=");

        if (gen != lh_null && since == *gen)
        {
            host_reply(client, 304, "text/plain; charset=utf-8", "", 0u);
            return lh_bool_false;
        }
        if (!host_draw(view, width, height, json, json_cap, lh_addr_of(web)))
        {
            const char *msg = "draw failed\n";

            host_reply(client, 500, "text/plain; charset=utf-8", msg, strlen(msg));
            return lh_bool_false;
        }
        host_reply(client, 200, "application/json; charset=utf-8", lh_ui_canvas_web_bytes(lh_addr_of(web)),
                   lh_ui_canvas_web_length(lh_addr_of(web)));
        return lh_bool_false;
    }
    if (strncmp(req, "GET / ", 6) == 0 || strncmp(req, "GET /index.html", 15) == 0)
    {
        host_reply(client, 200, "text/html; charset=utf-8", lh_ui_web_page,
                   (lh_usize_t)(sizeof(lh_ui_web_page) - 1u));
        return lh_bool_false;
    }
    {
        const char *msg = "not found\n";

        host_reply(client, 404, "text/plain; charset=utf-8", msg, strlen(msg));
    }
    return lh_bool_false;
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
    fprintf(stderr, "[lh] ui web host http://127.0.0.1:%u/\n", (unsigned)port);
    {
        unsigned gen = 1u;
        unsigned sent = 0u;
        lh_os_net_socket_t ws;
        lh_bool_t have_ws = lh_bool_false;
        char ws_buf[2048];
        unsigned ws_fill = 0u;

        lh_os_net_socket_init(lh_addr_of(ws));
        for (;;)
        {
            lh_os_net_socket_t client;
            lh_net_ip4_socket_addr_t peer;
            lh_ui_rect_t damage;
            lh_bool_t accepted;

            if (hooks != lh_null && hooks->tick != lh_null)
            {
                hooks->tick(hooks->ctx);
            }
            if (lh_ui_view_take_damage(view, lh_addr_of(damage)))
            {
                ++gen;
            }
            lh_os_net_socket_init(lh_addr_of(client));
            accepted = lh_os_net_socket_accept(lh_addr_of(listen_fd), lh_addr_of(client), lh_addr_of(peer));
            if (accepted)
            {
                if (host_handle(view, width, height, json, json_cap, hooks, lh_addr_of(gen),
                                lh_addr_of(client)))
                {
                    if (have_ws)
                    {
                        lh_os_net_socket_close(lh_addr_of(ws));
                    }
                    ws = client;
                    have_ws = lh_bool_true;
                    sent = 0u;
                    ws_fill = 0u;
                }
                else
                {
                    lh_os_net_socket_close(lh_addr_of(client));
                }
            }
            if (have_ws)
            {
                const lh_ssize_t got =
                    lh_os_net_socket_recv(lh_addr_of(ws), ws_buf + ws_fill, sizeof(ws_buf) - 1u - ws_fill);

                if (got == LH_OS_RESULT_PENDING)
                {
                    /* idle */
                }
                else if (got <= 0)
                {
                    lh_os_net_socket_close(lh_addr_of(ws));
                    lh_os_net_socket_init(lh_addr_of(ws));
                    have_ws = lh_bool_false;
                }
                else
                {
                    ws_fill += (unsigned)got;
                    for (;;)
                    {
                        char line[256];
                        const int take = host_ws_take(ws_buf, lh_addr_of(ws_fill), line, (unsigned)sizeof(line));

                        if (take < 0)
                        {
                            lh_os_net_socket_close(lh_addr_of(ws));
                            lh_os_net_socket_init(lh_addr_of(ws));
                            have_ws = lh_bool_false;
                            break;
                        }
                        if (take == 0)
                        {
                            break;
                        }
                        host_event_line(view, hooks, lh_addr_of(gen), line);
                    }
                }
            }
            if (have_ws && gen != sent)
            {
                lh_ui_canvas_web_t web;

                if (host_draw(view, width, height, json, json_cap, lh_addr_of(web)))
                {
                    host_ws_send_text(lh_addr_of(ws), lh_ui_canvas_web_bytes(lh_addr_of(web)),
                                      lh_ui_canvas_web_length(lh_addr_of(web)));
                    sent = gen;
                }
            }
            if (!accepted)
            {
                host_idle_ms(20u);
            }
        }
    }
}
