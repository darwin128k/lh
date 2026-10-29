#include <lh/runtime/terminate.h>
#include <lh/attribute/thread_local.h>

#if (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include <stdlib.h>
LH_ATTRIBUTE_THREAD_LOCAL
lh_runtime_terminate_cb m_runtime_terminate = abort;
#elif (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_OFF)
LH_ATTRIBUTE_THREAD_LOCAL
lh_runtime_terminate_cb m_runtime_terminate = lh_null;
#endif

lh_runtime_terminate_cb
lh_runtime_terminate_get(void)
{
    return m_runtime_terminate;
}

lh_void
lh_runtime_terminate_set(lh_runtime_terminate_cb fn)
{
    m_runtime_terminate = fn;
}

lh_void
lh_runtime_terminate(void)
{
    /* Freestanding builds leave the slot null (USE_STDLIB=OFF). Calling it is a
       hard fault; spin instead so the host can still inspect the core. */
    if (m_runtime_terminate != lh_null)
    {
        m_runtime_terminate();
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
