#include <lh/runtime/terminate.h>
#include <lh/compiler/trap.h>
#include <lh/runtime/terminate/handler/initializer.h>

/* One object for the whole program, empty until the host fills it: the core
   has no C library and no defaults. The host fills it before the first use
   and before it starts other threads, so every thread sees the same one. */
lh_runtime_terminate_handler_t m_runtime_terminate =
    lh_runtime_terminate_handler_initializer(lh_null, lh_null);

lh_runtime_terminate_handler_t *
lh_runtime_terminate_get_mut(void)
{
    return lh_addr_of(m_runtime_terminate);
}

const lh_runtime_terminate_handler_t *
lh_runtime_terminate_get(void)
{
    return lh_addr_of(m_runtime_terminate);
}

lh_void
lh_runtime_terminate(void)
{
    /* An empty handler, or one that returns, ends in a trap: no C library is
       needed for it, and a debugger stops right here. The loop only tells the
       compiler that nothing follows on a target whose trap may return. */
    lh_runtime_terminate_handler_call(lh_runtime_terminate_get());
    lh_compiler_trap();
    for (;;)
    {
    }
}

lh_void
lh_runtime_terminate_unless(lh_bool_t ok)
{
    if (!ok)
    {
        lh_runtime_terminate();
    }
}
