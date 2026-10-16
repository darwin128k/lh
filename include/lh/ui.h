/**
 * @file ui.h
 * @brief UI model: every public `lh/ui` header in one include.
 *
 * Geometry (::lh_ui_scalar_t, ::lh_ui_point_t, ::lh_ui_size_t,
 * ::lh_ui_rect_t), corner radius, color, paint / brush / pen, gradient, style, the canvas
 * a tree draws on, alpha masks, bitmap fonts and text, and the entity tree
 * with container (scrolling), label and scrollbar, and ::lh_ui_view_t.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_H
#define LH_UI_H

#include <lh/config.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui.h requires LH_LIBRARY_OPTION_UI (CMake: -DLH_LIBRARY_OPTION_UI=ON)"
#endif

#include <lh/ui/brush.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/decl.h>
#include <lh/ui/pen.h>
#include <lh/ui/range.h>
#include <lh/ui/view.h>
#include <lh/ui/blur.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/frame/stats.h>
#if LH_LIBRARY_OPTION_OS && LH_LIBRARY_OPTION_OS_WINDOW
#    include <lh/ui/surface.h>
#endif

#endif /* LH_UI_H */
