/**
 * @file kind.h
 * @brief What happened to the pointer: ::lh_gx_event_kind_t.
 */

#ifndef LH_GX_EVENT_KIND_H
#define LH_GX_EVENT_KIND_H

/**
 * @enum lh_gx_event_kind
 * @brief What happened to the pointer — a finger, a mouse button, a stylus.
 *
 * A press starts a gesture and a release or a cancel ends it; between them the
 * pointer may move. The whole gesture goes to the entity the press landed on
 * (::lh_gx_pointer_t), wherever the pointer goes afterwards.
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
    lh_gx_event_kind_cancel
};

/** @brief What happened to the pointer. */
typedef enum lh_gx_event_kind lh_gx_event_kind_t;

#endif /* LH_GX_EVENT_KIND_H */
