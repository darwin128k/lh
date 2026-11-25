/**
 * @file sample.h
 * @brief One contact a driver read: ::lh_input_sample_t.
 *
 * A touch driver fills a list of these and does not build events. Being in
 * the list means the contact is down. ::lh_input_touch_report turns the
 * list into pointer events. The id is the driver's own, not a pointer id.
 *
 * Needs no ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_INPUT_SAMPLE_H
#define LH_INPUT_SAMPLE_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/input/contact/id.h>
#include <lh/core/input/coord.h>
#include <lh/core/input/sample/fields.h>
#include <lh/core/input/sample/ptr.h>

/**
 * @struct lh_input_sample
 * @brief One contact in a driver's frame.
 */
struct lh_input_sample
{
    /** Members: ::lh_input_sample_fields. */
    lh_input_sample_fields(lh_input_contact_id_t, lh_input_coord_t);
};
/** @copybrief lh_input_sample */
typedef struct lh_input_sample lh_input_sample_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The driver's id of @p self. */
lh_input_contact_id_t
lh_input_sample_get_contact(const lh_input_sample_t *self);

/** @brief Store the driver's id. */
lh_void
lh_input_sample_set_contact(lh_input_sample_t *self, lh_input_contact_id_t contact);

/** @brief Client x, Q24.8. */
lh_input_coord_t
lh_input_sample_get_x(const lh_input_sample_t *self);

/** @brief Store client x. */
lh_void
lh_input_sample_set_x(lh_input_sample_t *self, lh_input_coord_t x);

/** @brief Client y, Q24.8. */
lh_input_coord_t
lh_input_sample_get_y(const lh_input_sample_t *self);

/** @brief Store client y. */
lh_void
lh_input_sample_set_y(lh_input_sample_t *self, lh_input_coord_t y);

/** @brief @p contact at (@p x, @p y). */
lh_void
lh_input_sample_init(lh_input_sample_t *self, lh_input_contact_id_t contact, lh_input_coord_t x,
                     lh_input_coord_t y);

/**
 * @brief The index of @p contact in @p samples, into @p index.
 *
 * @return False, @p index untouched, when @p count is 0, @p samples is null,
 *         or no entry has @p contact. The first match wins.
 */
lh_bool_t
lh_input_sample_find(const lh_input_sample_t *samples, lh_u8_t count, lh_input_contact_id_t contact,
                     lh_u8_t *index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_INPUT_SAMPLE_H */
