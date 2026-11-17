/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_entity_t.
 */

#ifndef LH_GX_ENTITY_FIELDS_H
#define LH_GX_ENTITY_FIELDS_H

#include <lh/core/bool.h>
#include <lh/core/ptr.h>

/**
 * @def lh_gx_entity_fields(node_type, place_type, class_type, style_type)
 * @brief The bricks of one thing on the screen, embedded by value.
 *
 * - `node` — its place in the tree: its owner, its children, its own link
 *   (::lh_gx_node_t);
 * - `place` — where it is and how big it is, relative to its owner;
 * - `visible` — whether it is drawn, hit and laid out at all;
 * - `klass` — *what it is and how it behaves*: a constant table of slots the
 *   entity only points at, one for every entity of its kind (the name is not
 *   `class`, which C++ reserves);
 * - `style` — *how it looks*: a constant record of a theme, shared;
 * - `content` — the class's own data: the text of a label, the reaction of a
 *   button. The application's; the entity only points at it.
 *
 * None of the pointers owns: the class and the style are usually constants in
 * flash, the content belongs to the application.
 *
 * @param node_type  Type of the node (::lh_gx_node_t).
 * @param place_type Type of the place (::lh_gx_rect_t).
 * @param class_type Type of the class (`struct lh_gx_class`).
 * @param style_type Type of the style (`struct lh_gx_style`).
 */
#define lh_gx_entity_fields(node_type, place_type, class_type, style_type)                         \
    node_type node;                                                                                \
    place_type place;                                                                              \
    lh_bool_t visible;                                                                             \
    const class_type *klass;                                                                       \
    const style_type *style;                                                                       \
    lh_ptr content

#endif /* LH_GX_ENTITY_FIELDS_H */
