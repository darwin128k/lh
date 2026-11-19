/**
 * @file shadow.h
 * @brief The alphas of a shadow, kept for the next one like it:
 *        ::lh_gx_backend_sw_shadow_t.
 *
 * A screen of cards casts the same shadow many times: the same box, radius,
 * spread and shift, only somewhere else. Its alphas are the same wherever it
 * falls, so they are worked out once into a mask (::lh_gx_backend_sw_shadow_fill)
 * and every shadow like it is that mask laid down again — "caching the mask, not
 * the pixels" of `docs/design/gx.md`. A shadow is known by the core's shadow of
 * its box laid at `0, 0` (::lh_pixel_shadow_is_equal), so any difference that
 * would change one alpha is a different key.
 *
 * The canvas keeps nothing of the frame: the memory is the application's, handed
 * to the software backend (::lh_gx_backend_sw_set_shadow), and a backend without
 * one works every shadow out where it falls. A board short of memory gives none;
 * a mask too large for the memory is worked out where it falls too.
 */

#ifndef LH_GX_BACKEND_SW_SHADOW_H
#define LH_GX_BACKEND_SW_SHADOW_H

#include <lh/core/pixel/shadow/ramp.h>
#include <lh/core/size.h>
#include <lh/gx/backend/sw/shadow/fields.h>

/**
 * @struct lh_gx_backend_sw_shadow
 * @brief One shadow's alphas. Members: ::lh_gx_backend_sw_shadow_fields.
 */
struct lh_gx_backend_sw_shadow
{
    /** Members: ::lh_gx_backend_sw_shadow_fields. */
    lh_gx_backend_sw_shadow_fields(lh_pixel_shadow_t);
};
/** @copybrief lh_gx_backend_sw_shadow */
typedef struct lh_gx_backend_sw_shadow lh_gx_backend_sw_shadow_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The memory the alphas are kept in (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @return The caller's memory.
 */
const lh_byte_t *
lh_gx_backend_sw_shadow_get_bits(const lh_gx_backend_sw_shadow_t *self);

/**
 * @brief The memory, to write the alphas (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @return The caller's memory.
 */
lh_byte_t *
lh_gx_backend_sw_shadow_get_bits_mut(lh_gx_backend_sw_shadow_t *self);

/**
 * @brief Store the memory (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @param bits The caller's memory; not owned.
 */
lh_void
lh_gx_backend_sw_shadow_set_bits(lh_gx_backend_sw_shadow_t *self, lh_byte_t *bits);

/**
 * @brief How many bytes the memory holds (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_gx_backend_sw_shadow_get_capacity(const lh_gx_backend_sw_shadow_t *self);

/**
 * @brief Store how many bytes the memory holds (level 1).
 *
 * @param self     The mask; checked by ::lh_assert_runtime_ref.
 * @param capacity Bytes.
 */
lh_void
lh_gx_backend_sw_shadow_set_capacity(lh_gx_backend_sw_shadow_t *self, lh_usize_t capacity);

/**
 * @brief Which shadow the alphas are: the core's shadow of the box at `0, 0`
 *        (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @return The mask's own key.
 */
const lh_pixel_shadow_t *
lh_gx_backend_sw_shadow_get_key(const lh_gx_backend_sw_shadow_t *self);

/**
 * @brief Store which shadow the alphas are (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @param key  The shadow; copied.
 */
lh_void
lh_gx_backend_sw_shadow_set_key(lh_gx_backend_sw_shadow_t *self, const lh_pixel_shadow_t *key);

/**
 * @brief The width of the alphas kept, pixels (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gx_backend_sw_shadow_get_width(const lh_gx_backend_sw_shadow_t *self);

/**
 * @brief Store the width of the alphas kept (level 1).
 *
 * @param self  The mask; checked by ::lh_assert_runtime_ref.
 * @param width Pixels.
 */
lh_void
lh_gx_backend_sw_shadow_set_width(lh_gx_backend_sw_shadow_t *self, lh_s32_t width);

/**
 * @brief The height of the alphas kept, pixels (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gx_backend_sw_shadow_get_height(const lh_gx_backend_sw_shadow_t *self);

/**
 * @brief Store the height of the alphas kept (level 1).
 *
 * @param self   The mask; checked by ::lh_assert_runtime_ref.
 * @param height Pixels.
 */
lh_void
lh_gx_backend_sw_shadow_set_height(lh_gx_backend_sw_shadow_t *self, lh_s32_t height);

/**
 * @brief Whether the memory holds the alphas of a shadow yet (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gx_backend_sw_shadow_is_filled(const lh_gx_backend_sw_shadow_t *self);

/**
 * @brief Store whether the memory holds the alphas of a shadow (level 1).
 *
 * @param self   The mask; checked by ::lh_assert_runtime_ref.
 * @param filled The flag.
 */
lh_void
lh_gx_backend_sw_shadow_set_filled(lh_gx_backend_sw_shadow_t *self, lh_bool_t filled);

/**
 * @brief An empty mask over @p capacity bytes of @p bits.
 *
 * @param self     The mask; checked by the setters.
 * @param bits     The caller's memory; not owned; it must outlive the mask's use.
 * @param capacity Bytes.
 */
lh_void
lh_gx_backend_sw_shadow_init(lh_gx_backend_sw_shadow_t *self, lh_byte_t *bits, lh_usize_t capacity);

/**
 * @brief Whether the memory can hold the alphas of a @p width by @p height shadow.
 *
 * @param self   The mask; checked by the getters.
 * @param width  Pixels.
 * @param height Pixels.
 */
lh_bool_t
lh_gx_backend_sw_shadow_can_hold(const lh_gx_backend_sw_shadow_t *self, lh_s32_t width,
                                 lh_s32_t height);

/**
 * @brief Whether the mask holds the alphas of @p key over @p width by @p height.
 *
 * @param self   The mask; checked by the getters.
 * @param key    The shadow of a box at `0, 0`; must not be null.
 * @param width  Pixels.
 * @param height Pixels.
 */
lh_bool_t
lh_gx_backend_sw_shadow_is_for(const lh_gx_backend_sw_shadow_t *self, const lh_pixel_shadow_t *key,
                               lh_s32_t width, lh_s32_t height);

/**
 * @brief Work the alphas of @p key out into the memory: @p width by @p height of
 *        them from column @p x, row @p y (relative to the box at `0, 0`), row by
 *        row (::lh_pixel_shadow_get_span); the mask then holds them. The memory
 *        must hold them (::lh_gx_backend_sw_shadow_can_hold).
 *
 * @param self   The mask; checked by the accessors.
 * @param key    The shadow of a box at `0, 0`; must not be null; copied.
 * @param x      The first column, relative to the box.
 * @param y      The first row.
 * @param width  Pixels.
 * @param height Pixels.
 */
lh_void
lh_gx_backend_sw_shadow_fill(lh_gx_backend_sw_shadow_t *self, const lh_pixel_shadow_t *key,
                             lh_s32_t x, lh_s32_t y, lh_s32_t width, lh_s32_t height);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BACKEND_SW_SHADOW_H */
