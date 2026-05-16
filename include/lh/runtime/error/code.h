/**
 * @file code.h
 * @brief Runtime-layer codes on top of ::lh_error_code_t.
 */

#ifndef LH_RUNTIME_ERROR_CODE_H
#define LH_RUNTIME_ERROR_CODE_H

#include <lh/error/code.h>

/**
 * @def lh_runtime_error_code_ok
 * @brief Runtime alias
 *        for the non-error / success sentinel (::LH_ERROR_CODE_OK).
 *
 * @see LH_ERROR_CODE_OK
 */
#define lh_runtime_error_code_ok lh_error_code_ok

/**
 * @def lh_runtime_error_code_interrupt
 * @brief Runtime error code for user- or library-requested interruption.
 *
 * Used for paths that stop normal control flow with a generic interrupt kind
 * (e.g. ::lh_runtime_throw_by_desc pairs this code with a message).
 */
#define lh_runtime_error_code_interrupt 0x01

/**
 * @def lh_runtime_error_code_null_pointer
 * @brief Runtime error code for failed null-pointer / reference checks.
 *
 * Used when ::lh_runtime_check_ref (or similar) detects an invalid pointer.
 */
#define lh_runtime_error_code_null_pointer 0x02

/**
 * @def lh_runtime_error_code_memory_not_allocated
 * @brief Allocation callback returned null for a requested memory block.
 *
 * Emitted by allocator helpers when memory acquisition fails.
 */
#define lh_runtime_error_code_memory_not_allocated 0x03

/**
 * @def lh_runtime_error_code_allocator_function_not_initialized
 * @brief Allocator callback was not set before allocation was requested.
 *
 * Indicates that ::lh_memory_allocator_alloc
 * was called without a configured alloc function pointer.
 */
#define lh_runtime_error_code_allocator_function_not_initialized 0x04

/**
 * @def lh_runtime_error_code_deallocator_function_not_initialized
 * @brief Deallocator callback was not set before free was requested.
 *
 * Indicates that ::lh_memory_allocator_dealloc was called
 * without a configured dealloc function pointer.
 */
#define lh_runtime_error_code_deallocator_function_not_initialized 0x05

/**
 * @def lh_runtime_error_code_invalid_range
 * @brief Runtime error code for invalid range state.
 *
 * Used when range validation fails (for example in
 * ::lh_memory_bounds_unpack_v before diff/size calculations).
 */
#define lh_runtime_error_code_invalid_range 0x06

/**
 * @def lh_runtime_error_code_not_power_of_two
 * @brief Runtime error code for non power-of-two alignment argument.
 *
 * Emitted by APIs that require power-of-two alignment values.
 */
#define lh_runtime_error_code_not_power_of_two 0x07

/**
 * @def lh_runtime_error_code_out_of_range
 * @brief Runtime error code for out-of-range access.
 *
 * Emitted when a pointer offset is invalid for the range size.
 */
#define lh_runtime_error_code_out_of_range 0x08

/**
 * @def lh_runtime_error_code_invalid_argument
 * @brief Runtime error code for invalid argument.
 *
 * Emitted when an invalid argument is passed to a function.
 */
#define lh_runtime_error_code_invalid_argument 0x09

/**
 * @def lh_runtime_error_code_null_dereference
 * @brief Runtime error code for explicit null-pointer dereference attempts.
 *
 * Use when a pointer is about to be dereferenced and was found null
 * (distinct from generic pointer/reference validation checks).
 */
#define lh_runtime_error_code_null_dereference 0x0A

/**
 * @def lh_runtime_error_code_no_null_terminator
 * @brief Runtime error code when a C string has no NUL within the scanned range.
 *
 * Emitted when a bounded search (for example over address space up to ::LH_USIZE_T_MAX)
 * fails to find a null terminator before the scan limit.
 */
#define lh_runtime_error_code_no_null_terminator 0x0B

/**
 * @def lh_runtime_error_code_size_not_multiple_of_type_size
 * @brief Runtime error code when range size is not a multiple of typed element size.
 *
 * Emitted when calculating element count in a typed memory span
 * (for example in ::lh_memory_typed_get_size) fails because
 * the underlying range size is not evenly divisible by the element type size.
 */
#define lh_runtime_error_code_size_not_multiple_of_type_size 0x0C

/**
 * @def lh_runtime_error_code_overflow
 * @brief Runtime error code for arithmetic overflow.
 *
 * Emitted when a runtime calculation would exceed the maximum representable
 * value for its type (for example during size or offset arithmetic).
 */
#define lh_runtime_error_code_overflow 0x0D

/**
 * @def lh_runtime_error_code_underflow
 * @brief Runtime error code for arithmetic underflow.
 *
 * Emitted when a runtime calculation would go below the minimum representable
 * value for its type (for example during backward offset arithmetic).
 */
#define lh_runtime_error_code_underflow 0x0E

/**
 * @def lh_runtime_error_code_division_by_zero
 * @brief Runtime error code for division by zero.
 *
 * Emitted when a runtime calculation attempts to divide by zero.
 */
#define lh_runtime_error_code_division_by_zero 0x0F

/**
 * @def lh_runtime_error_code_not_initialized
 * @brief Runtime error code for use of an uninitialized object.
 *
 * Emitted when an operation is performed on an object that has not yet been
 * initialized.
 */
#define lh_runtime_error_code_not_initialized 0x10

/**
 * @def lh_runtime_error_code_type_size_mismatch
 * @brief Runtime error code for incompatible element type sizes.
 *
 * Emitted when two typed memory objects are expected to share the same element
 * type size but do not (for example in ::lh_memory_typed_assign).
 */
#define lh_runtime_error_code_type_size_mismatch 0x11

/**
 * @typedef lh_runtime_error_code_t
 * @brief Same representation as ::lh_error_code_t for runtime-layer APIs.
 *
 * Distinct typedef documents intent (runtime throw / check paths) while keeping
 * binary compatibility with ::lh_error_code_t.
 */
typedef lh_error_code_t lh_runtime_error_code_t;

#endif // LH_RUNTIME_ERROR_CODE_H
