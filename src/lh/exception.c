#include <lh/exception.h>
#include <lh/assert.h>
#include <lh/util/addr.h>

const lh_runtime_error_t *
lh_exception_get_error(const lh_exception_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->error);
}

lh_runtime_error_code_t
lh_exception_get_code(const lh_exception_t *self)
{
    return lh_runtime_error_get_code(lh_exception_get_error(self));
}

const lh_runtime_check_site_t *
lh_exception_get_site(const lh_exception_t *self)
{
    lh_assert_runtime_ref(self);
    return self->site;
}
