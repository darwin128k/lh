#include <lh/runtime/terminate.h>

#if (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include <stdlib.h>
    LH_ATTRIBUTE_THREAD_LOCAL
lh_runtime_terminate_cb m_runtime_terminate = abort;
#elif (LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB == LH_LIBRARY_OPTION_OFF)
#    include <lh/null.h>
    LH_ATTRIBUTE_THREAD_LOCAL
lh_runtime_terminate_cb m_runtime_terminate = lh_null;
#endif

lh_runtime_terminate_cb lh_runtime_terminate_set(lh_runtime_terminate_cb fn) {
    lh_runtime_terminate_fn *prev = m_runtime_terminate;
    m_runtime_terminate = fn;
    return prev;
}

void lh_runtime_terminate(void) {
    m_runtime_terminate();
    lh_compiler_unreachable();
}
