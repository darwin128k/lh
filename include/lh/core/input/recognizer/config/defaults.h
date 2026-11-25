/**
 * @file defaults.h
 * @brief The recognizer's thresholds that a system also keeps a setting for:
 *        ::LH_INPUT_RECOGNIZER_SLOP_DEFAULT,
 *        ::LH_INPUT_RECOGNIZER_DOUBLE_CLICK_MS_DEFAULT,
 *        ::LH_INPUT_RECOGNIZER_DOUBLE_CLICK_SLOP_DEFAULT.
 *
 * ::lh_input_recognizer_config_default_initializer starts from them, and a
 * system kernel that has nothing to read (`lh/os/system/input.h` on Linux and
 * macOS) answers them, so "the default" is one number wherever it is used. A
 * desktop's own setting replaces them through
 * `lh_os_input_read_recognizer_config` (`lh/os/input.h`).
 *
 * Override any of them with a compile definition (for example
 * `-DLH_INPUT_RECOGNIZER_DOUBLE_CLICK_MS_DEFAULT=400U`).
 */

#ifndef LH_INPUT_RECOGNIZER_CONFIG_DEFAULTS_H
#define LH_INPUT_RECOGNIZER_CONFIG_DEFAULTS_H

#ifndef LH_INPUT_RECOGNIZER_SLOP_DEFAULT
/** Pixels a pointer may move while down and still click: 4, Windows' own drag
    threshold (`SM_CXDRAG`). */
#    define LH_INPUT_RECOGNIZER_SLOP_DEFAULT 4U
#endif

#ifndef LH_INPUT_RECOGNIZER_DOUBLE_CLICK_MS_DEFAULT
/** Milliseconds within which a click continues a series: 500, Windows' own default
    (`GetDoubleClickTime`). */
#    define LH_INPUT_RECOGNIZER_DOUBLE_CLICK_MS_DEFAULT 500U
#endif

#ifndef LH_INPUT_RECOGNIZER_DOUBLE_CLICK_SLOP_DEFAULT
/** Pixels within which a click continues a series: 4. */
#    define LH_INPUT_RECOGNIZER_DOUBLE_CLICK_SLOP_DEFAULT 4U
#endif

#endif /* LH_INPUT_RECOGNIZER_CONFIG_DEFAULTS_H */
