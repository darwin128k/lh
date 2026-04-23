/**
 * @file option.h
 * @brief Simple and portable ON/OFF option/state macros
 *
 * This header defines two symbolic constants commonly used to represent
 * binary states (enabled/disabled, on/off, true/false, active/inactive)
 * in a clear, self-documenting way.
 *
 * These macros are especially useful in:
 * - configuration / feature toggles
 * - preprocessor-controlled conditional compilation
 * - function parameters that accept enable/disable flags
 * - structure fields representing binary options
 *
 * Using LH_OPTION_ON / LH_OPTION_OFF instead of raw 1/0
 * improves readability and reduces the chance of semantic mistakes.
 */

#ifndef LH_OPTION_H
#define LH_OPTION_H

#ifndef LH_OPTION_OFF
/**
 * @def LH_OPTION_OFF
 * @brief Symbolic constant representing "disabled",
 *        "off", "false", "inactive"
 *
 * Default value: 0
 *
 * This macro is guaranteed to be defined to 0 unless
 * explicitly overridden before including this header.
 *
 * Typical usage:
 * @code
 * #define FEATURE_LOGGING   LH_OPTION_OFF
 * #define USE_COMPRESSION   LH_OPTION_ON
 * @endcode
 *
 * @see LH_OPTION_ON
 */
#    define LH_OPTION_OFF 0
#endif // LH_OPTION_OFF

#ifndef LH_OPTION_ON
/**
 * @def LH_OPTION_ON
 * @brief Symbolic constant representing "enabled",
 *        "on", "true", "active"
 *
 * Default value: 1
 *
 * This macro is guaranteed to be defined to 1 unless
 * explicitly overridden before including this header.
 *
 * Typical usage:
 * @code
 * #if LH_OPTION_DEBUG_MODE == LH_OPTION_ON
 *     #define DEBUG_PRINT(...)  fprintf(stderr, __VA_ARGS__)
 * #else
 *     #define DEBUG_PRINT(...)  ((void)0)
 * #endif
 * @endcode
 *
 * @see LH_OPTION_OFF
 */
#    define LH_OPTION_ON 1
#endif // LH_OPTION_ON

#endif // LH_OPTION_H