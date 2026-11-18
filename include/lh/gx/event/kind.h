/**
 * @file kind.h
 * @brief What happened: ::lh_gx_event_kind_t.
 */

#ifndef LH_GX_EVENT_KIND_H
#define LH_GX_EVENT_KIND_H

/**
 * @enum lh_gx_event_kind
 * @brief What happened — to the pointer, to the keyboard, or to the focus.
 *
 * The first four are the pointer's — a finger, a mouse button, a stylus. A press
 * starts a gesture and a release or a cancel ends it; between them the pointer
 * may move. The whole gesture goes to the entity the press landed on
 * (::lh_gx_pointer_t), wherever the pointer goes afterwards; they reach a
 * class's `input` slot.
 *
 * The other four are the keyboard's and the focus's: they go to the entity that
 * has the focus (::lh_gx_focus_t), through its class's `key` slot.
 */
enum lh_gx_event_kind
{
    /** The pointer went down on the entity. */
    lh_gx_event_kind_press,
    /** The pointer moved while it was down; it may be outside the entity now. */
    lh_gx_event_kind_move,
    /** The pointer went up; it may be outside the entity. */
    lh_gx_event_kind_release,
    /** The gesture is over without a release: the entity must forget it was
     *  pressed and do nothing. The point means nothing. */
    lh_gx_event_kind_cancel,
    /** A key that does not type was pressed (::lh_gx_key_t). The point means
     *  nothing. */
    lh_gx_event_kind_key,
    /** A character was typed: its Unicode code point. The point means nothing. */
    lh_gx_event_kind_text,
    /** The entity was given the focus: the keys come to it from now on. */
    lh_gx_event_kind_focus,
    /** The entity lost the focus: no key comes to it any more. */
    lh_gx_event_kind_blur
};

/** @brief What happened. */
typedef enum lh_gx_event_kind lh_gx_event_kind_t;

#endif /* LH_GX_EVENT_KIND_H */
