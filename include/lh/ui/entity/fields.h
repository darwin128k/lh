/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_entity_t.
 */

#ifndef LH_UI_ENTITY_FIELDS_H
#define LH_UI_ENTITY_FIELDS_H

struct lh_ui_view;

#include <lh/compiler/cxx.h>
#include <lh/list.h>
#include <lh/list/node.h>

/**
 * @def lh_ui_entity_fields(rect_type, style_type, class_type, bool_type, entity_type)
 * @brief The area this entity covers, how it is painted, the class it
 *        belongs to, visibility, pressed state, parent, and its place in a
 *        child list.
 *
 * The rect is in the same space as the parent's (absolute), not relative to
 * it. The style is not copied: several entities may share one. Children are
 * not owned: the caller keeps them alive. When `hidden` is true, paint and
 * hit tests skip this node and its descendants. `pressed` is what the pointer
 * is doing on it right now, kept by the view
 * (::lh_ui_view_press / ::lh_ui_view_release): it paints the pressed style of
 * ::lh_ui_style_t when there is one, and changes nothing about the entity.
 * `parent` is ::lh_null for a root. `view` is set on that root by
 * ::lh_ui_view_set_root and is ::lh_null everywhere else: a setter that changes
 * what is on screen walks up to it and records one rect
 * (::lh_ui_entity_note). In C++ the class member is `klass` (`class` is a
 * keyword).
 *
 * @param rect_type   Type of the area.
 * @param style_type  Type of the paint recipe pointed at.
 * @param class_type  Type of the class pointer.
 * @param bool_type   Type of the hidden and pressed flags.
 * @param entity_type Type of the parent pointer.
 * @param place_type  Type of ::lh_ui_place_t — how this entity wants a flow to place it.
 */
#ifdef LH_COMPILER_CXX
#    define lh_ui_entity_fields(rect_type, style_type, class_type, bool_type, entity_type, place_type)           \
        rect_type rect;                                                                             \
        const style_type *style;                                                                    \
        const class_type *klass;                                                                    \
        bool_type hidden;                                                                           \
        bool_type pressed;                                                                          \
        entity_type *parent;                                                                        \
        struct lh_ui_view *view;                                                                    \
        place_type place;                                                                            \
        lh_list_t children;                                                                         \
        lh_list_node_t link
#else
#    define lh_ui_entity_fields(rect_type, style_type, class_type, bool_type, entity_type, place_type)           \
        rect_type rect;                                                                             \
        const style_type *style;                                                                    \
        const class_type *class;                                                                    \
        bool_type hidden;                                                                           \
        bool_type pressed;                                                                          \
        entity_type *parent;                                                                        \
        struct lh_ui_view *view;                                                                    \
        place_type place;                                                                            \
        lh_list_t children;                                                                         \
        lh_list_node_t link
#endif

#endif /* LH_UI_ENTITY_FIELDS_H */
