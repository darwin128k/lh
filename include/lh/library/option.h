/**
 * @file lh/library/option.h
 * @brief Library-level ON/OFF option constants.
 *
 * Provides ::LH_LIBRARY_OPTION_OFF and ::LH_LIBRARY_OPTION_ON —
 * library-scoped aliases for ::LH_OPTION_OFF and ::LH_OPTION_ON.
 * Both macros can be overridden by defining them before including this header.
 */

#ifndef LH_LIBRARY_OPTION_H
#define LH_LIBRARY_OPTION_H

#include <lh/option.h>

#ifndef LH_LIBRARY_OPTION_OFF
/**
 * @def LH_LIBRARY_OPTION_OFF
 * @brief Library-level "disabled" constant (0).
 *
 * Can be overridden by defining it before including this header.
 */
#    define LH_LIBRARY_OPTION_OFF LH_OPTION_OFF
#endif // LH_LIBRARY_OPTION_OFF

#ifndef LH_LIBRARY_OPTION_ON
/**
 * @def LH_LIBRARY_OPTION_ON
 * @brief Library-level "enabled" constant (1).
 *
 * Can be overridden by defining it before including this header.
 */
#    define LH_LIBRARY_OPTION_ON LH_OPTION_ON
#endif // LH_LIBRARY_OPTION_ON

#endif // LH_LIBRARY_OPTION_H