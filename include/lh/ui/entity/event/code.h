/**
 * @file code.h
 * @brief ::lh_ui_entity_event_code_t — what the face was asked to do.
 */

#ifndef LH_UI_ENTITY_EVENT_CODE_H
#define LH_UI_ENTITY_EVENT_CODE_H

/**
 * @enum lh_ui_entity_event_code
 * @brief What the face was asked to do.
 */
typedef enum lh_ui_entity_event_code
{
    lh_ui_entity_event_draw = 0,         /**< Draw on the canvas in the context. */
    lh_ui_entity_event_click = 1,        /**< Primary click; context is ::lh_ui_point_t *. */
    lh_ui_entity_event_children = 2,     /**< How are the children placed? Context is the
                                              ::lh_ui_entity_transform_t * to fill, preset
                                              to no offset, no clip. */
    lh_ui_entity_event_visible = 3,      /**< Is the entity shown? Context is an
                                              ::lh_bool_t * preset to true; a class may
                                              set it to false. */
    lh_ui_entity_event_measure = 4,      /**< What does the content cover? Context is
                                              the ::lh_ui_rect_t * bounds, preset to the
                                              children bounds; a class may grow it. */
    lh_ui_entity_event_press = 5,        /**< Pointer went down on it; context is the
                                              ::lh_ui_point_t * in its rect space. */
    lh_ui_entity_event_release = 6,      /**< The pointer that went down on it came up
                                              (wherever); context as press. */
    lh_ui_entity_event_scroll_begin = 7, /**< A container starts moving under a
                                              gesture (drag, wheel, keys); no context. */
    lh_ui_entity_event_scroll = 8,       /**< A container's scroll changed; no context. */
    lh_ui_entity_event_scroll_end = 9,   /**< The gesture is over (and any glide after
                                              it); no context. */
    lh_ui_entity_event_focusable = 10,   /**< May it take keyboard focus? Context is an
                                              ::lh_bool_t * preset to false. */
    lh_ui_entity_event_focus = 11,       /**< It took the focus; no context. */
    lh_ui_entity_event_defocus = 12,     /**< It lost the focus; no context. */
    lh_ui_entity_event_key = 13,         /**< A key or a character while focused; context
                                              is the const ::lh_ui_key_input_t *. */
    lh_ui_entity_event_clickable = 14,   /**< Does the pointer belong to it, or to what it
                                             holds? Context is an ::lh_bool_t * preset to
                                             false: a caption and a picture say no, and a
                                             button says yes, so the pointer over a button's
                                             own content reaches the button. */
    lh_ui_entity_event_baseline = 15     /**< Where its first baseline is. Context is an
                                              ::lh_ui_scalar_t * preset to -1, meaning it has
                                              none and a row that asks centres it. */
} lh_ui_entity_event_code_t;             /**< What the face was asked to do. */

#endif /* LH_UI_ENTITY_EVENT_CODE_H */
