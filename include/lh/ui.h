/**
 * @file ui.h
 * @brief UI model: every public `lh/ui` header in one include.
 *
 * Geometry (::lh_ui_scalar_t, ::lh_ui_point_t, ::lh_ui_size_t,
 * ::lh_ui_rect_t), corner radius, color, paint / brush / pen, gradients and
 * their samplers, style, blur, the canvases a tree draws on (software, box,
 * web), alpha masks, bitmap fonts and text, the entity tree (container,
 * label, scrollbar, button, toggle, input) and ::lh_ui_view_t, frame stats,
 * display / present sinks, the 3D viewport, and the web page / HTML / JS /
 * WebSocket helpers.
 *
 * Option-gated headers are included only when their option is on:
 * `lh/ui/font/roboto.h` (::LH_LIBRARY_OPTION_UI_FONT_ROBOTO),
 * `lh/ui/surface.h` (::LH_LIBRARY_OPTION_OS and ::LH_LIBRARY_OPTION_OS_WINDOW)
 * and `lh/ui/web/host.h` (::LH_LIBRARY_OPTION_OS).
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_H
#define LH_UI_H

#include <lh/config.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui.h requires LH_LIBRARY_OPTION_UI (CMake: -DLH_LIBRARY_OPTION_UI=ON)"
#endif

#include <lh/ui/blur.h>
#include <lh/ui/brush.h>
#include <lh/ui/canvas/box.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/decl.h>
#include <lh/ui/display.h>
#include <lh/ui/frame/stats.h>
#include <lh/ui/gradient/sine.h>
#include <lh/ui/pen.h>
#include <lh/ui/range.h>
#include <lh/ui/view.h>
#include <lh/ui/view/controller.h>
#include <lh/ui/viewport3d.h>
#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/ui/renderer3d.h>
#endif
#include <lh/ui/web/html.h>
#include <lh/ui/web/js.h>
#include <lh/ui/web/js/x.h>
#include <lh/ui/web/page.h>
#include <lh/ui/web/ws.h>
#if LH_LIBRARY_OPTION_UI_FONT_ROBOTO
#    include <lh/ui/font/roboto.h>
#endif
#if LH_LIBRARY_OPTION_OS && LH_LIBRARY_OPTION_OS_WINDOW
#    include <lh/ui/surface.h>
#endif
#if LH_LIBRARY_OPTION_OS
#    include <lh/ui/web/host.h>
#endif

#endif /* LH_UI_H */
