/**
 * @file fn.h
 * @brief Function type ::lh_gx_class_report_fn.
 */

#ifndef LH_GX_CLASS_REPORT_FN_H
#define LH_GX_CLASS_REPORT_FN_H

#include <lh/gx/rect.h>

struct lh_gx_entity;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_class_report_fn
 * @brief Hear that @p area of the screen under @p root changed.
 *
 * Optional, and asked only of the root of a tree: an entity that changed
 * reports its rectangle on the screen up through its owners, and the root's
 * class is what hears it — the renderer's screen (::lh_gx_render_t) adds it to
 * the damage. A tree whose root has no `report` is not on a screen, and its
 * changes go nowhere.
 *
 * @param root The root of the tree.
 * @param area The changed rectangle, screen coordinates.
 */
typedef lh_void(lh_gx_class_report_fn)(struct lh_gx_entity *root, const lh_gx_rect_t *area);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CLASS_REPORT_FN_H */
