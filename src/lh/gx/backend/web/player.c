/**
 * @file player.c
 * @brief Implementation of `lh/gx/backend/web/player.h`.
 */

#include <lh/gx/backend/web/player.h>

/* The script, line by line; the size of the array less its NUL is its length. */
static const lh_char_t m_gx_backend_web_player_script[] =
    "/* lh gx web player: replays the frames of lh/gx/backend/web.h on a 2D canvas. */\n"
    "function lhGxColor(k) {\n"
    "  return \"rgba(\" + ((k >>> 16) & 255) + \",\" + ((k >>> 8) & 255) + \",\" + (k & 255) + "
    "\",\" +\n"
    "    (k >>> 24) / 255 + \")\";\n"
    "}\n"
    "/* A brush is a colour, or [\"gradient\", box, shape, angle, frac, colour, ...] laid as\n"
    "   lh/core/pixel/gradient.h lays it: linear along (sin a, -cos a) through the middle,\n"
    "   as long as the box seen along it; radial on an ellipse of the box through its corners. "
    "*/\n"
    "function lhGxFill(ctx, k, x, y, w, h) {\n"
    "  if (typeof k === \"number\") {\n"
    "    ctx.fillStyle = lhGxColor(k);\n"
    "    ctx.fillRect(x, y, w, h);\n"
    "    return;\n"
    "  }\n"
    "  const b = k[1], cx = b[0] + b[2] / 2, cy = b[1] + b[3] / 2;\n"
    "  let g;\n"
    "  if (k[2] === 1) {\n"
    "    const sx = b[2] / 2 * Math.SQRT2, sy = b[3] / 2 * Math.SQRT2;\n"
    "    if (sx <= 0 || sy <= 0) return;\n"
    "    g = ctx.createRadialGradient(0, 0, 0, 0, 0, 1);\n"
    "    for (let i = 4; i + 1 < k.length; i += 2) g.addColorStop(k[i] / 255, lhGxColor(k[i + "
    "1]));\n"
    "    ctx.save();\n"
    "    ctx.translate(cx, cy);\n"
    "    ctx.scale(sx, sy);\n"
    "    ctx.fillStyle = g;\n"
    "    ctx.fillRect((x - cx) / sx, (y - cy) / sy, w / sx, h / sy);\n"
    "    ctx.restore();\n"
    "    return;\n"
    "  }\n"
    "  const a = k[3] * Math.PI / 180, dx = Math.sin(a), dy = -Math.cos(a);\n"
    "  const half = (Math.abs(b[2] * dx) + Math.abs(b[3] * dy)) / 2;\n"
    "  g = ctx.createLinearGradient(cx - dx * half, cy - dy * half, cx + dx * half, cy + dy * "
    "half);\n"
    "  for (let i = 4; i + 1 < k.length; i += 2) g.addColorStop(k[i] / 255, lhGxColor(k[i + "
    "1]));\n"
    "  ctx.fillStyle = g;\n"
    "  ctx.fillRect(x, y, w, h);\n"
    "}\n"
    "/* The brush filled over the mask's place, then kept only where the mask covers. */\n"
    "function lhGxFillMask(ctx, r, k, cover) {\n"
    "  const w = r[2], h = r[3];\n"
    "  if (w <= 0 || h <= 0) return;\n"
    "  const out = new OffscreenCanvas(w, h);\n"
    "  const oc = out.getContext(\"2d\");\n"
    "  oc.translate(-r[0], -r[1]);\n"
    "  lhGxFill(oc, k, r[0], r[1], w, h);\n"
    "  oc.setTransform(1, 0, 0, 1, 0, 0);\n"
    "  const mask = new OffscreenCanvas(w, h);\n"
    "  const mc = mask.getContext(\"2d\");\n"
    "  const img = mc.createImageData(w, h);\n"
    "  const d = img.data;\n"
    "  for (let i = 0; i < w * h; ++i) d[4 * i + 3] = parseInt(cover.substr(2 * i, 2), 16);\n"
    "  mc.putImageData(img, 0, 0);\n"
    "  oc.globalCompositeOperation = \"destination-in\";\n"
    "  oc.drawImage(mask, 0, 0);\n"
    "  ctx.drawImage(out, r[0], r[1]);\n"
    "}\n"
    "function lhGxPlay(ctx, frame) {\n"
    "  for (const c of frame) {\n"
    "    const clip = c[1], r = c[2], k = c[3];\n"
    "    ctx.save();\n"
    "    ctx.beginPath();\n"
    "    ctx.rect(clip[0], clip[1], clip[2], clip[3]);\n"
    "    ctx.clip();\n"
    "    if (c[0] === \"fill_rect\") {\n"
    "      lhGxFill(ctx, k, r[0], r[1], r[2], r[3]);\n"
    "    } else if (c[0] === \"fill_mask\") {\n"
    "      lhGxFillMask(ctx, r, k, c[4]);\n"
    "    }\n"
    "    ctx.restore();\n"
    "  }\n"
    "}\n";

lh_str_view_t
lh_gx_backend_web_player_get_script(lh_void)
{
    lh_str_view_t script;

    lh_str_view_init_by_size(lh_addr_of(script), m_gx_backend_web_player_script,
                             lh_math_sub_one(sizeof(m_gx_backend_web_player_script)));
    return script;
}
