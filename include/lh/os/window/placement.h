/**
 * @file placement.h
 * @brief Where a window opens on the screen: ::lh_os_window_placement_t.
 *
 * Not hardcoded here: ::lh_os_window_set_placement says, and the window system
 * picks when it is left at ::lh_os_window_placement_default.
 */

#ifndef LH_OS_WINDOW_PLACEMENT_H
#define LH_OS_WINDOW_PLACEMENT_H

/**
 * @enum lh_os_window_placement
 * @brief Where ::lh_os_window_open puts a window for the first time.
 */
typedef enum lh_os_window_placement
{
    /**
     * Let the window system decide. On Win32 that is `CW_USEDEFAULT`, which puts
     * the first top-level window against the corner of the screen and every later
     * one relative to the window this app opened before it — a cascade, and
     * nothing like "in the middle of what the user is looking at".
     */
    lh_os_window_placement_default = 0,

    /**
     * Centred on the work area of the monitor it opens on: the area the system
     * considers usable, so a taskbar or a docked app bar is not in the middle of
     * the window.
     *
     * A modal child centres on the monitor its owner is on, because a dialog that
     * lands on the other screen is not near the thing it belongs to.
     *
     * A window larger than that work area keeps its top-left corner inside it, so
     * the title stays reachable; it is not resized, the size asked for is the size
     * asked for.
     */
    lh_os_window_placement_center = 1
} lh_os_window_placement_t;

#endif /* LH_OS_WINDOW_PLACEMENT_H */