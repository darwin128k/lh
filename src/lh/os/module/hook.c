/**
 * @file hook.c
 * @brief ::lh_os_module_hook_t: a bounded chain of handlers with a time budget.
 */

#include <lh/os/module/hook.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>
#include <lh/os/tick.h>

const lh_os_module_hook_handler_t *
lh_os_module_hook_get_handler(const lh_os_module_hook_t *self, lh_usize_t index)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handlers[index]);
}

lh_os_module_hook_handler_t *
lh_os_module_hook_get_handler_mut(lh_os_module_hook_t *self, lh_usize_t index)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->handlers[index]);
}

lh_usize_t
lh_os_module_hook_get_count(const lh_os_module_hook_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_os_module_hook_set_count(lh_os_module_hook_t *self, lh_usize_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_void
lh_os_module_hook_init(lh_os_module_hook_t *self)
{
    lh_os_module_hook_set_count(self, 0U);
}

lh_void
lh_os_module_hook_erase(lh_os_module_hook_t *self, lh_usize_t index)
{
    const lh_usize_t count = lh_math_sub_one(lh_os_module_hook_get_count(self));

    for (lh_usize_t i = index; lh_math_lt(i, count); ++i)
    {
        lh_ptr_deref(lh_os_module_hook_get_handler_mut(self, i)) =
            lh_ptr_deref(lh_os_module_hook_get_handler(self, lh_math_add_one(i)));
    }
    lh_os_module_hook_set_count(self, count);
}

lh_bool_t
lh_os_module_hook_try_add(lh_os_module_hook_t *self, lh_ptr owner, const lh_char_t *tag,
                          lh_os_module_hook_cb fn, lh_ptr context)
{
    const lh_usize_t count = lh_os_module_hook_get_count(self);

    if (lh_null_eq(fn) || lh_math_ge(count, LH_OS_MODULE_HOOK_SIZE))
    {
        return lh_bool_false;
    }

    lh_os_module_hook_handler_t *handler = lh_os_module_hook_get_handler_mut(self, count);

    lh_os_module_hook_handler_set_fn(handler, fn);
    lh_os_module_hook_handler_set_context(handler, context);
    lh_os_module_hook_handler_set_owner(handler, owner);
    lh_os_module_hook_handler_set_tag(handler, tag);
    lh_os_module_hook_set_count(self, lh_math_add_one(count));
    return lh_bool_true;
}

lh_bool_t
lh_os_module_hook_try_remove(lh_os_module_hook_t *self, lh_ptr owner, lh_os_module_hook_cb fn,
                             lh_ptr context)
{
    for (lh_usize_t i = 0U; lh_math_lt(i, lh_os_module_hook_get_count(self)); ++i)
    {
        if (lh_os_module_hook_handler_is(lh_os_module_hook_get_handler(self, i), owner, fn,
                                         context))
        {
            lh_os_module_hook_erase(self, i);
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_void
lh_os_module_hook_remove_owner(lh_os_module_hook_t *self, lh_ptr owner)
{
    lh_usize_t i = 0U;

    while (lh_math_lt(i, lh_os_module_hook_get_count(self)))
    {
        if (lh_math_eq(lh_os_module_hook_handler_get_owner(lh_os_module_hook_get_handler(self, i)),
                       owner))
        {
            lh_os_module_hook_erase(self, i);
        }
        else
        {
            ++i;
        }
    }
}

lh_bool_t
lh_os_module_hook_call(lh_os_module_hook_t *self, lh_ptr call, lh_u64_t budget_us,
                       const lh_char_t **stopped_by, lh_bool_t *overran)
{
    lh_bool_t passed = lh_bool_true;
    const lh_char_t *stopper = lh_null;
    lh_bool_t dropped = lh_bool_false;

    for (lh_usize_t i = lh_os_module_hook_get_count(self); lh_math_gt(i, 0U); --i)
    {
        /* Copied: a handler that overran is erased before its tag would be read. */
        const lh_os_module_hook_handler_t handler =
            lh_ptr_deref(lh_os_module_hook_get_handler(self, lh_math_sub_one(i)));
        /* The tick is 64-bit: a 32-bit counter would wrap and every handler would
           look like it overran its budget. */
        const lh_u64_t started = lh_os_tick_us();
        const lh_bool_t keep = lh_os_module_hook_handler_get_fn(lh_addr_of(handler))(
            lh_os_module_hook_handler_get_context(lh_addr_of(handler)), call);

        if (!lh_math_is_zero(budget_us) &&
            lh_math_gt(lh_math_sub(lh_os_tick_us(), started), budget_us))
        {
            dropped = lh_bool_true;
            lh_os_module_hook_erase(self, lh_math_sub_one(i));
            continue;
        }
        if (!keep)
        {
            passed = lh_bool_false;
            stopper = lh_os_module_hook_handler_get_tag(lh_addr_of(handler));
            break;
        }
    }
    if (lh_null_ne(stopped_by))
    {
        lh_ptr_deref(stopped_by) = stopper;
    }
    if (lh_null_ne(overran))
    {
        lh_ptr_deref(overran) = dropped;
    }
    return passed;
}
