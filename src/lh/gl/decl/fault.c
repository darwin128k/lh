/**
 * @file fault.c
 * @brief Implementation of `lh/gl/decl/fault.h`.
 */

#include <lh/gl/decl/fault.h>

#include <lh/core/assert.h>

lh_gl_decl_error_t
lh_gl_decl_fault_get_code(const lh_gl_decl_fault_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_void
lh_gl_decl_fault_set_code(lh_gl_decl_fault_t *self, lh_gl_decl_error_t code)
{
    lh_assert_runtime_ref(self);
    self->code = code;
}

lh_u32_t
lh_gl_decl_fault_get_index(const lh_gl_decl_fault_t *self)
{
    lh_assert_runtime_ref(self);
    return self->index;
}

lh_void
lh_gl_decl_fault_set_index(lh_gl_decl_fault_t *self, lh_u32_t index)
{
    lh_assert_runtime_ref(self);
    self->index = index;
}
