/**
 * @file viewport3d.h
 * @brief A GPU pane in the window, not a canvas JSON op.
 *
 * ::lh_ui_canvas_backend_web records 2D primitives. A mesh, a camera and a
 * gizmo do not belong in that stream: they would become tens of thousands of
 * ops the way a software shadow once did. The UI tree still owns the **rect**
 * (a hole in the 2D frame). This backend paints into that rect — native GL
 * on the desktop, WebGL in the browser player. XP stays GDI and does not
 * load the player.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_VIEWPORT3D_H
#define LH_UI_VIEWPORT3D_H

#include <lh/config.h>
#include <lh/ui/rect.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/viewport3d.h requires LH_LIBRARY_OPTION_UI"
#endif

/**
 * @brief One GPU sink. Slots may be ::lh_null until that renderer exists.
 */
struct lh_ui_viewport3d_backend
{
    /** Start drawing the 3D pane into @p rect. */
    lh_void (*begin)(lh_ptr context, const lh_ui_rect_t *rect);
    /** Finish the 3D pane started by ::begin. */
    lh_void (*end)(lh_ptr context);
};
/** @copybrief lh_ui_viewport3d_backend */
typedef struct lh_ui_viewport3d_backend lh_ui_viewport3d_backend_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Accepts every call and draws nothing. */
extern const lh_ui_viewport3d_backend_t lh_ui_viewport3d_backend_null;

/**
 * @brief Start a frame in @p rect (window / canvas space). ::lh_null @p rect hides it.
 */
lh_void
lh_ui_viewport3d_begin(const lh_ui_viewport3d_backend_t *backend, lh_ptr context,
                       const lh_ui_rect_t *rect);

/**
 * @brief Finish the frame.
 */
lh_void
lh_ui_viewport3d_end(const lh_ui_viewport3d_backend_t *backend, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_VIEWPORT3D_H */
