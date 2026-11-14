/**
 * @file fields.h
 * @brief Owned parts and application update hook of ::lh_ui_window_t.
 *
 * The view borrows its entity tree. The other members are owned by the window
 * composition and live until it is deinitialized.
 */

#ifndef LH_UI_WINDOW_FIELDS_H
#define LH_UI_WINDOW_FIELDS_H

/**
 * @def lh_ui_window_fields
 * @brief Native window, software screen, canvas, view, controller and update hook.
 * @param window_type Native window type.
 * @param screen_type Software screen context type.
 * @param canvas_type Canvas type.
 * @param view_type Entity-tree view type.
 * @param controller_type Input controller type.
 * @param update_type Application update callback pointer type.
 * @param context_type Callback context pointer type.
 */
#define lh_ui_window_fields(window_type, screen_type, canvas_type, view_type, controller_type,     \
                            update_type, context_type)                                             \
    window_type window;                                                                            \
    screen_type screen;                                                                            \
    canvas_type canvas;                                                                            \
    view_type view;                                                                                \
    controller_type controller;                                                                    \
    update_type on_update_fn;                                                                      \
    context_type on_update_context

#endif /* LH_UI_WINDOW_FIELDS_H */
