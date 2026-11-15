/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_input_binding_t.
 */

#ifndef LH_GL_INPUT_BINDING_FIELDS_H
#define LH_GL_INPUT_BINDING_FIELDS_H

/**
 * @def lh_gl_input_binding_fields(input_type, layer_type)
 * @brief The hold and the layer a controller searches.
 *
 * Both are borrowed. The binding does not own them.
 *
 * @param input_type ::lh_gl_input_t.
 * @param layer_type ::lh_gl_layer_t.
 */
#define lh_gl_input_binding_fields(input_type, layer_type)                                         \
    input_type *input;                                                                             \
    const layer_type *layer

#endif /* LH_GL_INPUT_BINDING_FIELDS_H */
