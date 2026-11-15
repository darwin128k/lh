/**
 * @file class.h
 * @brief What is shared by every entity of one kind: ::lh_gl_entity_class_t, a
 *        small table with a `kind`, a draw call and an opaque flag.
 *
 * An entity does not carry its behaviour; it points at a class
 * (::lh_gl_entity_get_class). A class is a `const` table, normally one per
 * kind of entity, defined once and placed in flash. It holds the `kind`
 * (::lh_gl_entity_class_kind_t), which says whether the object behind an
 * entity pointer is a plain entity or a ::lh_gl_layer_t, the `draw_cb`
 * (::lh_gl_entity_class_draw_cb), which draws an entity of the class on a
 * canvas, the `pick_cb` (::lh_gl_entity_class_pick_cb), which answers a press,
 * and `opaque`, which says the entity covers every pixel of its rectangle.
 *
 * A class knows nothing about input, styles or drivers; those are separate
 * objects that act on an entity from outside. Fields are private: they are read
 * and written only by the level-1 accessors.
 */

#ifndef LH_GL_ENTITY_CLASS_H
#define LH_GL_ENTITY_CLASS_H

#include <lh/gl/entity/class/draw/cb.h>
#include <lh/gl/entity/class/fields.h>
#include <lh/gl/entity/class/kind.h>
#include <lh/gl/entity/class/pick/cb.h>

/**
 * @struct lh_gl_entity_class
 * @brief The table shared by entities of one kind: `kind`, `draw_cb`, `pick_cb`, `opaque`.
 */
struct lh_gl_entity_class
{
    /** Members: ::lh_gl_entity_class_fields. */
    lh_gl_entity_class_fields(lh_gl_entity_class_kind_t, lh_gl_entity_class_draw_fn,
                              lh_gl_entity_class_pick_fn, lh_bool_t);
};
/** @copybrief lh_gl_entity_class */
typedef struct lh_gl_entity_class lh_gl_entity_class_t;

/* -- Accessors ------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The kind of @p self: entity or layer.
 *
 * @param self Class to read; must not be null (asserted in debug builds).
 * @return One of the `lh_gl_entity_class_kind_*` constants.
 */
lh_gl_entity_class_kind_t
lh_gl_entity_class_get_kind(const lh_gl_entity_class_t *self);

/**
 * @brief Store the kind of @p self.
 *
 * @param self Class to change; must not be null (asserted in debug builds).
 * @param kind One of the `lh_gl_entity_class_kind_*` constants.
 */
lh_void
lh_gl_entity_class_set_kind(lh_gl_entity_class_t *self, lh_gl_entity_class_kind_t kind);

/**
 * @brief The call that draws an entity of @p self.
 *
 * @param self Class to read; must not be null (asserted in debug builds).
 * @return The draw call; may be ::lh_null, which means "draws nothing".
 */
lh_gl_entity_class_draw_cb
lh_gl_entity_class_get_draw_cb(const lh_gl_entity_class_t *self);

/**
 * @brief Store the call that draws an entity of @p self.
 *
 * @param self    Class to change; must not be null (asserted in debug builds).
 * @param draw_cb The draw call; ::lh_null for an entity that draws nothing.
 */
lh_void
lh_gl_entity_class_set_draw_cb(lh_gl_entity_class_t *self, lh_gl_entity_class_draw_cb draw_cb);

/** @brief The call that answers a press, or ::lh_null when the rectangle stands. */
lh_gl_entity_class_pick_cb
lh_gl_entity_class_get_pick_cb(const lh_gl_entity_class_t *self);

/** @brief Store the call that answers a press. ::lh_null keeps the rectangle. */
lh_void
lh_gl_entity_class_set_pick_cb(lh_gl_entity_class_t *self, lh_gl_entity_class_pick_cb pick_cb);

/**
 * @brief True when an entity of @p self covers every pixel of its rectangle.
 *
 * @param self Class to read; must not be null (asserted in debug builds).
 */
lh_bool_t
lh_gl_entity_class_get_opaque(const lh_gl_entity_class_t *self);

/**
 * @brief Store whether an entity of @p self covers its rectangle.
 *
 * @param self   Class to change; must not be null (asserted in debug builds).
 * @param opaque ::lh_bool_true when the rectangle is fully covered.
 */
lh_void
lh_gl_entity_class_set_opaque(lh_gl_entity_class_t *self, lh_bool_t opaque);

/* -- Init and queries, by level -------------------------------------------- */

/**
 * @brief Fill @p self with its kind and its draw call. The class is not opaque.
 *
 * @param self    Class to fill; must not be null (asserted in debug builds).
 * @param kind    One of the `lh_gl_entity_class_kind_*` constants.
 * @param draw_cb The draw call; ::lh_null for an entity that draws nothing.
 */
lh_void
lh_gl_entity_class_init(lh_gl_entity_class_t *self, lh_gl_entity_class_kind_t kind,
                        lh_gl_entity_class_draw_cb draw_cb);

/**
 * @brief True when entities of @p self are layers.
 *
 * @param self Class to read; must not be null (asserted in debug builds).
 */
lh_bool_t
lh_gl_entity_class_is_layer(const lh_gl_entity_class_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_ENTITY_CLASS_H */