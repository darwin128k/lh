/**
 * @file style.c
 * @brief Implementation of `lh/ui/style.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>

lh_void
lh_ui_style_init(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    self->_reserved = 0;
}
