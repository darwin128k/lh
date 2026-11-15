/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_input_t.
 */

#ifndef LH_GL_INPUT_FIELDS_H
#define LH_GL_INPUT_FIELDS_H

#include <lh/core/bool.h>

/**
 * @def lh_gl_input_fields(entity_type, flag_type)
 * @brief The one entity a press is holding, and which pointer owns it.
 *
 * @param entity_type ::lh_gl_entity_t. The member is `captured`, a borrowed
 *                    pointer. Null means nothing is held. The input does not
 *                    own the entity and does not unlink it.
 * @param flag_type   ::lh_bool_t. The member is `tracking`: true when
 *                    ::lh_gl_input_apply is following `pointer`.
 */
#define lh_gl_input_fields(entity_type, flag_type, scalar_type)                                    \
    const entity_type *captured;                                                                   \
    lh_u8_t pointer;                                                                               \
    flag_type tracking;                                                                            \
    scalar_type slop

#endif /* LH_GL_INPUT_FIELDS_H */
