/**
 * @file return.h
 * @brief Header file for the `lh_runtime_return` macro.
 *
 * This file contains the definition of the `lh_runtime_return` macro,
 * which enables immediate termination of function execution with a specified return value.
 * The macro is used to implement early exits from functions, e.g., in error handling scenarios.
 *
 * @details The `lh_runtime_return` macro serves as a convenient tool for scenarios
 *          requiring abrupt termination of function execution at any point while returning
 *          a value, eliminating the need for additional exit control flow statements.
 *
 * @note Use of this macro should be limited to maintain code readability and avoid
 *       excessive complexity. It is particularly useful for implementing early function
 *       returns or error condition handling.
 */

#ifndef LH_RUNTIME_RETURN_H
#define LH_RUNTIME_RETURN_H

/**
 * @def lh_runtime_return
 * @brief Terminates execution with a specified return value.
 *
 * This macro performs an immediate exit from the current function while returning
 * the specified value. It can be used to abruptly terminate function execution
 * and return a result from any point in the code.
 *
 * @param ... Value to be returned from the function.
 * @return Returns the value passed as the parameter.
 *
 * @note This macro should be reserved for scenarios requiring immediate function
 *       termination with a return value. It is particularly useful for error
 *       handling or implementing early exits from nested control structures.
 */
#define lh_runtime_return(...) return __VA_ARGS__

#endif // LH_RUNTIME_RETURN_H