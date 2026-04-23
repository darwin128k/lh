#include <lh/exception.h>
#include <lh/util/ptr.h>

lh_error_t *lh_exception_get_error(const lh_exception_t *self) {
    return lh_ptr_cast(lh_error_t, self);
}