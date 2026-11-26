/**
 * @file fault.h
 * @brief Which row a build refused, and why: ::lh_gl_decl_fault_t.
 *
 * A table is data, so a typo in one row has to say which row -- that is the whole
 * difference between a declaration and a call sequence. A build fills one when it
 * refuses (::lh_gl_decl_build), and the caller reads it back.
 */

#ifndef LH_GL_DECL_FAULT_H
#define LH_GL_DECL_FAULT_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>
#include <lh/core/void.h>
#include <lh/gl/decl/error.h>
#include <lh/gl/decl/fault/fields.h>

/**
 * @struct lh_gl_decl_fault
 * @brief Which row a build refused, and why.
 */
struct lh_gl_decl_fault
{
    /** Members: ::lh_gl_decl_fault_fields. */
    lh_gl_decl_fault_fields(lh_gl_decl_error_t);
};
/** @copybrief lh_gl_decl_fault */
typedef struct lh_gl_decl_fault lh_gl_decl_fault_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What went wrong (level 1).
 *
 * @param self The fault; checked by ::lh_assert_runtime_ref.
 */
lh_gl_decl_error_t
lh_gl_decl_fault_get_code(const lh_gl_decl_fault_t *self);

/**
 * @brief Store what went wrong (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The fault; checked by ::lh_assert_runtime_ref.
 * @param code The new value.
 */
lh_void
lh_gl_decl_fault_set_code(lh_gl_decl_fault_t *self, lh_gl_decl_error_t code);

/**
 * @brief The row it went wrong at (level 1).
 *
 * @param self The fault; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_gl_decl_fault_get_index(const lh_gl_decl_fault_t *self);

/**
 * @brief Store the row it went wrong at (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The fault; checked by ::lh_assert_runtime_ref.
 * @param index The new value.
 */
lh_void
lh_gl_decl_fault_set_index(lh_gl_decl_fault_t *self, lh_u32_t index);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_DECL_FAULT_H */
