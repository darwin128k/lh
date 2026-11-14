/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_input_t.
 */

#ifndef LH_GL_INPUT_FIELDS_H
#define LH_GL_INPUT_FIELDS_H

/**
 * @def lh_gl_input_fields(entity_type)
 * @brief The one entity a press is holding.
 *
 * @param entity_type ::lh_gl_entity_t. The member is `captured`, a borrowed
 *                    pointer. Null means nothing is held. The input does not
 *                    own the entity and does not unlink it.
 */
#define lh_gl_input_fields(entity_type)                                                            \
    const entity_type *captured

#endif /* LH_GL_INPUT_FIELDS_H */
