/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_entity_t.
 */

#ifndef LH_UI_ENTITY_FIELDS_H
#define LH_UI_ENTITY_FIELDS_H

#include <lh/compiler/cxx.h>
#include <lh/list.h>
#include <lh/list/node.h>

/**
 * @def lh_ui_entity_fields(rect_type, style_type, class_type)
 * @brief The area this entity covers, how it is painted, the class it
 *        belongs to, and its place in a parent/child tree.
 *
 * The style is not copied: several entities may share one. Children are not
 * owned: the caller keeps them alive. In C++ the class member is `klass`
 * (`class` is a keyword).
 *
 * @param rect_type  Type of the area.
 * @param style_type Type of the paint recipe pointed at.
 * @param class_type Type of the class pointer.
 */
#ifdef LH_COMPILER_CXX
#    define lh_ui_entity_fields(rect_type, style_type, class_type)                                 \
        rect_type rect;                                                                             \
        const style_type *style;                                                                    \
        const class_type *klass;                                                                    \
        lh_list_t children;                                                                         \
        lh_list_node_t link
#else
#    define lh_ui_entity_fields(rect_type, style_type, class_type)                                 \
        rect_type rect;                                                                             \
        const style_type *style;                                                                    \
        const class_type *class;                                                                    \
        lh_list_t children;                                                                         \
        lh_list_node_t link
#endif

#endif /* LH_UI_ENTITY_FIELDS_H */
