/**
 * @file zone.h
 * @brief What a point of a window is to the OS: ::lh_os_window_zone_t.
 */

#ifndef LH_OS_WINDOW_ZONE_H
#define LH_OS_WINDOW_ZONE_H

/**
 * @enum lh_os_window_zone
 * @brief What part of the window a point is, as far as the window system is
 *        concerned.
 *
 * The answer is the app's, through ::lh_os_window_set_on_zone, because only the app
 * knows where its own chrome is: which strip moves the window, which corners resize
 * it, and which of its buttons are its own and must keep getting clicks. Nothing here
 * assumes a title bar at the top or a fixed border — a caller that answers
 * ::lh_os_window_zone_client everywhere gets the ordinary behaviour, and a caller that
 * answers ::lh_os_window_zone_caption over a strip it drew itself gets a window the
 * OS moves with its own move loop, cursors and all.
 *
 * `DefWindowProc` turns a press on a non-client zone into the matching system command
 * (`SC_MOVE`, `SC_SIZE`), so naming the zone here is the whole of moving and
 * resizing a frameless window; `lh` only has to translate the answer.
 */
typedef enum lh_os_window_zone
{
    /** The app's own content: ordinary press, move, release and wheel reach it. */
    lh_os_window_zone_client = 0,
    /** Drag the window, the way a caption does. */
    lh_os_window_zone_caption = 1,
    /** Resize by the left edge. */
    lh_os_window_zone_left = 2,
    /** Resize by the right edge. */
    lh_os_window_zone_right = 3,
    /** Resize by the top edge. */
    lh_os_window_zone_top = 4,
    /** Resize by the bottom edge. */
    lh_os_window_zone_bottom = 5,
    /** Resize by the top-left corner. */
    lh_os_window_zone_top_left = 6,
    /** Resize by the top-right corner. */
    lh_os_window_zone_top_right = 7,
    /** Resize by the bottom-left corner. */
    lh_os_window_zone_bottom_left = 8,
    /** Resize by the bottom-right corner. */
    lh_os_window_zone_bottom_right = 9
} lh_os_window_zone_t;

#endif /* LH_OS_WINDOW_ZONE_H */