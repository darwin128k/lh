/**
 * @file ui.h
 * @brief UI model: every public `lh/gl` header in one include.
 *
 * Geometry (::lh_gl_scalar_t, ::lh_gl_point_t, ::lh_gl_size_t,
 * ::lh_gl_rect_t), corner radius, color, paint / brush / pen, gradients and
 * their samplers, style, blur, the canvases a tree draws on (software, box,
 * web), alpha masks, bitmap fonts and text, the entity tree (container,
 * label, scrollbar, button, toggle, input) and ::lh_gl_view_t, frame stats,
 * display / present sinks, the 3D viewport, and the web page / HTML / JS /
 * WebSocket helpers.
 *
 * Option-gated headers are included only when their option is on:
 * `lh/gl/font/roboto.h` (::LH_LIBRARY_OPTION_GL_FONT_ROBOTO),
 * `lh/gl/surface.h` (::LH_LIBRARY_OPTION_OS and ::LH_LIBRARY_OPTION_OS_WINDOW)
 * and `lh/gl/web/host.h` (::LH_LIBRARY_OPTION_OS).
 *
 * Requires ::LH_LIBRARY_OPTION_GL.
 */

#ifndef LH_GL_H
#define LH_GL_H

#include <lh/config.h>

#if !LH_LIBRARY_OPTION_GL
#    error "lh/gl.h requires LH_LIBRARY_OPTION_GL (CMake: -DLH_LIBRARY_OPTION_GL=ON)"
#endif

#include <lh/gl/blur.h>
#include <lh/gl/brush.h>
#include <lh/gl/canvas/box.h>
#include <lh/gl/canvas/mask.h>
#include <lh/gl/canvas/round.h>
#include <lh/gl/canvas/web.h>
#include <lh/gl/decl.h>
#include <lh/gl/display.h>
#include <lh/gl/frame/stats.h>
#include <lh/gl/gradient/sine.h>
#include <lh/gl/pen.h>
#include <lh/gl/range.h>
#include <lh/gl/view.h>
#include <lh/gl/view/controller.h>
#include <lh/gl/viewport3d.h>
#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/gl/renderer3d.h>
#endif
#include <lh/gl/web/player/page.h>
#include <lh/gl/web/ws.h>
#if LH_LIBRARY_OPTION_GL_FONT_ROBOTO
#    include <lh/gl/font/roboto.h>
#endif
#if LH_LIBRARY_OPTION_OS && LH_LIBRARY_OPTION_OS_WINDOW
#    include <lh/gl/surface.h>
#endif
#if LH_LIBRARY_OPTION_OS
#    include <lh/gl/web/host.h>
#endif

#endif /* LH_GL_H */
