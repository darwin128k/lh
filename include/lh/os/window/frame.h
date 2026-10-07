/**
 * @file frame.h
 * @brief Who draws a window's frame: ::lh_os_window_frame_t.
 */

#ifndef LH_OS_WINDOW_FRAME_H
#define LH_OS_WINDOW_FRAME_H

/**
 * @enum lh_os_window_frame
 * @brief Whether the window system draws a window's frame or the app does.
 *
 * A system frame and a styled one are the same window otherwise: same events, same
 * client, same damage. What changes is creation time, so set it before opening.
 */
typedef enum lh_os_window_frame
{
    /** The ordinary OS frame, with its caption, menu and resize borders. The default. */
    lh_os_window_frame_system = 0,
    /**
     * No OS frame at all. The client is then the whole window, so the width and
     * height given to ::lh_os_window_open are the client size with nothing subtracted
     * for a frame, and there are no edges left to resize by — name resize zones
     * through ::lh_os_window_set_on_zone instead.
     */
    lh_os_window_frame_own = 1
} lh_os_window_frame_t;

#endif /* LH_OS_WINDOW_FRAME_H */