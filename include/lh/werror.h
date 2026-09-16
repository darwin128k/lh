/**
 * @file werror.h
 * @brief Wide error value type (::lh_werror_t).
 *
 * Same shape as ::lh_error_t: ::lh_error_code_t plus optional
 * ::lh_werror_desc_t (::lh_wstr_view_t). Does not own text.
 */

#ifndef LH_WERROR_H
#define LH_WERROR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/error/code.h>
#include <lh/error/fields.h>
#include <lh/werror/desc.h>

/**
 * @struct lh_werror
 * @brief Error code and optional wide description.
 */
typedef struct lh_werror
{
    lh_error_fields(lh_error_code_t, lh_werror_desc_t);
} lh_werror_t; /**< Typedef for struct ::lh_werror. */

LH_COMPILER_EXTERN_C_BEGIN

LH_ATTRIBUTE_SYMBOL
void
lh_werror_pack(lh_werror_t *self, const lh_error_code_t *code, lh_werror_desc_t *desc);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_unpack(const lh_werror_t *self, lh_error_code_t *code, lh_werror_desc_t *desc);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_unpack_to_other(const lh_werror_t *self, lh_werror_t *other);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_set(lh_werror_t *self, lh_error_code_t code, lh_werror_desc_t desc);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_set_code(lh_werror_t *self, lh_error_code_t code);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_set_desc(lh_werror_t *self, lh_werror_desc_t desc);

LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_werror_get_code(const lh_werror_t *self);

LH_ATTRIBUTE_SYMBOL
lh_werror_desc_t
lh_werror_get_desc(const lh_werror_t *self);

LH_ATTRIBUTE_SYMBOL
lh_werror_desc_t
lh_werror_get_desc_or(const lh_werror_t *self, lh_werror_desc_t fallback);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_has_code(const lh_werror_t *self, lh_error_code_t code);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_is_ok(const lh_werror_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_is_failure(const lh_werror_t *self);

/**
 * @brief True when the description view is non-empty.
 *
 * Empty text and an uninitialized view both mean “no description”.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_has_desc(const lh_werror_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_is_empty(const lh_werror_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_equals(const lh_werror_t *self, const lh_werror_t *other);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_has_same_code(const lh_werror_t *self, const lh_werror_t *other);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_werror_has_diff_code(const lh_werror_t *self, const lh_werror_t *other);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_assign(lh_werror_t *self, const lh_werror_t *other);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_clear(lh_werror_t *self);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_init(lh_werror_t *self, lh_error_code_t code, lh_werror_desc_t desc);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_init_by_other(lh_werror_t *self, const lh_werror_t *other);

LH_ATTRIBUTE_SYMBOL
void
lh_werror_init_by_empty(lh_werror_t *self);

LH_ATTRIBUTE_SYMBOL
lh_error_code_t
lh_werror_get_code_and_clear(lh_werror_t *self);

LH_ATTRIBUTE_SYMBOL
lh_werror_t
lh_werror_make(lh_error_code_t code, lh_werror_desc_t desc);

LH_ATTRIBUTE_SYMBOL
lh_werror_t
lh_werror_make_by_code(lh_error_code_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WERROR_H */
