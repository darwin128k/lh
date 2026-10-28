/**
 * @file kind.h
 * @brief ::lh_ui_decl_kind_t — what a row builds. Each is the component of the same name, and each is the thing an app would have declared by hand.
 */

#ifndef LH_UI_DECL_KIND_H
#define LH_UI_DECL_KIND_H

/**
 * @enum lh_ui_decl_kind
 * @brief What a row builds. Each is the component of the same name, and each is
 *        the thing an app would have declared by hand.
 */
typedef enum lh_ui_decl_kind
{
    lh_ui_decl_kind_entity = 0, /**< A plain ::lh_ui_entity_t: a fill and nothing else. */
    lh_ui_decl_kind_container,  /**< ::lh_ui_container_t, with the flow that places its children. */
    lh_ui_decl_kind_label,      /**< ::lh_ui_label_t and its text. */
    lh_ui_decl_kind_button,     /**< ::lh_ui_button_t, its two looks and its click. */
    lh_ui_decl_kind_toggle,     /**< ::lh_ui_toggle_t, both pairs of looks and its state. */
    lh_ui_decl_kind_image,      /**< ::lh_ui_image_t, its mask and its tint. */
    lh_ui_decl_kind_scrollbar,  /**< ::lh_ui_scrollbar_t over the container of another row. */
    lh_ui_decl_kind_input       /**< ::lh_ui_input_t over a buffer the app owns. */
} lh_ui_decl_kind_t;            /**< What a row builds. */

#endif /* LH_UI_DECL_KIND_H */
