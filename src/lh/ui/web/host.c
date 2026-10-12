/**
 * @file host.c
 * @brief Implementation of `lh/ui/web/host.h`.
 */

#include <lh/ui/web/host.h>

#include <lh/net/ip.h>
#include <lh/net/socket/addr/ip4.h>
#include <lh/null.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/result.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/point.h>
#include <lh/ui/scalar.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char lh_ui_web_page[] =
    "<!doctype html><html lang=\"ru\"><head><meta charset=\"utf-8\">"
    "<title>lh_ui</title>"
    "<style>html,body{margin:0;background:#edf0f5;color:#1b2433;font:14px/1.4 sans-serif}"
    "header{padding:10px 16px;background:#fafbfd;border-bottom:1px solid #e7e8ec}"
    "canvas{display:block;margin:16px auto;background:#edf0f5;box-shadow:0 8px 24px #1b24331a;cursor:pointer}"
    "#st{color:#54596a;padding:0 16px 16px}</style></head><body>"
    "<header>Кадр из <code>lh_ui</code>. Не Python и не DOM-макет.</header>"
    "<canvas id=\"c\" width=\"980\" height=\"620\"></canvas>"
    "<div id=\"st\"></div>"
    "<script>"
    "function rgba(c){return 'rgba('+c[0]+','+c[1]+','+c[2]+','+(c[3]/255)+')'}"
    "function play(d){"
    "var c=document.getElementById('c'),x=c.getContext('2d');"
    "c.width=d.width;c.height=d.height;x.clearRect(0,0,c.width,c.height);"
    "(d.ops||[]).forEach(function(op){"
    "if(op.op==='clear'){x.fillStyle=rgba(op.color);x.fillRect(0,0,c.width,c.height)}"
    "else if(op.op==='fill_rect'){x.fillStyle=rgba(op.color);x.fillRect(op.rect[0],op.rect[1],op.rect[2],op.rect[3])}"
    "else if(op.op==='fill_round_rect'){var r=op.rect;x.fillStyle=rgba(op.color);x.beginPath();"
    "if(x.roundRect)x.roundRect(r[0],r[1],r[2],r[3],op.radius||0);else x.rect(r[0],r[1],r[2],r[3]);x.fill()}"
    "else if(op.op==='fill_mask'){var im=x.createImageData(op.size[0],op.size[1]),cv=op.cover||'',cr=op.color;"
    "for(var i=0;i<op.size[0]*op.size[1];i++){var a=parseInt(cv.substr(i*2,2),16);if(isNaN(a))a=0;"
    "im.data[i*4]=cr[0];im.data[i*4+1]=cr[1];im.data[i*4+2]=cr[2];im.data[i*4+3]=Math.round(cr[3]*a/255)}"
    "var o=document.createElement('canvas');o.width=op.size[0];o.height=op.size[1];"
    "o.getContext('2d').putImageData(im,0,0);x.drawImage(o,op.origin[0],op.origin[1])}"
    "});"
    "document.getElementById('st').textContent=(d.ops?d.ops.length:0)+' ops, '+d.width+'x'+d.height+' from /ui.json';"
    "}\n"
    "function pos(e){var c=document.getElementById('c'),r=c.getBoundingClientRect();"
    "return {x:Math.round((e.clientX-r.left)*c.width/r.width),y:Math.round((e.clientY-r.top)*c.height/r.height)};"
    "}\n"
    "function load(){return fetch('/ui.json').then(function(r){return r.json()}).then(play);}\n"
    "document.getElementById('c').onclick=function(e){var p=pos(e);"
    "fetch('/ui/click?x='+p.x+'&y='+p.y).then(load);};\n"
    "load().catch(function(e){document.getElementById('st').textContent=String(e);});"
    "</script></body></html>";

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
    const char *reason = (code == 200) ? "OK" : ((code == 500) ? "Error" : "Not Found");
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

static void
host_click(lh_ui_view_t *view, const char *req)
{
    lh_ui_point_t point;
    const int x = host_query_int(req, "x=");
    const int y = host_query_int(req, "y=");

    lh_ui_point_init(lh_addr_of(point), (lh_ui_scalar_t)x, (lh_ui_scalar_t)y);
    lh_ui_view_press(view, point);
    (void)lh_ui_view_release(view, point);
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
host_handle(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
            lh_os_net_socket_t *client)
{
    char req[1024];
    const lh_ssize_t n = lh_os_net_socket_recv(client, req, sizeof(req) - 1u);
    lh_ui_canvas_web_t web;

    if (n <= 0)
    {
        return;
    }
    req[n] = 0;
    if (strncmp(req, "POST /ui/click", 14) == 0 || strncmp(req, "GET /ui/click", 13) == 0)
    {
        const char *ok = "ok\n";

        host_click(view, req);
        host_reply(client, 200, "text/plain; charset=utf-8", ok, strlen(ok));
        return;
    }
    if (strncmp(req, "GET /ui.json", 12) == 0)
    {
        if (!host_draw(view, width, height, json, json_cap, lh_addr_of(web)))
        {
            const char *msg = "draw failed\n";

            host_reply(client, 500, "text/plain; charset=utf-8", msg, strlen(msg));
            return;
        }
        host_reply(client, 200, "application/json; charset=utf-8", lh_ui_canvas_web_bytes(lh_addr_of(web)),
                   lh_ui_canvas_web_length(lh_addr_of(web)));
        return;
    }
    if (strncmp(req, "GET / ", 6) == 0 || strncmp(req, "GET /index.html", 15) == 0)
    {
        host_reply(client, 200, "text/html; charset=utf-8", lh_ui_web_page,
                   (lh_usize_t)(sizeof(lh_ui_web_page) - 1u));
        return;
    }
    {
        const char *msg = "not found\n";

        host_reply(client, 404, "text/plain; charset=utf-8", msg, strlen(msg));
    }
}

lh_bool_t
lh_ui_web_host_run(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
                   lh_net_port_t port)
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
    fprintf(stderr, "[lh] ui web host http://127.0.0.1:%u/\n", (unsigned)port);
    for (;;)
    {
        lh_os_net_socket_t client;
        lh_net_ip4_socket_addr_t peer;

        lh_os_net_socket_init(lh_addr_of(client));
        if (!lh_os_net_socket_accept(lh_addr_of(listen_fd), lh_addr_of(client), lh_addr_of(peer)))
        {
            continue;
        }
        host_handle(view, width, height, json, json_cap, lh_addr_of(client));
        lh_os_net_socket_close(lh_addr_of(client));
    }
}
