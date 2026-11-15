/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_gradient_fill_t.
 */

#ifndef LH_GL_GRADIENT_FILL_FIELDS_H
#define LH_GL_GRADIENT_FILL_FIELDS_H

/**
 * @def lh_gl_gradient_fill_fields(entity_type, gradient_type, scalar_type, bool_type)
 * @brief The entity a gradient fill starts with, and the gradient it borrows.
 *
 * The `entity` member comes first, so a pointer to the fill is also a pointer
 * to its entity.
 *
 * @param entity_type   ::lh_gl_entity_t. Must be the first member.
 * @param gradient_type ::lh_gl_gradient_t. The member is `gradient`, borrowed.
 *                      Null draws nothing.
 * @param scalar_type   ::lh_gl_scalar_t. The member `angle` is degrees:
 *                      `0` points up, `90` right, `180` down. The member `radius`
 *                      rounds the rectangle the gradient is clipped to. `0` leaves
 *                      the rectangle.
 * @param bool_type     ::lh_bool_t. The member `radial` is true when the paint
 *                      spreads from the middle. False keeps the angle. `shown`
 *                      draws. `target` takes a press.
 */
#define lh_gl_gradient_fill_fields(entity_type, gradient_type, scalar_type, bool_type)             \
    entity_type entity;                                                                            \
    const gradient_type *gradient;                                                                 \
    scalar_type angle;                                                                             \
    scalar_type radius;                                                                            \
    bool_type radial;                                                                              \
    bool_type shown;                                                                               \
    bool_type target

#endif /* LH_GL_GRADIENT_FILL_FIELDS_H */
