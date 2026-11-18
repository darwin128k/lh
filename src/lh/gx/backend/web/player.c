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
    "function lhGxFillMask(ctx, r, k, cover) {\n"
    "  const w = r[2], h = r[3];\n"
    "  if (w <= 0 || h <= 0) return;\n"
    "  const mask = new OffscreenCanvas(w, h);\n"
    "  const mc = mask.getContext(\"2d\");\n"
    "  const img = mc.createImageData(w, h);\n"
    "  const d = img.data, a = k >>> 24;\n"
    "  for (let i = 0; i < w * h; ++i) {\n"
    "    d[4 * i] = (k >>> 16) & 255;\n"
    "    d[4 * i + 1] = (k >>> 8) & 255;\n"
    "    d[4 * i + 2] = k & 255;\n"
    "    d[4 * i + 3] = Math.round(parseInt(cover.substr(2 * i, 2), 16) * a / 255);\n"
    "  }\n"
    "  mc.putImageData(img, 0, 0);\n"
    "  ctx.drawImage(mask, r[0], r[1]);\n"
    "}\n"
    "function lhGxPlay(ctx, frame) {\n"
    "  for (const c of frame) {\n"
    "    const clip = c[1], r = c[2], k = c[3];\n"
    "    ctx.save();\n"
    "    ctx.beginPath();\n"
    "    ctx.rect(clip[0], clip[1], clip[2], clip[3]);\n"
    "    ctx.clip();\n"
    "    if (c[0] === \"fill_rect\") {\n"
    "      ctx.fillStyle = lhGxColor(k);\n"
    "      ctx.fillRect(r[0], r[1], r[2], r[3]);\n"
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
