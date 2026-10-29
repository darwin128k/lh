/**
 * @file cross.h
 * @brief ::lh_ui_layout_cross_t — where one child stands across the flow: filled by ::lh_ui_layout_cross_fit.
 */

#ifndef LH_UI_LAYOUT_CROSS_H
#define LH_UI_LAYOUT_CROSS_H

#include <lh/ui/entity.h>
#include <lh/ui/layout/fields.h>
#include <lh/ui/justify.h>

/**
 * @struct lh_ui_layout_cross
 * @brief Where one child stands across the flow: filled by
 *        ::lh_ui_layout_cross_fit.
 */
struct lh_ui_layout_cross
{
    /** Cross start of the child's slot (content box less its cross margin). */
    lh_ui_scalar_t start;
    /** The child's own cross length. */
    lh_ui_scalar_t own;
    /** Cross length of the slot. */
    lh_ui_scalar_t room;
    /** How far the slot starts past the content box across. */
    lh_ui_scalar_t before;
};

/** @brief Alias for ::lh_ui_layout_cross. */
typedef struct lh_ui_layout_cross lh_ui_layout_cross_t;

struct lh_ui_layout;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Slot start, own length, slot length and margin offset of @p child
 *        across the flow of @p self in @p content, into @p out.
 *
 * @p out must not be null (::lh_assert_runtime_ref).
 */
lh_void
lh_ui_layout_cross_fit(const struct lh_ui_layout *self, lh_ui_entity_t *child,
                       const lh_ui_rect_t *content, lh_ui_layout_cross_t *out);

/**
 * @brief Offset of a non-fill child inside its slot across, for @p align:
 *        0 at the start, ::lh_ui_scalar_centre when centred, `room - own` at the
 *        end, and on the row's @p line when both @p line and the child's
 *        @p base are not negative (centred otherwise). May be negative; the
 *        caller clamps.
 */
lh_ui_scalar_t
lh_ui_layout_cross_at(const lh_ui_layout_cross_t *fit, lh_ui_place_align_t align,
                      lh_ui_scalar_t base, lh_ui_scalar_t line);

/**
 * @brief Where the line starts on the cross axis (level 1).
 *
 * @param self The cross placement; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_cross_get_start(const lh_ui_layout_cross_t *self);

/**
 * @brief Store where the line starts on the cross axis (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The cross placement; checked by ::lh_assert_runtime_ref.
 * @param start The new value.
 */
lh_void
lh_ui_layout_cross_set_start(lh_ui_layout_cross_t *self, lh_ui_scalar_t start);

/**
 * @brief The extent of the child on the cross axis (level 1).
 *
 * @param self The cross placement; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_cross_get_own(const lh_ui_layout_cross_t *self);

/**
 * @brief Store the extent of the child on the cross axis (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The cross placement; checked by ::lh_assert_runtime_ref.
 * @param own The new value.
 */
lh_void
lh_ui_layout_cross_set_own(lh_ui_layout_cross_t *self, lh_ui_scalar_t own);

/**
 * @brief The room the line has on the cross axis (level 1).
 *
 * @param self The cross placement; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_cross_get_room(const lh_ui_layout_cross_t *self);

/**
 * @brief Store the room the line has on the cross axis (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The cross placement; checked by ::lh_assert_runtime_ref.
 * @param room The new value.
 */
lh_void
lh_ui_layout_cross_set_room(lh_ui_layout_cross_t *self, lh_ui_scalar_t room);

/**
 * @brief The space before the child on the cross axis (level 1).
 *
 * @param self The cross placement; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_layout_cross_get_before(const lh_ui_layout_cross_t *self);

/**
 * @brief Store the space before the child on the cross axis (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The cross placement; checked by ::lh_assert_runtime_ref.
 * @param before The new value.
 */
lh_void
lh_ui_layout_cross_set_before(lh_ui_layout_cross_t *self, lh_ui_scalar_t before);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LAYOUT_CROSS_H */
