/**
 * @file kind.h
 * @brief What sort of device an event came from: ::lh_os_input_device_kind_t.
 */

#ifndef LH_OS_INPUT_DEVICE_KIND_H
#define LH_OS_INPUT_DEVICE_KIND_H

/**
 * @enum lh_os_input_device_kind
 * @brief The sort of device behind one ::lh_os_input_event_t.
 */
typedef enum lh_os_input_device_kind
{
    lh_os_input_device_kind_none = 0,  /**< Not a device (a focus change). */
    lh_os_input_device_kind_mouse,     /**< A mouse or a touchpad. */
    lh_os_input_device_kind_touch,     /**< A touch screen, one contact per pointer. */
    lh_os_input_device_kind_pen,       /**< A pen or a stylus. */
    lh_os_input_device_kind_keyboard,  /**< A keyboard. */
    lh_os_input_device_kind_controller /**< A game pad, a joystick, GPIO buttons. */
} lh_os_input_device_kind_t;           /**< The sort of device behind an event. */

#endif /* LH_OS_INPUT_DEVICE_KIND_H */
