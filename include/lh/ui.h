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

#include <lh/ui/axis.h>
#include <lh/ui/brush.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/button.h>
#include <lh/ui/container.h>
#include <lh/ui/label.h>
#include <lh/ui/scrollbar.h>
#include <lh/ui/toggle.h>
#include <lh/ui/font.h>
#include <lh/ui/gradient.h>
#include <lh/ui/image.h>
#include <lh/ui/insets.h>
#include <lh/ui/key.h>
#include <lh/ui/layout.h>
#include <lh/ui/layout/place.h>
#include <lh/ui/mask.h>
#include <lh/ui/paint.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/pen.h>
#include <lh/ui/point.h>
#include <lh/ui/radius.h>
#include <lh/ui/range.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/ui/style.h>
#include <lh/ui/text.h>
#include <lh/ui/view.h>
#if LH_LIBRARY_OPTION_OS && LH_LIBRARY_OPTION_OS_WINDOW
#    include <lh/ui/surface.h>
#endif

#endif /* LH_UI_H */
