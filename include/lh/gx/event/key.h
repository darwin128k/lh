/**
 * @file key.h
 * @brief The keys a control acts on: ::lh_gx_key_t.
 */

#ifndef LH_GX_EVENT_KEY_H
#define LH_GX_EVENT_KEY_H

/**
 * @enum lh_gx_key
 * @brief A key that moves, edits or leaves — not one that types.
 *
 * A key that types a character comes as a text event with the character
 * (::lh_gx_event_kind_text), so a keyboard layout, a compose key or an input
 * method is the source's affair. These are the keys that do something else.
 * A source that has a key not listed here does not send it.
 */
enum lh_gx_key
{
    /** No key: the event is not a key event. */
    lh_gx_key_none,
    /** Left arrow. */
    lh_gx_key_left,
    /** Right arrow. */
    lh_gx_key_right,
    /** Up arrow. */
    lh_gx_key_up,
    /** Down arrow. */
    lh_gx_key_down,
    /** Home: the start of the line. */
    lh_gx_key_home,
    /** End: the end of the line. */
    lh_gx_key_end,
    /** Backspace: delete before the cursor. */
    lh_gx_key_backspace,
    /** Delete: delete after the cursor. */
    lh_gx_key_delete,
    /** Enter: confirm. */
    lh_gx_key_enter,
    /** Escape: give up. */
    lh_gx_key_escape,
    /** Tab: on to the next. */
    lh_gx_key_tab
};
/** @copybrief lh_gx_key */
typedef enum lh_gx_key lh_gx_key_t;

#endif /* LH_GX_EVENT_KEY_H */
