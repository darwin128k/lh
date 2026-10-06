/**
 * @file ui.h
 * @brief UI model: every public `lh/ui` header in one include.
 *
 * Geometry (::lh_ui_scalar_t, ::lh_ui_point_t, ::lh_ui_size_t,
 * ::lh_ui_rect_t), color, paint / brush / pen, gradient, style, the canvas
 * a tree draws on, and the entity tree with its label.
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
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/entity/label.h>
#include <lh/ui/gradient.h>
#include <lh/ui/paint.h>
#include <lh/ui/pen.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/ui/style.h>

#endif /* LH_UI_H */
