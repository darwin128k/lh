/**
 * @file damage.h
 * @brief The part of the screen that changed: ::lh_gx_damage_t.
 *
 * One rect is the wrong shape for a region. The hull of two cuts on the same
 * strip is the rectangle spanning the space between them: a stretch of pixels
 * that belongs to neither cut. A backend that presents its drawn region as
 * one rect therefore presents that gap too, and the bytes it holds are
 * whatever the buffer it just reallocated gave back.
 *
 * This is the shape that does not have that problem: rects that join are
 * merged, rects that do not are kept apart, and the result is read one rect at
 * a time. The table is fixed and never reallocated (::LH_GX_DAMAGE_MAX),
 * because a drawing path that allocates is a drawing path that cannot be
 * stepped through.
 *
 * The merge rule is the exact one: two rects merge only when their union is
 * exactly a rectangle, so a merge never covers a pixel that did not change.
 * Rects that meet edge to edge merge; rects that overlap in an L do not,
 * because the hull of an L is bigger than the L and the bigger part would be
 * repainted for nothing. The price is slots: an L costs two where a hull
 * would cost one.
 *
 * The damage is a value: the caller owns it, it is copied when it is passed,
 * and no part of gx allocates one or frees one.
 *
 * @see lh_gx_rect_t, the rectangle of one change.
 */

#ifndef LH_GX_DAMAGE_H
#define LH_GX_DAMAGE_H

#include <lh/gx/rect.h>

/**
 * @def LH_GX_DAMAGE_MAX
 * @brief How many rects ::lh_gx_damage_t keeps apart at most.
 *
 * `16`, the same table gl used: more than one strip of any scene so far
 * holds, because rects that join collapse into one, so what a strip really
 * needs is the number of things standing apart in it, not the number drawn.
 * The price is storage: one rect per slot (16 bytes with a 32-bit scalar), so
 * one ::lh_gx_damage_t is about 260 bytes.
 *
 * @note Past this the list stops being the region exactly: the added rect is
 *       merged into whichever stored one the pair wastes the fewest pixels on
 *       (::lh_gx_damage_squeeze). It never drops a rect.
 */
#define LH_GX_DAMAGE_MAX 16

#include <lh/gx/damage/fields.h>

/**
 * @struct lh_gx_damage
 * @brief The changed part of the screen, as up to ::LH_GX_DAMAGE_MAX rects.
 */
struct lh_gx_damage
{
    /** Members: ::lh_gx_damage_fields. */
    lh_gx_damage_fields(lh_gx_rect_t, lh_u32_t);
};
/** @copybrief lh_gx_damage */
typedef struct lh_gx_damage lh_gx_damage_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p self with no rects in it.
 *
 * @param self The damage; must not be null.
 */
lh_void
lh_gx_damage_init(lh_gx_damage_t *self);

/**
 * @brief Nothing in @p self.
 *
 * @param self The damage; must not be null.
 * @return True when no rect is held.
 */
lh_bool_t
lh_gx_damage_is_empty(const lh_gx_damage_t *self);

/**
 * @brief How many rects @p self keeps apart.
 *
 * @param self The damage; must not be null.
 * @return The count.
 */
lh_u32_t
lh_gx_damage_get_count(const lh_gx_damage_t *self);

/**
 * @brief The rect at @p index.
 *
 * @param self  The damage; must not be null.
 * @param index The index of the rect.
 * @return The rect, or ::lh_null when @p index is past the end.
 */
const lh_gx_rect_t *
lh_gx_damage_get_rect(const lh_gx_damage_t *self, lh_u32_t index);

/**
 * @brief The whole table of @p self.
 *
 * @param self The damage; must not be null.
 * @return The table; the entries from the count on hold nothing.
 */
const lh_gx_rect_t *
lh_gx_damage_get_rects(const lh_gx_damage_t *self);

/**
 * @brief The whole table of @p self, to change it in place.
 *
 * @param self The damage; must not be null.
 * @return The table.
 */
lh_gx_rect_t *
lh_gx_damage_get_rects_mut(lh_gx_damage_t *self);

/**
 * @brief Store how many rects @p self holds.
 *
 * @param self  The damage; must not be null.
 * @param count The count; must not be past ::LH_GX_DAMAGE_MAX.
 */
lh_void
lh_gx_damage_set_count(lh_gx_damage_t *self, lh_u32_t count);

/**
 * @brief Add @p rect to @p self, merging it with the rects it joins.
 *
 * An empty rect is ignored. A rect that joins one already held is merged
 * with it, and the merge continues until nothing else joins.
 *
 * @param self The damage; must not be null.
 * @param rect The rect that changed; must not be null.
 */
lh_void
lh_gx_damage_add(lh_gx_damage_t *self, const lh_gx_rect_t *rect);

/**
 * @brief Forget every rect in @p self.
 *
 * @param self The damage; must not be null.
 */
lh_void
lh_gx_damage_clear(lh_gx_damage_t *self);

/**
 * @brief The pixels @p rect covers.
 *
 * @param rect The rect; must not be null.
 * @return The area.
 */
lh_gx_scalar_t
lh_gx_damage_rect_area(const lh_gx_rect_t *rect);

/**
 * @brief The pixels @p self holds, the rects counted one by one.
 *
 * Two rects that overlap are counted twice: the list is what the screen has
 * to repaint, and a repaint of an overlap is a repaint.
 *
 * @param self The damage; must not be null.
 * @return The area.
 */
lh_gx_scalar_t
lh_gx_damage_area(const lh_gx_damage_t *self);

/**
 * @brief True when the union of @p a and @p b is exactly a rectangle.
 *
 * Two rects may be merged into one only when the hull covers nothing that the
 * two did not cover between them. Rects that meet edge to edge merge; rects
 * that overlap in an L do not, because the hull of an L is bigger than the L
 * and the bigger part would be repainted for nothing.
 *
 * @param a The first rect; must not be null.
 * @param b The second rect; must not be null.
 * @return True when the two may be one rect.
 */
lh_bool_t
lh_gx_damage_joins(const lh_gx_rect_t *a, const lh_gx_rect_t *b);

/**
 * @brief The pixels a merge of @p a and @p b would cover for nothing.
 *
 * @param a The first rect; must not be null.
 * @param b The second rect; must not be null.
 * @return The waste; zero when the two join.
 */
lh_gx_scalar_t
lh_gx_damage_waste(const lh_gx_rect_t *a, const lh_gx_rect_t *b);

/**
 * @brief Merge every pair of @p self that joins, until no pair does.
 *
 * @param self The damage; must not be null.
 */
lh_void
lh_gx_damage_close(lh_gx_damage_t *self);

/**
 * @brief Merge @p rect into the rect of @p self that wastes the fewest pixels.
 *
 * The list is full, so the rect cannot be added as one more. It is merged
 * into the stored rect whose hull wastes the least, and the list is closed
 * again. No rect is ever dropped.
 *
 * @param self The damage; must not be null.
 * @param rect The rect that changed; must not be null.
 */
lh_void
lh_gx_damage_squeeze(lh_gx_damage_t *self, const lh_gx_rect_t *rect);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_DAMAGE_H */
