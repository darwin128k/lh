/**
 * @file seen.h
 * @brief What a label last saw of its text: ::lh_gl_label_seen_t.
 *
 * The hash and the length of the text the label last showed, which is how
 * ::lh_gl_label_set_text_v tells a new string from the same one when the
 * caller has already overwritten the buffer the label points at, and `noted`,
 * where that text's ink was the last time the label recorded it, **relative to
 * the label's own origin**: the one way left to erase an old text whose bytes
 * are gone. A part of ::lh_gl_label_t that lives inside it
 * (::lh_gl_label_get_seen).
 */

#ifndef LH_GL_LABEL_SEEN_H
#define LH_GL_LABEL_SEEN_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>
#include <lh/gl/label/seen/fields.h>
#include <lh/gl/rect.h>

struct lh_font;

/**
 * @struct lh_gl_label_seen
 * @brief What a label last saw of its text.
 */
struct lh_gl_label_seen
{
    /** Members: ::lh_gl_label_seen_fields. */
    lh_gl_label_seen_fields();
};
/** @copybrief lh_gl_label_seen */
typedef struct lh_gl_label_seen lh_gl_label_seen_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Where the text's ink was last recorded, read in place (level 1).
 *
 * @param self The seen; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gl_rect_t *
lh_gl_label_seen_get_noted(const lh_gl_label_seen_t *self);

/**
 * @brief Where the text's ink was last recorded, to change in place (level 1).
 *
 * @param self The seen; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gl_rect_t *
lh_gl_label_seen_get_noted_mut(lh_gl_label_seen_t *self);

/**
 * @brief Store where the text's ink was last recorded (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The seen; checked by ::lh_assert_runtime_ref.
 * @param noted The new value.
 */
lh_void
lh_gl_label_seen_set_noted(lh_gl_label_seen_t *self, lh_gl_rect_t noted);

/**
 * @brief The hash of the text last shown (level 1).
 *
 * @param self The seen; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_gl_label_seen_get_hash(const lh_gl_label_seen_t *self);

/**
 * @brief Store the hash of the text last shown (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The seen; checked by ::lh_assert_runtime_ref.
 * @param hash The new value.
 */
lh_void
lh_gl_label_seen_set_hash(lh_gl_label_seen_t *self, lh_u32_t hash);

/**
 * @brief The length of the text last shown (level 1).
 *
 * @param self The seen; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_gl_label_seen_get_length(const lh_gl_label_seen_t *self);

/**
 * @brief Store the length of the text last shown (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The seen; checked by ::lh_assert_runtime_ref.
 * @param length The new value.
 */
lh_void
lh_gl_label_seen_set_length(lh_gl_label_seen_t *self, lh_u32_t length);

/**
 * @brief Whether the hash and length are set (level 1).
 *
 * @param self The seen; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gl_label_seen_is_hashed(const lh_gl_label_seen_t *self);

/**
 * @brief Store whether the hash and length are set (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The seen; checked by ::lh_assert_runtime_ref.
 * @param hashed The new value.
 */
lh_void
lh_gl_label_seen_set_hashed(lh_gl_label_seen_t *self, lh_bool_t hashed);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_LABEL_SEEN_H */
