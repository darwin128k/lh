/**
 * @file page.c
 * @brief Canvas2D player: built with ::lh_ui_web_js, not a host string literal.
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
    lh_ui_web_js_t js;

    lh_ui_web_js_init(lh_addr_of(js), mem, cap);
    if (out_len != lh_null)
    {
        *out_len = 0u;
    }

    /* ── document shell ───────────────────────────────────────────────────── */
    page_lit(lh_addr_of(js), "<!doctype html><html lang=\"ru\"><head><meta charset=\"utf-8\">"
                             "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
                             "<title>lh_ui</title>"
                             "<style>html,body{margin:0;height:100%;background:#edf0f5;overflow:hidden}"
                             "#c{display:block;position:absolute;cursor:pointer;outline:none}"
                             "#v3d{position:absolute;display:none;pointer-events:none}</style></head><body>"
                             "<canvas id=\"c\" width=\"980\" height=\"620\" tabindex=\"0\"></canvas>"
                             "<canvas id=\"v3d\"></canvas>"
                             "<script>\n");

    /* ── script: helpers and state ────────────────────────────────────────── */
    lh_ui_web_js_fun(lh_addr_of(js), "rgba", "c",
                     "return 'rgba('+c[0]+','+c[1]+','+c[2]+','+(c[3]/255)+')'");

    lh_ui_web_js_vars(lh_addr_of(js),
                      "atlas=[],tints={},ntint=0,fw=980,fh=620,cssS=1,lastD=null,pend=null,raf=0,v3on=0");
    lh_ui_web_js_vars(lh_addr_of(js),
                      "scaleMode=(/scale=layout/.test(location.search)?'layout':'fit'),lastZW=0,lastZH=0");

    lh_ui_web_js_fun(lh_addr_of(js), "hexv", "c", "return c<58?c-48:(c|32)-87");

    lh_ui_web_js_fun(
        lh_addr_of(js), "decodeCover", "w,h,cv",
        "var im=document.createElement('canvas');im.width=w;im.height=h;"
        "if(w<=0||h<=0)return im;var g=im.getContext('2d'),id=g.createImageData(w,h),p=id.data,n=w*h,i,j,k;"
        "for(i=0,j=0;i<n;i++,j+=4){k=i*2;p[j]=p[j+1]=p[j+2]=255;"
        "p[j+3]=k+1<cv.length?(hexv(cv.charCodeAt(k))<<4)|hexv(cv.charCodeAt(k+1)):0;}"
        "g.putImageData(id,0,0);return im");

    lh_ui_web_js_fun(lh_addr_of(js), "ingest", "d",
                     "var ops=d.ops||[],i,op;for(i=0;i<ops.length;i++){op=ops[i];"
                     "if(op.cover!==undefined&&typeof op.m==='number')"
                     "atlas[op.m]=decodeCover(op.size[0],op.size[1],op.cover);}");

    lh_ui_web_js_fun(lh_addr_of(js), "tint", "a,c",
                     "var t=document.createElement('canvas');t.width=a.width;t.height=a.height;"
                     "if(!a.width||!a.height)return t;"
                     "var g=t.getContext('2d');g.fillStyle=rgba(c);g.fillRect(0,0,t.width,t.height);"
                     "g.globalCompositeOperation='destination-in';g.drawImage(a,0,0);return t");

    lh_ui_web_js_fun(
        lh_addr_of(js), "glyph", "op",
        "var c=op.color;"
        "if(typeof op.m!=='number'){return tint(decodeCover(op.size[0],op.size[1],op.cover||''),c);}"
        "var k=op.m+','+c[0]+','+c[1]+','+c[2]+','+c[3],t=tints[k];if(t)return t;"
        "var a=atlas[op.m];if(!a)return null;"
        "if(ntint>=8192){tints={};ntint=0;}t=tint(a,c);tints[k]=t;++ntint;return t");

    lh_ui_web_js_fun(lh_addr_of(js), "round", "x,r,rad",
                     "x.beginPath();if(x.roundRect)x.roundRect(r[0],r[1],r[2],r[3],rad||0);"
                     "else x.rect(r[0],r[1],r[2],r[3])");

    lh_ui_web_js_fun(
        lh_addr_of(js), "pane3d", "d",
        "var v=document.getElementById('v3d'),r=d.viewport3d,c=document.getElementById('c');"
        "if(!r){if(v3on){v.style.display='none';v3on=0;}return;}v3on=1;"
        "var box=c.getBoundingClientRect(),dpr=window.devicePixelRatio||1;"
        "v.style.display='block';v.style.left=(box.left+r[0]*cssS)+'px';"
        "v.style.top=(box.top+r[1]*cssS)+'px';"
        "v.style.width=(r[2]*cssS)+'px';v.style.height=(r[3]*cssS)+'px';"
        "v.width=Math.max(1,Math.round(r[2]*cssS*dpr));v.height=Math.max(1,Math.round(r[3]*cssS*dpr));"
        "if(!v.gl)v.gl=v.getContext('webgl')||v.getContext('experimental-webgl')");

    lh_ui_web_js_fun(
        lh_addr_of(js), "fit", "",
        "var c=document.getElementById('c'),vw=window.innerWidth,vh=window.innerHeight,"
        "dpr=window.devicePixelRatio||1,s,bw,bh;"
        "if(scaleMode==='layout'){cssS=1;bw=Math.max(1,Math.round(fw*dpr));bh=Math.max(1,Math.round(fh*dpr));"
        "if(c.width!==bw)c.width=bw;if(c.height!==bh)c.height=bh;"
        "c.style.width=fw+'px';c.style.height=fh+'px';c.style.left='0';c.style.top='0';"
        "return c.getContext('2d');}"
        "s=Math.min(vw/fw,vh/fh);if(!isFinite(s)||s<=0)s=1;cssS=s;"
        "bw=Math.max(1,Math.round(fw*s*dpr));bh=Math.max(1,Math.round(fh*s*dpr));"
        "if(c.width!==bw)c.width=bw;if(c.height!==bh)c.height=bh;"
        "c.style.width=Math.round(fw*s)+'px';c.style.height=Math.round(fh*s)+'px';"
        "c.style.left=Math.round((vw-fw*s)/2)+'px';c.style.top=Math.round((vh-fh*s)/2)+'px';"
        "return c.getContext('2d')");

    lh_ui_web_js_fun(
        lh_addr_of(js), "play", "d",
        "lastD=d;fw=d.width;fh=d.height;"
        "var x=fit(),ops=d.ops||[],i,op,s,fs=null,dpr=window.devicePixelRatio||1;"
        "x.setTransform(cssS*dpr,0,0,cssS*dpr,0,0);"
        "pane3d(d);"
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
        "}}");

    /* Reject only the design 980×620 when the viewport is not that size. */
    lh_ui_web_js_fun(lh_addr_of(js), "sized", "d",
                     "if(scaleMode!=='layout'||!lastZW)return 1;"
                     "if(d.width===980&&d.height===620&&(lastZW!==980||lastZH!==620))return 0;return 1");

    lh_ui_web_js_fun(lh_addr_of(js), "show", "d",
                     "ingest(d);if(!sized(d))return;pend=d;"
                     "if(!raf)raf=requestAnimationFrame(function(){raf=0;var p=pend;pend=null;if(p)play(p);})");

    lh_ui_web_js_fun(lh_addr_of(js), "pos", "e",
                     "var c=document.getElementById('c'),r=c.getBoundingClientRect();"
                     "return {x:Math.round((e.clientX-r.left)*fw/r.width),"
                     "y:Math.round((e.clientY-r.top)*fh/r.height)}");

    lh_ui_web_js_fun(lh_addr_of(js), "viewSize", "",
                     "var w=window.innerWidth|0,h=window.innerHeight|0;if(w<1)w=1;if(h<1)h=1;return {w:w,h:h}");

    lh_ui_web_js_fun(lh_addr_of(js), "reportSize", "force",
                     "if(scaleMode!=='layout')return;"
                     "var s=viewSize();if(!force&&s.w===lastZW&&s.h===lastZH)return;"
                     "lastZW=s.w;lastZH=s.h;emit('z '+s.w+' '+s.h)");

    lh_ui_web_js_assign(lh_addr_of(js), "window.onresize",
                        "function(){if(scaleMode==='layout')reportSize();else if(lastD)play(lastD);}");

    lh_ui_web_js_vars(lh_addr_of(js), "busy=0,rev=-1");

    lh_ui_web_js_fun(lh_addr_of(js), "load", "",
                     "if(busy)return busy;busy=fetch('/ui.json').then(function(r){return r.json();})"
                     ".then(show).then(function(){busy=0;},function(){busy=0;});return busy");

    lh_ui_web_js_fun(lh_addr_of(js), "poll", "",
                     "fetch('/ui.rev').then(function(r){return r.text();}).then(function(t){"
                     "var n=+t;if(n!==rev){rev=n;load();}})");

    lh_ui_web_js_fun(
        lh_addr_of(js), "emit", "line",
        "if(sock&&sock.readyState===1){sock.send(line);return;}"
        "var p=line.split(' ');"
        "if(p[0]==='p')return fetch('/ui/press?x='+p[1]+'&y='+p[2]).then(function(){rev=-1;return load();});"
        "if(p[0]==='r')return fetch('/ui/release?x='+p[1]+'&y='+p[2]).then(function(){rev=-1;return load();});"
        "if(p[0]==='w')return fetch('/ui/wheel?x='+p[1]+'&y='+p[2]+'&dx='+p[3]+'&dy='+p[4])"
        ".then(function(){rev=-1;return load();});"
        "if(p[0]==='k')return fetch('/ui/key?k='+p[1]).then(function(){rev=-1;return load();});"
        "if(p[0]==='t')return fetch('/ui/text?c='+p[1]).then(function(){rev=-1;return load();});"
        "if(p[0]==='z')return fetch('/ui/size?w='+p[1]+'&h='+p[2]).then(function(){rev=-1;return load();});");

    lh_ui_web_js_var(lh_addr_of(js), "c", "document.getElementById('c')");
    lh_ui_web_js_prop(lh_addr_of(js), "c", "onmousedown",
                      "function(e){var p=pos(e);emit('p '+p.x+' '+p.y);}");
    lh_ui_web_js_prop(lh_addr_of(js), "c", "onmouseup",
                      "function(e){var p=pos(e);emit('r '+p.x+' '+p.y);}");
    lh_ui_web_js_prop(
        lh_addr_of(js), "c", "onwheel",
        "function(e){e.preventDefault();var p=pos(e);"
        "var dy=e.deltaY===0?0:(e.deltaY>0?-1:1);var dx=e.deltaX===0?0:(e.deltaX>0?-1:1);"
        "if(e.shiftKey&&dy){dx=dy;dy=0;}emit('w '+p.x+' '+p.y+' '+dx+' '+dy);}");

    lh_ui_web_js_var(lh_addr_of(js), "keys",
                     "{Tab:1,Enter:2,Escape:3,Backspace:4,Delete:5,ArrowLeft:7,ArrowRight:8,"
                     "ArrowUp:9,ArrowDown:10,Home:11,End:12,PageUp:13,PageDown:14}");

    lh_ui_web_js_assign(
        lh_addr_of(js), "document.onkeydown",
        "function(e){var k=keys[e.key];"
        "if(k){e.preventDefault();emit('k '+k);return;}"
        "if(e.key&&e.key.length===1&&!e.ctrlKey&&!e.altKey&&!e.metaKey){"
        "e.preventDefault();emit('t '+e.key.codePointAt(0));}}");

    lh_ui_web_js_stmt(lh_addr_of(js), "c.focus()");
    lh_ui_web_js_vars(lh_addr_of(js), "sock=null,retry=200,pollId=0,retryId=0");

    lh_ui_web_js_fun(lh_addr_of(js), "stopPoll", "",
                     "if(pollId){clearInterval(pollId);pollId=0;}");
    lh_ui_web_js_fun(lh_addr_of(js), "startPoll", "",
                     "if(!pollId)pollId=setInterval(poll,80)");
    lh_ui_web_js_fun(lh_addr_of(js), "armRetry", "",
                     "if(retryId)return;retryId=setTimeout(function(){retryId=0;openWs();},retry);"
                     "retry=Math.min(retry*2,5000)");

    lh_ui_web_js_fun(
        lh_addr_of(js), "openWs", "",
        "if(sock&&(sock.readyState===0||sock.readyState===1))return;"
        "try{sock=new WebSocket((location.protocol==='https:'?'wss://':'ws://')+location.host+'/ws');}"
        "catch(e){sock=null;startPoll();armRetry();return;}"
        "sock.onopen=function(){retry=200;stopPoll();reportSize(1);};"
        "sock.onmessage=function(ev){show(JSON.parse(ev.data));};"
        "sock.onclose=function(){sock=null;startPoll();armRetry();};"
        "sock.onerror=function(){}");

    /* Layout: HTTP size before WS so the first frame is not 980×620. */
    page_lit(lh_addr_of(js),
             "if(scaleMode==='layout'){(function(){var s=viewSize();lastZW=s.w;lastZH=s.h;"
             "fetch('/ui/size?w='+s.w+'&h='+s.h).then(function(){rev=-1;return load();},function(){})."
             "then(function(){openWs();},function(){openWs();});})();}\n"
             "else{load();openWs();}\n"
             "</script></body></html>");

    lh_ui_web_js_finish(lh_addr_of(js));
    if (!lh_ui_web_js_ok(lh_addr_of(js)))
    {
        return lh_bool_false;
    }
    if (out_len != lh_null)
    {
        *out_len = lh_ui_web_js_len(lh_addr_of(js));
    }
    return lh_bool_true;
}
