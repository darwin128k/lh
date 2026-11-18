/**
 * @file class.h
 * @brief What an entity is and how it behaves: ::lh_gx_class_t.
 *
 * A class is a constant table of slots that every entity of one kind shares:
 * how it paints, whether it covers what is under it, and — for the root of a
 * tree — where its changes go. gx has one entity type (::lh_gx_entity_t); a
 * layer, a card, a label and an effect differ only by their class, and an
 * entity points at one, usually a constant in flash defined with
 * ::lh_gx_class_initializer.
 *
 * Every slot is optional, and each slot's type says what happens without it: a
 * class with nothing at all is a layer — it draws nothing and only holds its
 * children. More slots (hit, input, measure) are added when a class needs them.
 */

#ifndef LH_GX_CLASS_H
#define LH_GX_CLASS_H

#include <lh/gx/class/covers/fn.h>
#include <lh/gx/class/fields.h>
#include <lh/gx/class/paint/fn.h>
#include <lh/gx/class/report/fn.h>

/**
 * @struct lh_gx_class
 * @brief The slots of one kind of entity.
 */
struct lh_gx_class
{
    /** Members: ::lh_gx_class_fields. */
    lh_gx_class_fields(lh_gx_class_paint_fn, lh_gx_class_covers_fn, lh_gx_class_report_fn);
};
/** @copybrief lh_gx_class */
typedef struct lh_gx_class lh_gx_class_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The `paint` slot of @p self (level 1): draw the entity.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the class does not fill it.
 */
lh_gx_class_paint_fn *
lh_gx_class_get_paint(const lh_gx_class_t *self);

/**
 * @brief Store the `paint` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the class does not fill it.
 */
lh_void
lh_gx_class_set_paint(lh_gx_class_t *self, lh_gx_class_paint_fn *fn);

/**
 * @brief The `covers` slot of @p self (level 1): is the entity opaque over an area.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the class does not fill it.
 */
lh_gx_class_covers_fn *
lh_gx_class_get_covers(const lh_gx_class_t *self);

/**
 * @brief Store the `covers` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the class does not fill it.
 */
lh_void
lh_gx_class_set_covers(lh_gx_class_t *self, lh_gx_class_covers_fn *fn);

/**
 * @brief The `report` slot of @p self (level 1): hear a change under the root.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The call, or ::lh_null when the class does not fill it.
 */
lh_gx_class_report_fn *
lh_gx_class_get_report(const lh_gx_class_t *self);

/**
 * @brief Store the `report` slot of @p self (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @param fn   The call; ::lh_null when the class does not fill it.
 */
lh_void
lh_gx_class_set_report(lh_gx_class_t *self, lh_gx_class_report_fn *fn);

/**
 * @brief Empty table: every slot ::lh_null.
 *
 * A class built at run time then sets the slots it has; a class in flash uses
 * ::lh_gx_class_initializer instead. Nothing here owns the table.
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gx_class_init(lh_gx_class_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CLASS_H */
