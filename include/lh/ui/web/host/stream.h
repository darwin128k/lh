/**
 * @file stream.h
 * @brief ::lh_ui_web_host_stream_t — what the stream of WS frames shares between iterations.
 */

#ifndef LH_UI_WEB_HOST_STREAM_H
#define LH_UI_WEB_HOST_STREAM_H

#include <lh/config.h>
#include <lh/ui/web/host/client.h>

/**
 * @struct lh_ui_web_host_stream
 * @brief What the stream of WS frames shares between iterations.
 */
struct lh_ui_web_host_stream
{
    /** Members: ::lh_ui_web_host_stream_fields. */
    lh_ui_web_host_stream_fields(lh_ui_canvas_web_glyphs_t, lh_str_ptr, lh_usize_t);
};
/** @copybrief lh_ui_web_host_stream */
typedef struct lh_ui_web_host_stream lh_ui_web_host_stream_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The glyph masks the stream has sent (level 1).
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_canvas_web_glyphs_t *
lh_ui_web_host_stream_get_glyphs(const lh_ui_web_host_stream_t *self);

/**
 * @brief The glyph masks the stream has sent, to change in place (level 1).
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_canvas_web_glyphs_t *
lh_ui_web_host_stream_get_glyphs_mut(lh_ui_web_host_stream_t *self);

/**
 * @brief Store the glyph masks the stream has sent (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 * @param glyphs The new value.
 */
lh_void
lh_ui_web_host_stream_set_glyphs(lh_ui_web_host_stream_t *self, lh_ui_canvas_web_glyphs_t glyphs);

/**
 * @brief The last message sent, kept to skip repeats (level 1).
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 */
lh_str_ptr
lh_ui_web_host_stream_get_last(const lh_ui_web_host_stream_t *self);

/**
 * @brief Store the last message sent, kept to skip repeats (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 * @param last The new value.
 */
lh_void
lh_ui_web_host_stream_set_last(lh_ui_web_host_stream_t *self, lh_str_ptr last);

/**
 * @brief How long the last message is (level 1).
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_ui_web_host_stream_get_last_len(const lh_ui_web_host_stream_t *self);

/**
 * @brief Store how long the last message is (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 * @param last_len The new value.
 */
lh_void
lh_ui_web_host_stream_set_last_len(lh_ui_web_host_stream_t *self, lh_usize_t last_len);

/**
 * @brief How many bytes the last-message buffer holds (level 1).
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_ui_web_host_stream_get_last_cap(const lh_ui_web_host_stream_t *self);

/**
 * @brief Store how many bytes the last-message buffer holds (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The stream; checked by ::lh_assert_runtime_ref.
 * @param last_cap The new value.
 */
lh_void
lh_ui_web_host_stream_set_last_cap(lh_ui_web_host_stream_t *self, lh_usize_t last_cap);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_STREAM_H */
