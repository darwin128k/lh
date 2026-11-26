/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_decl_fault_t.
 */

#ifndef LH_GL_DECL_FAULT_FIELDS_H
#define LH_GL_DECL_FAULT_FIELDS_H

/**
 * @def lh_gl_decl_fault_fields(error_type)
 * @brief Which row a build refused, and why: `code` (::lh_gl_decl_error_none when
 *        the build worked) and `index`, the row that was refused, or the last row
 *        for a fault of the whole table.
 *
 * @param error_type ::lh_gl_decl_error_t.
 */
#define lh_gl_decl_fault_fields(error_type)                                                        \
    error_type code;                                                                               \
    lh_u32_t index

#endif /* LH_GL_DECL_FAULT_FIELDS_H */
