/**
 * @file kind.h
 * @brief ::lh_gl_decl_kind_t — what a row builds. Each is the component of the same name, and each is the thing an app would have declared by hand.
 */

#ifndef LH_GL_DECL_KIND_H
#define LH_GL_DECL_KIND_H

/**
 * @enum lh_gl_decl_kind
 * @brief What a row builds. Each is the component of the same name, and each is
 *        the thing an app would have declared by hand.
 */
typedef enum lh_gl_decl_kind
{
    lh_gl_decl_kind_entity = 0, /**< A plain ::lh_gl_entity_t: a fill and nothing else. */
    lh_gl_decl_kind_container,  /**< ::lh_gl_container_t, with the flow that places its children. */
    lh_gl_decl_kind_label,      /**< ::lh_gl_label_t and its text. */
    lh_gl_decl_kind_button,     /**< ::lh_gl_button_t, its two looks and its click. */
    lh_gl_decl_kind_toggle,     /**< ::lh_gl_toggle_t, both pairs of looks and its state. */
    lh_gl_decl_kind_image,      /**< ::lh_gl_image_t, its mask and its tint. */
    lh_gl_decl_kind_scrollbar,  /**< ::lh_gl_scrollbar_t over the container of another row. */
    lh_gl_decl_kind_input       /**< ::lh_gl_input_t over a buffer the app owns. */
} lh_gl_decl_kind_t;            /**< What a row builds. */

#endif /* LH_GL_DECL_KIND_H */
