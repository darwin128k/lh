/**
 * @file fields.h
 * @brief Member fields of ::lh_entity_event_t.
 */

#ifndef LH_ENTITY_EVENT_FIELDS_H
#define LH_ENTITY_EVENT_FIELDS_H

/**
 * @def lh_entity_event_fields(code_type, canvas_type, brush_type, pen_type)
 * @brief Which event this is, and the draw arguments that came with it.
 *
 * @param code_type   Type of the event code.
 * @param canvas_type Type of the canvas pointer.
 * @param brush_type  Type of the brush pointer.
 * @param pen_type    Type of the pen pointer.
 */
#define lh_entity_event_fields(code_type, canvas_type, brush_type, pen_type)                        \
    code_type code;                                                                                 \
    canvas_type *canvas;                                                                            \
    brush_type *brush;                                                                              \
    pen_type *pen

#endif /* LH_ENTITY_EVENT_FIELDS_H */
