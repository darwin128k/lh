/**
 * @file pointer.h
 * @brief How many pointers (mouse, touch contacts) are told apart:
 *        ::LH_OS_INPUT_POINTER_MAX.
 */

#ifndef LH_OS_INPUT_POINTER_H
#define LH_OS_INPUT_POINTER_H

#ifndef LH_OS_INPUT_POINTER_MAX
/**
 * @def LH_OS_INPUT_POINTER_MAX
 * @brief Pointer ids `0 .. LH_OS_INPUT_POINTER_MAX - 1` get gestures; a higher
 *        id is passed on without any. Define it before the include (or on the
 *        command line) to change it.
 */
#    define LH_OS_INPUT_POINTER_MAX 5
#endif

#endif /* LH_OS_INPUT_POINTER_H */
