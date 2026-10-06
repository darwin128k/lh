/**
 * @file kind.h
 * @brief What a paint holds: ::lh_ui_paint_kind_t.
 */

#ifndef LH_UI_PAINT_KIND_H
#define LH_UI_PAINT_KIND_H

/**
 * @enum lh_ui_paint_kind
 * @brief Kind of an ::lh_ui_paint_t.
 *
 * A gradient kind is added once a renderer can sample ::lh_ui_gradient_t.
 */
typedef enum lh_ui_paint_kind
{
    lh_ui_paint_kind_none = 0, /**< Paints nothing. */
    lh_ui_paint_kind_solid     /**< One solid ::lh_ui_color_t. */
} lh_ui_paint_kind_t;

#endif /* LH_UI_PAINT_KIND_H */
