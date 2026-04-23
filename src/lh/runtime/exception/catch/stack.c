#include <lh/assert/static.h>
#include <lh/library/fallback.h>
#include <lh/null.h>
#include <lh/runtime/exception/catch/stack.h>
#include <lh/util/addr.h>

lh_assert_static(LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX,
                 "Zero stack depth makes exception handling impossible");

LH_ATTRIBUTE(THREAD_LOCAL)
lh_exception_catch_t *m_runtime_exceptions[LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX];

LH_ATTRIBUTE(THREAD_LOCAL)
lh_exception_catch_t **m_runtime_exception = lh_null;

lh_exception_catch_t *lh_runtime_exception_catch_stack_get_current(void) {
    return *m_runtime_exception;
}

lh_bool_t lh_runtime_exception_catch_stack_is_begin(void) {
    return m_runtime_exception == m_runtime_exceptions;
}

lh_bool_t lh_runtime_exception_catch_stack_is_end(void) {
    return m_runtime_exception ==
           lh_addr_ref(m_runtime_exceptions[LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX]);
}

lh_exception_catch_t *lh_runtime_exception_catch_stack_next(void) {
    if (!lh_runtime_exception_catch_stack_is_end()) {
        m_runtime_exception++;
        return *m_runtime_exception;
    }
    return lh_null;
}

lh_exception_catch_t *lh_runtime_exception_catch_stack_prev(void) {
    if (!lh_runtime_exception_catch_stack_is_begin()) {
        m_runtime_exception--;
        return *m_runtime_exception;
    }
    return lh_null;
}

lh_exception_catch_t *lh_runtime_exception_catch_stack_push(lh_exception_catch_t *e) {
    if (!lh_runtime_exception_catch_stack_is_end()) {
        *m_runtime_exception = e;
        lh_runtime_exception_catch_stack_next();
        return e;
    }
    return lh_null;
}

lh_compiler_constructor(lh_runtime_exception_catch_stack_init) {
    m_runtime_exception = m_runtime_exceptions;
}

lh_compiler_destructor(lh_runtime_exception_catch_stack_deinit) {
    m_runtime_exception = lh_null;
}