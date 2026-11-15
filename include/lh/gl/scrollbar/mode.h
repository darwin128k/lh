/**
 * @file mode.h
 * @brief When a scrollbar is shown: ::lh_gl_scrollbar_mode_t.
 */

#ifndef LH_GL_SCROLLBAR_MODE_H
#define LH_GL_SCROLLBAR_MODE_H

/**
 * @enum lh_gl_scrollbar_mode
 * @brief The scrollbar's answer to ::lh_gl_entity_event_visible.
 *
 * The container scrolls in every mode; the mode only decides whether the
 * bar is drawn and takes clicks. The hidden flag of the entity wins over all.
 */
typedef enum lh_gl_scrollbar_mode
{
    lh_gl_scrollbar_mode_hidden = 0, /**< Never shown. */
    lh_gl_scrollbar_mode_auto = 1,   /**< Shown only when the container overflows on the axis. */
    lh_gl_scrollbar_mode_always = 2  /**< Always shown; with no overflow the thumb is the track. */
} lh_gl_scrollbar_mode_t;            /**< The scrollbar's answer to ::lh_gl_entity_event_visible. */

#endif /* LH_GL_SCROLLBAR_MODE_H */
