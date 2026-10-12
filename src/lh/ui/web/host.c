/**
 * @file host.c
 * @brief Implementation of `lh/ui/web/host.h`.
 */

#include <lh/ui/web/host.h>
#include <lh/ui/web/ws.h>

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
#    include <time.h>
#    include <unistd.h>
#endif

#define HOST_WS_MAX 8u
#define HOST_WS_RX (LH_UI_WEB_WS_MAX_MSG + 32u)
#define HOST_WS_QMAX (4u * 1024u * 1024u)
#define HOST_WS_PING_MS 15000u
#define HOST_WS_DEAD_MS 45000u

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
} host_ws_cli_t;

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
    "function glyph(op){var mid=typeof op.m==='number'?op.m:-1,a=null;"
    "if(op.cover){a=decodeCover(op);if(mid>=0)atlas[mid]=a;}"
    "else if(mid>=0)a=atlas[mid];if(!a)return null;"
    "var k=mid+','+(op.color||[]).join(',');if(glyphs[k])return glyphs[k];"
    "var t=document.createElement('canvas');t.width=a.width;t.height=a.height;"
    "var g=t.getContext('2d');g.fillStyle=rgba(op.color);g.fillRect(0,0,t.width,t.height);"
    "g.globalCompositeOperation='destination-in';g.drawImage(a,0,0);"
    "if(mid>=0)glyphs[k]=t;return t;}\n"
    "function round(x,r,rad){x.beginPath();if(x.roundRect)x.roundRect(r[0],r[1],r[2],r[3],rad||0);else x.rect(r[0],r[1],r[2],r[3]);}\n"
    "function pane3d(d){var v=document.getElementById('v3d'),r=d.viewport3d;"
    "if(!r){v.style.display='none';return;}"
    "v.style.display='block';v.style.left=r[0]+'px';v.style.top=r[1]+'px';"
    "v.width=r[2];v.height=r[3];v.style.width=r[2]+'px';v.style.height=r[3]+'px';"
    "if(!v.gl)v.gl=v.getContext('webgl')||v.getContext('experimental-webgl');}\n"
    "function play(d){"
    "glyphs={};"
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
    "sock.onmessage=function(ev){play(JSON.parse(ev.data));};"
    "sock.onclose=function(){sock=null;startPoll();armRetry();};"
    "sock.onerror=function(){};"
    "}\n"
    "load();\n"
    "openWs();\n"
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

static unsigned
host_now_ms(void)
{
#if defined(_WIN32)
    return (unsigned)GetTickCount();
#else
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned)((unsigned long)ts.tv_sec * 1000ul + (unsigned long)ts.tv_nsec / 1000000ul);
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

static void
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

        free(q->bytes);
        free(q);
        q = n;
    }
    free(c->rx);
    free(c->msg);
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
    if (need == 0u || c->queued + (unsigned)need > HOST_WS_QMAX)
    {
        host_ws_drop(c);
        return lh_bool_false;
    }
    q = (host_ws_q_t *)malloc(sizeof(*q));
    bytes = (unsigned char *)malloc(need);
    if (q == lh_null || bytes == lh_null)
    {
        free(q);
        free(bytes);
        host_ws_drop(c);
        return lh_bool_false;
    }
    if (lh_ui_web_ws_write(bytes, need, opcode, lh_bool_true, payload, plen) != need)
    {
        free(q);
        free(bytes);
        host_ws_drop(c);
        return lh_bool_false;
    }
    q->bytes = bytes;
    q->len = (unsigned)need;
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
    c->queued += (unsigned)need;
    return lh_bool_true;
}

