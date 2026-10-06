/**
 * @file style.c
 * @brief Implementation of `lh/ui/style.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>

lh_ui_style_t
lh_ui_style_make_empty(void)
{
    lh_ui_style_t style;
    lh_ui_style_init(lh_addr_of(style));
    return style;
}

lh_void
lh_ui_style_init(lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    self->_reserved = 0;
}
