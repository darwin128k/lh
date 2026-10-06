/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_entity_event_t.
 */

#ifndef LH_UI_ENTITY_EVENT_FIELDS_H
#define LH_UI_ENTITY_EVENT_FIELDS_H

/**
 * @def lh_ui_entity_event_fields(code_type, context_type)
 * @brief Which event this is, and what it carries.
 *
 * For ::lh_ui_entity_event_draw the context is the ::lh_ui_canvas_t to draw
 * on (may be ::lh_null). For ::lh_ui_entity_event_click it is a
 * ::lh_ui_point_t * in the space of the entity's own rect. For
 * ::lh_ui_entity_event_children it is the ::lh_ui_entity_transform_t * to fill.
 * For ::lh_ui_entity_event_visible it is the ::lh_bool_t * answer. For
 * ::lh_ui_entity_event_measure it is the ::lh_ui_rect_t * content bounds.
 *
 * @param code_type    Type of the event code.
 * @param context_type Type of the per-event context.
 */
#define lh_ui_entity_event_fields(code_type, context_type)                                          \
    code_type code;                                                                                 \
    context_type context

#endif /* LH_UI_ENTITY_EVENT_FIELDS_H */
