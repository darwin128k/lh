/**
 * @file event.h
 * @brief One thing that happened to an entity: ::lh_gx_event_t.
 *
 * An event is what an entity's class hears. Through its `input` slot
 * (::lh_gx_class_input_fn): a press, a move, a release or a cancel of the
 * pointer, and the point on the screen where it happened. Through its `key`
 * slot (::lh_gx_class_key_fn): a key (::lh_gx_key_t), a typed character (its
 * code point), and the focus coming or going. It is a value: whoever delivers
 * it builds it on its stack for one call and nothing keeps it.
 *
 * Where events come from is not gx's business: a window, a touch controller on
 * a board, a test, a browser — each turns its own report into events, and the
 * pointer (::lh_gx_pointer_t) and the focus (::lh_gx_focus_t) deliver them.
 */

#ifndef LH_GX_EVENT_H
#define LH_GX_EVENT_H

#include <lh/gx/event/fields.h>
#include <lh/gx/event/key.h>
#include <lh/gx/event/kind.h>
#include <lh/gx/point.h>

/**
 * @struct lh_gx_event
 * @brief What happened, and where or with what.
 */
struct lh_gx_event
{
    /** Members: ::lh_gx_event_fields. */
    lh_gx_event_fields(lh_gx_event_kind_t, lh_gx_point_t, lh_gx_key_t, lh_u32_t);
};
/** @copybrief lh_gx_event */
typedef struct lh_gx_event lh_gx_event_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What happened (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 */
lh_gx_event_kind_t
lh_gx_event_get_kind(const lh_gx_event_t *self);

/**
 * @brief Store what happened (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param kind What happened.
 */
lh_void
lh_gx_event_set_kind(lh_gx_event_t *self, lh_gx_event_kind_t kind);

/**
 * @brief Where it happened, screen coordinates (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @return The point, inside @p self.
 */
const lh_gx_point_t *
lh_gx_event_get_at(const lh_gx_event_t *self);

/**
 * @brief Store where it happened (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param at   The point, screen coordinates.
 */
lh_void
lh_gx_event_set_at(lh_gx_event_t *self, lh_gx_point_t at);

/**
 * @brief The key of a key event (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @return The key; ::lh_gx_key_none for every other kind.
 */
lh_gx_key_t
lh_gx_event_get_key(const lh_gx_event_t *self);

/**
 * @brief Store the key (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param key  The key.
 */
lh_void
lh_gx_event_set_key(lh_gx_event_t *self, lh_gx_key_t key);

/**
 * @brief The code point of a text event (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @return The Unicode code point; `0` for every other kind.
 */
lh_u32_t
lh_gx_event_get_code(const lh_gx_event_t *self);

/**
 * @brief Store the code point (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param code The Unicode code point.
 */
lh_void
lh_gx_event_set_code(lh_gx_event_t *self, lh_u32_t code);

/**
 * @brief Whether @p code is a character a text event may carry: one that types —
 *        not a control character (below U+0020, or U+007F) and not half of a
 *        surrogate pair — up to U+10FFFF.
 *
 * A source that reports Enter or Backspace as a character as well as a key (a
 * Windows `WM_CHAR` does) drops it here: those are key events.
 *
 * @param code A Unicode code point.
 */
lh_bool_t
lh_gx_event_is_text_code(lh_u32_t code);

/**
 * @brief Whether the event is the pointer's — a press, a move, a release or a
 *        cancel — and so goes to a class's `input` slot.
 *
 * @param self The event; must not be null.
 * @return ::lh_bool_false for a key, a text, a focus or a blur.
 */
lh_bool_t
lh_gx_event_is_pointer(const lh_gx_event_t *self);

/**
 * @brief An event of @p kind at @p at, with no key and no code: the pointer's.
 *
 * @param self The event; must not be null.
 * @param kind What happened.
 * @param at   Where, screen coordinates.
 */
lh_void
lh_gx_event_init(lh_gx_event_t *self, lh_gx_event_kind_t kind, lh_gx_point_t at);

/**
 * @brief An event of @p kind with no point, no key and no code: a cancel, a
 *        focus or a blur.
 *
 * @param self The event; must not be null.
 * @param kind What happened.
 */
lh_void
lh_gx_event_init_kind(lh_gx_event_t *self, lh_gx_event_kind_t kind);

/**
 * @brief A key event: @p key was pressed.
 *
 * @param self The event; must not be null.
 * @param key  The key.
 */
lh_void
lh_gx_event_init_key(lh_gx_event_t *self, lh_gx_key_t key);

/**
 * @brief A text event: the character @p code was typed.
 *
 * @param self The event; must not be null.
 * @param code Its Unicode code point.
 */
lh_void
lh_gx_event_init_text(lh_gx_event_t *self, lh_u32_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_EVENT_H */
