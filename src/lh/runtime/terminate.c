#include <lh/runtime/terminate.h>
#include <lh/atomic/fn_ptr.h>
#include <lh/atomic/fn_ptr/initializer.h>
#include <lh/attribute/thread_local.h>

/* The process handler is published with release and read with acquire, so a
   thread taking its copy while the host installs another is no data race. */
#if (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include <stdlib.h>
lh_atomic_fn_ptr_t m_runtime_terminate_process = lh_atomic_fn_ptr_initializer(abort);
#elif (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_OFF)
lh_atomic_fn_ptr_t m_runtime_terminate_process = lh_atomic_fn_ptr_initializer(lh_null);
#endif

/* Each thread's handler starts as a copy of the process handler, taken on the
   thread's first use (as the runtime allocator does). */
LH_ATTRIBUTE_THREAD_LOCAL
lh_runtime_terminate_cb m_runtime_terminate = lh_null;

LH_ATTRIBUTE_THREAD_LOCAL
lh_bool_t m_runtime_terminate_is_seeded = lh_bool_false;
lh_runtime_terminate_cb
lh_runtime_terminate_get_process(void)
{
    return lh_atomic_fn_ptr_get_acquire(lh_addr_of(m_runtime_terminate_process));
}

lh_void
lh_runtime_terminate_set_process(lh_runtime_terminate_cb fn)
{
    lh_atomic_fn_ptr_set_release(lh_addr_of(m_runtime_terminate_process), fn);
}

lh_void
lh_runtime_terminate_set(lh_runtime_terminate_cb fn)
{
    m_runtime_terminate = fn;
    m_runtime_terminate_is_seeded = lh_bool_true;
}

lh_runtime_terminate_cb
lh_runtime_terminate_get(void)
{
    if (!m_runtime_terminate_is_seeded)
    {
        m_runtime_terminate = lh_runtime_terminate_get_process();
        m_runtime_terminate_is_seeded = lh_bool_true;
    }
    return m_runtime_terminate;
}


lh_void
lh_runtime_terminate(void)
{
    /* Freestanding builds leave the slot null (USE_STDLIB=OFF). Calling it is a
       hard fault; spin instead so the host can still inspect the core. */
    lh_runtime_terminate_cb fn = lh_runtime_terminate_get();
    if (fn != lh_null)
    {
        fn();
    }
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
