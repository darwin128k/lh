/**
 * @file page.c
 * @brief Canvas2D player: built with ::lh_ui_web_js, not a host string literal.
 *
 * Functions are opened, filled with stmt/if/ret, and closed — bodies are not
 * one opaque string per function (except dense loops still emitted with ::lh_ui_web_js_raw).
 */

#include <lh/ui/web/page.h>

#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/ui/web/js.h>
#include <lh/util/addr.h>

#define page_lit(j, s) lh_ui_web_js_lit((j), (s), lh_cast_static(lh_usize_t, (sizeof(s) - 1u)))

lh_bool_t
lh_ui_web_page_build(lh_byte_ptr mem, lh_usize_t cap, lh_usize_t *out_len)
{
    lh_ui_web_js_t *j;
    lh_ui_web_js_t js;

    lh_ui_web_js_init(lh_addr_of(js), mem, cap);
    j = lh_addr_of(js);
    if (out_len != lh_null)
    {
        *out_len = 0u;
    }

    /* ── document shell ───────────────────────────────────────────────────── */
    page_lit(j, "<!doctype html><html lang=\"ru\"><head><meta charset=\"utf-8\">"
                "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
                "<title>lh_ui</title>"
                "<style>html,body{margin:0;height:100%;background:#edf0f5;overflow:hidden}"
                "#c{display:block;position:absolute;cursor:pointer;outline:none}"
                "#v3d{position:absolute;display:none;pointer-events:none}</style></head><body>"
                "<canvas id=\"c\" width=\"980\" height=\"620\" tabindex=\"0\"></canvas>"
                "<canvas id=\"v3d\"></canvas>"
                "<script>\n");

    /* ── rgba ─────────────────────────────────────────────────────────────── */
    lh_ui_web_js_fun_open(j, "rgba", "c");
    lh_ui_web_js_ret(j, "'rgba('+c[0]+','+c[1]+','+c[2]+','+(c[3]/255)+')'");
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_vars(j, "atlas=[],tints={},ntint=0,fw=980,fh=620,cssS=1,lastD=null,pend=null,raf=0,v3on=0");
    lh_ui_web_js_vars(j, "scaleMode=(/scale=layout/.test(location.search)?'layout':'fit'),lastZW=0,lastZH=0");

    /* ── hexv ─────────────────────────────────────────────────────────────── */
    lh_ui_web_js_fun_open(j, "hexv", "c");
    lh_ui_web_js_ret(j, "c<58?c-48:(c|32)-87");
    lh_ui_web_js_fun_close(j);

    /* ── decodeCover ──────────────────────────────────────────────────────── */
    lh_ui_web_js_fun_open(j, "decodeCover", "w,h,cv");
    lh_ui_web_js_var(j, "im", "document.createElement('canvas')");
    lh_ui_web_js_assign(j, "im.width", "w");
    lh_ui_web_js_assign(j, "im.height", "h");
    lh_ui_web_js_if(j, "w<=0||h<=0");
    lh_ui_web_js_ret(j, "im");
    lh_ui_web_js_end(j);
    lh_ui_web_js_vars(j, "g=im.getContext('2d'),id=g.createImageData(w,h),p=id.data,n=w*h,i,j,k");
    /* Pixel loop stays dense — expanding it gains nothing readable. */
    lh_ui_web_js_raw(j, "for(i=0,j=0;i<n;i++,j+=4){k=i*2;p[j]=p[j+1]=p[j+2]=255;"
                        "p[j+3]=k+1<cv.length?(hexv(cv.charCodeAt(k))<<4)|hexv(cv.charCodeAt(k+1)):0;}\n");
    lh_ui_web_js_stmt(j, "g.putImageData(id,0,0)");
    lh_ui_web_js_ret(j, "im");
    lh_ui_web_js_fun_close(j);

    /* ── ingest ───────────────────────────────────────────────────────────── */
    lh_ui_web_js_fun_open(j, "ingest", "d");
    lh_ui_web_js_vars(j, "ops=d.ops||[],i,op");
    lh_ui_web_js_for(j, "i=0;i<ops.length;i++");
    lh_ui_web_js_assign(j, "op", "ops[i]");
    lh_ui_web_js_if(j, "op.cover!==undefined&&typeof op.m==='number'");
    lh_ui_web_js_assign(j, "atlas[op.m]", "decodeCover(op.size[0],op.size[1],op.cover)");
    lh_ui_web_js_end(j);
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    /* ── tint ─────────────────────────────────────────────────────────────── */
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

    /* ── glyph ────────────────────────────────────────────────────────────── */
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

    /* ── round ────────────────────────────────────────────────────────────── */
    lh_ui_web_js_fun_open(j, "round", "x,r,rad");
    lh_ui_web_js_stmt(j, "x.beginPath()");
    lh_ui_web_js_if(j, "x.roundRect");
    lh_ui_web_js_stmt(j, "x.roundRect(r[0],r[1],r[2],r[3],rad||0)");
    lh_ui_web_js_else(j);
    lh_ui_web_js_stmt(j, "x.rect(r[0],r[1],r[2],r[3])");
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    /* ── pane3d ───────────────────────────────────────────────────────────── */
    lh_ui_web_js_fun_open(j, "pane3d", "d");
    lh_ui_web_js_vars(j, "v=document.getElementById('v3d'),r=d.viewport3d,c=document.getElementById('c')");
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

    /* ── fit ──────────────────────────────────────────────────────────────── */
    lh_ui_web_js_fun_open(j, "fit", "");
    lh_ui_web_js_vars(j, "c=document.getElementById('c'),vw=window.innerWidth,vh=window.innerHeight,"
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

    /* ── play (switch stays one raw block) ────────────────────────────────── */
    lh_ui_web_js_fun_open(j, "play", "d");
    lh_ui_web_js_assign(j, "lastD", "d");
    lh_ui_web_js_assign(j, "fw", "d.width");
    lh_ui_web_js_assign(j, "fh", "d.height");
    lh_ui_web_js_vars(j, "x=fit(),ops=d.ops||[],i,op,s,fs=null,dpr=window.devicePixelRatio||1");
    lh_ui_web_js_stmt(j, "x.setTransform(cssS*dpr,0,0,cssS*dpr,0,0)");
    lh_ui_web_js_stmt(j, "pane3d(d)");
    lh_ui_web_js_raw(j,
                     "for(i=0;i<ops.length;i++){op=ops[i];switch(op.op){"
                     "case 'fill_mask':{var g=glyph(op);if(g)x.drawImage(g,op.origin[0],op.origin[1]);break;}"
                     "case 'fill_rect':s=rgba(op.color);if(s!==fs){x.fillStyle=s;fs=s;}"
                     "x.fillRect(op.rect[0],op.rect[1],op.rect[2],op.rect[3]);break;"
                     "case 'fill_round_rect':s=rgba(op.color);if(s!==fs){x.fillStyle=s;fs=s;}"
                     "round(x,op.rect,op.radius||0);x.fill();break;"
                     "case 'clear':s=rgba(op.color);if(s!==fs){x.fillStyle=s;fs=s;}x.fillRect(0,0,fw,fh);break;"
                     "case 'shadow':x.save();x.shadowColor=rgba(op.color);x.shadowBlur=op.spread||0;"
                     "x.shadowOffsetX=op.offset?op.offset[0]:0;x.shadowOffsetY=op.offset?op.offset[1]:0;"
                     "x.fillStyle=rgba(op.color);round(x,op.rect,op.radius||0);x.fill();x.restore();fs=null;break;"
                     "}}\n");
    lh_ui_web_js_fun_close(j);

    /* ── sized / show / pos / viewSize / reportSize ───────────────────────── */
    lh_ui_web_js_fun_open(j, "sized", "d");
    lh_ui_web_js_if(j, "scaleMode!=='layout'||!lastZW");
    lh_ui_web_js_ret(j, "1");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "d.width===980&&d.height===620&&(lastZW!==980||lastZH!==620)");
    lh_ui_web_js_ret(j, "0");
    lh_ui_web_js_end(j);
    lh_ui_web_js_ret(j, "1");
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "show", "d");
    lh_ui_web_js_stmt(j, "ingest(d)");
    lh_ui_web_js_if(j, "!sized(d)");
    lh_ui_web_js_ret(j, lh_null);
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "pend", "d");
    lh_ui_web_js_if(j, "!raf");
    lh_ui_web_js_assign(j, "raf",
                        "requestAnimationFrame(function(){raf=0;var p=pend;pend=null;if(p)play(p);})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "pos", "e");
    lh_ui_web_js_vars(j, "c=document.getElementById('c'),r=c.getBoundingClientRect()");
    lh_ui_web_js_ret(j, "{x:Math.round((e.clientX-r.left)*fw/r.width),"
                        "y:Math.round((e.clientY-r.top)*fh/r.height)}");
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "viewSize", "");
    lh_ui_web_js_vars(j, "w=window.innerWidth|0,h=window.innerHeight|0");
    lh_ui_web_js_if(j, "w<1");
    lh_ui_web_js_assign(j, "w", "1");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "h<1");
    lh_ui_web_js_assign(j, "h", "1");
    lh_ui_web_js_end(j);
    lh_ui_web_js_ret(j, "{w:w,h:h}");
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_fun_open(j, "reportSize", "force");
    lh_ui_web_js_if(j, "scaleMode!=='layout'");
    lh_ui_web_js_ret(j, lh_null);
    lh_ui_web_js_end(j);
    lh_ui_web_js_var(j, "s", "viewSize()");
    lh_ui_web_js_if(j, "!force&&s.w===lastZW&&s.h===lastZH");
    lh_ui_web_js_ret(j, lh_null);
    lh_ui_web_js_end(j);
    lh_ui_web_js_assign(j, "lastZW", "s.w");
    lh_ui_web_js_assign(j, "lastZH", "s.h");
    lh_ui_web_js_stmt(j, "emit('z '+s.w+' '+s.h)");
    lh_ui_web_js_fun_close(j);

    lh_ui_web_js_assign(j, "window.onresize",
                        "function(){if(scaleMode==='layout')reportSize();else if(lastD)play(lastD);}");
    lh_ui_web_js_vars(j, "busy=0,rev=-1");

    /* ── load / poll / emit ───────────────────────────────────────────────── */
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
    lh_ui_web_js_ret(j, "fetch('/ui/press?x='+p[1]+'&y='+p[2]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='r'");
    lh_ui_web_js_ret(j, "fetch('/ui/release?x='+p[1]+'&y='+p[2]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='w'");
    lh_ui_web_js_ret(j, "fetch('/ui/wheel?x='+p[1]+'&y='+p[2]+'&dx='+p[3]+'&dy='+p[4])"
                        ".then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='k'");
    lh_ui_web_js_ret(j, "fetch('/ui/key?k='+p[1]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='t'");
    lh_ui_web_js_ret(j, "fetch('/ui/text?c='+p[1]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_if(j, "p[0]==='z'");
    lh_ui_web_js_ret(j, "fetch('/ui/size?w='+p[1]+'&h='+p[2]).then(function(){rev=-1;return load();})");
    lh_ui_web_js_end(j);
    lh_ui_web_js_fun_close(j);

    /* ── canvas handlers + websocket ──────────────────────────────────────── */
    lh_ui_web_js_var(j, "c", "document.getElementById('c')");
    lh_ui_web_js_prop(j, "c", "onmousedown", "function(e){var p=pos(e);emit('p '+p.x+' '+p.y);}");
    lh_ui_web_js_prop(j, "c", "onmouseup", "function(e){var p=pos(e);emit('r '+p.x+' '+p.y);}");
    lh_ui_web_js_prop(j, "c", "onwheel",
                      "function(e){e.preventDefault();var p=pos(e);"
                      "var dy=e.deltaY===0?0:(e.deltaY>0?-1:1);var dx=e.deltaX===0?0:(e.deltaX>0?-1:1);"
                      "if(e.shiftKey&&dy){dx=dy;dy=0;}emit('w '+p.x+' '+p.y+' '+dx+' '+dy);}");

    lh_ui_web_js_var(j, "keys",
                     "{Tab:1,Enter:2,Escape:3,Backspace:4,Delete:5,ArrowLeft:7,ArrowRight:8,"
                     "ArrowUp:9,ArrowDown:10,Home:11,End:12,PageUp:13,PageDown:14}");
    lh_ui_web_js_assign(j, "document.onkeydown",
                        "function(e){var k=keys[e.key];"
                        "if(k){e.preventDefault();emit('k '+k);return;}"
                        "if(e.key&&e.key.length===1&&!e.ctrlKey&&!e.altKey&&!e.metaKey){"
                        "e.preventDefault();emit('t '+e.key.codePointAt(0));}}");
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
    lh_ui_web_js_raw(j, "try{sock=new WebSocket((location.protocol==='https:'?'wss://':'ws://')"
                        "+location.host+'/ws');}\n"
                        "catch(e){sock=null;startPoll();armRetry();return;}\n");
    lh_ui_web_js_prop(j, "sock", "onopen", "function(){retry=200;stopPoll();reportSize(1);}");
    lh_ui_web_js_prop(j, "sock", "onmessage", "function(ev){show(JSON.parse(ev.data));}");
    lh_ui_web_js_prop(j, "sock", "onclose", "function(){sock=null;startPoll();armRetry();}");
    lh_ui_web_js_prop(j, "sock", "onerror", "function(){}");
    lh_ui_web_js_fun_close(j);

    /* Layout: HTTP size before WS so the first frame is not 980×620. */
    page_lit(j, "if(scaleMode==='layout'){(function(){var s=viewSize();lastZW=s.w;lastZH=s.h;"
                "fetch('/ui/size?w='+s.w+'&h='+s.h).then(function(){rev=-1;return load();},function(){})."
                "then(function(){openWs();},function(){openWs();});})();}\n"
                "else{load();openWs();}\n"
                "</script></body></html>");

    lh_ui_web_js_finish(j);
    if (!lh_ui_web_js_ok(j))
    {
        return lh_bool_false;
    }
    if (out_len != lh_null)
    {
        *out_len = lh_ui_web_js_len(j);
    }
    return lh_bool_true;
}