static void
host_ws_flush(host_ws_cli_t *c)
{
    while (c != lh_null && c->used && c->qhead != lh_null)
    {
        host_ws_q_t *q = c->qhead;
        const unsigned left = q->len - q->off;
        const unsigned chunk = (left > 16384u) ? 16384u : left;
        const lh_ssize_t w = lh_os_net_socket_send(lh_addr_of(c->sock), (lh_ptr)(q->bytes + q->off), chunk);

        if (w == LH_OS_RESULT_PENDING)
        {
            return;
        }
        if (w <= 0)
        {
            host_ws_drop(c);
            return;
        }
        q->off += (unsigned)w;
        c->queued -= (unsigned)w;
        if (q->off >= q->len)
        {
            c->qhead = q->next;
            if (c->qhead == lh_null)
            {
                c->qtail = lh_null;
            }
            free(q->bytes);
            free(q);
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

    if (c->msg_len + (unsigned)n > LH_UI_WEB_WS_MAX_MSG)
    {
        return lh_bool_false;
    }
    if (c->msg_len + (unsigned)n > c->msg_cap)
    {
        const unsigned cap = c->msg_len + (unsigned)n;
        const unsigned next = (c->msg_cap == 0u) ? 256u : (c->msg_cap * 2u);
        const unsigned want = (cap > next) ? cap : next;

        grow = (unsigned char *)realloc(c->msg, want);
        if (grow == lh_null)
        {
            return lh_bool_false;
        }
        c->msg = grow;
        c->msg_cap = want;
    }
    memcpy(c->msg + c->msg_len, p, (size_t)n);
    c->msg_len += (unsigned)n;
    return lh_bool_true;
}

static void
host_ws_text(host_ws_cli_t *c, lh_ui_view_t *view, const lh_ui_web_host_hooks_t *hooks, unsigned *gen,
             const unsigned char *p, unsigned n)
{
    char line[256];

    if (n >= sizeof(line))
    {
        return;
    }
    memcpy(line, p, (size_t)n);
    line[n] = 0;
    host_event_line(view, hooks, gen, line);
    (void)c;
}

static void
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
        if (n == (lh_usize_t)-1)
        {
            host_ws_drop(c);
            return;
        }
        body = (unsigned char *)payload;
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
                    host_ws_text(c, view, hooks, gen, c->msg, c->msg_len);
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
                    host_ws_text(c, view, hooks, gen, body, (unsigned)plen);
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
        memmove(c->rx, c->rx + n, (size_t)(c->rx_len - (unsigned)n));
        c->rx_len -= (unsigned)n;
    }
}

static void
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
        c->rx = (unsigned char *)malloc(HOST_WS_RX);
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
        return;
    }
    lh_os_net_socket_close(client);
}

static void
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
    c->rx_len += (unsigned)got;
    host_ws_ingest(c, view, hooks, gen, now);
    if (c->used)
    {
        host_ws_flush(c);
    }
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
        unsigned i;
        host_ws_cli_t cli[HOST_WS_MAX];

        memset(cli, 0, sizeof(cli));
        for (i = 0u; i < HOST_WS_MAX; ++i)
        {
            lh_os_net_socket_init(lh_addr_of(cli[i].sock));
        }
        for (;;)
        {
            lh_os_net_socket_t client;
            lh_net_ip4_socket_addr_t peer;
            lh_ui_rect_t damage;
            lh_bool_t accepted;
            lh_bool_t need_draw = lh_bool_false;
            const unsigned now = host_now_ms();

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
                    host_ws_attach(cli, lh_addr_of(client), now);
                }
                else
                {
                    lh_os_net_socket_close(lh_addr_of(client));
                }
            }
            for (i = 0u; i < HOST_WS_MAX; ++i)
            {
                host_ws_pump(lh_addr_of(cli[i]), view, hooks, lh_addr_of(gen), now);
                if (cli[i].used && !cli[i].closing && cli[i].seen != gen && cli[i].qhead == lh_null)
                {
                    need_draw = lh_bool_true;
                }
            }
            if (need_draw)
            {
                lh_ui_canvas_web_t web;

                if (host_draw(view, width, height, json, json_cap, lh_addr_of(web)))
                {
                    const char *body = lh_ui_canvas_web_bytes(lh_addr_of(web));
                    const lh_usize_t len = lh_ui_canvas_web_length(lh_addr_of(web));

                    for (i = 0u; i < HOST_WS_MAX; ++i)
                    {
                        if (cli[i].used && !cli[i].closing && cli[i].seen != gen &&
                            cli[i].qhead == lh_null)
                        {
                            if (host_ws_enqueue(lh_addr_of(cli[i]), LH_UI_WEB_WS_TEXT, body, len,
                                                lh_bool_false))
                            {
                                cli[i].seen = gen;
                                host_ws_flush(lh_addr_of(cli[i]));
                            }
                        }
                    }
                }
            }
            if (!accepted)
            {
                host_idle_ms(20u);
            }
        }
    }
}
