/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_entity_t.
 */

#ifndef LH_UI_ENTITY_FIELDS_H
#define LH_UI_ENTITY_FIELDS_H

#include <lh/compiler/cxx.h>

/**
 * @def lh_ui_entity_fields(rect_type, style_type, class_type)
 * @brief The area this entity covers, how it is painted, and the class it
 *        belongs to.
 *
 * The style is not copied: several entities may share one.
 * In C++ the class member is `klass` (`class` is a keyword).
 *
 * @param rect_type  Type of the area.
 * @param style_type Type of the paint recipe pointed at.
 * @param class_type Type of the class pointer.
 */
#ifdef LH_COMPILER_CXX
#    define lh_ui_entity_fields(rect_type, style_type, class_type)                                 \
        rect_type rect;                                                                             \
        const style_type *style;                                                                    \
        const class_type *klass
#else
#    define lh_ui_entity_fields(rect_type, style_type, class_type)                                 \
        rect_type rect;                                                                             \
        const style_type *style;                                                                    \
        const class_type *class
#endif

#endif /* LH_UI_ENTITY_FIELDS_H */
