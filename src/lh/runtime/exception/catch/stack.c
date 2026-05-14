#include <lh/runtime/exception/catch/stack.h>
#include <lh/attribute/thread_local.h>
#include <lh/compiler/constructor.h>
#include <lh/compiler/destructor.h>
#include <lh/library/fallback.h>
#include <lh/assert/static.h>
#include <lh/util/addr.h>
#include <lh/null.h>

lh_assert_static(LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX,
                 "Zero stack depth makes exception handling impossible");

LH_ATTRIBUTE_THREAD_LOCAL
lh_exception_catch_t *m_runtime_exceptions[LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX];

LH_ATTRIBUTE_THREAD_LOCAL
lh_exception_catch_t **m_runtime_exception = lh_null;

lh_exception_catch_t *
lh_runtime_exception_catch_stack_get_cur(void)
{
    return lh_ptr_deref(m_runtime_exception);
}

lh_exception_catch_t *
lh_runtime_exception_catch_stack_get_begin(void)
{
    return m_runtime_exceptions[0];
}

lh_usize_t
lh_runtime_exception_catch_stack_get_capacity(void)
{
    return LH_LIBRARY_OPTION_RUNTIME_EXCEPTION_CATCH_STACK_MAX;
}

lh_usize_t
lh_runtime_exception_catch_stack_get_last_index(void)
{
    lh_usize_t capacity = lh_runtime_exception_catch_stack_get_capacity();
    return capacity - 1;
}

lh_exception_catch_t *
lh_runtime_exception_catch_stack_get_end(void)
{
    lh_usize_t last_index = lh_runtime_exception_catch_stack_get_last_index();
    return m_runtime_exceptions[last_index];
}

lh_bool_t
lh_runtime_exception_catch_stack_is_begin(void)
{
    lh_exception_catch_t *begin = lh_runtime_exception_catch_stack_get_begin();
    return m_runtime_exception == lh_addr_of(begin);
}

lh_bool_t
lh_runtime_exception_catch_stack_is_end(void)
{
    lh_exception_catch_t *end = lh_runtime_exception_catch_stack_get_end();
    return m_runtime_exception == lh_addr_of(end);
}

lh_exception_catch_t *
lh_runtime_exception_catch_stack_next(void)
{
    if (!lh_runtime_exception_catch_stack_is_end())
    {
        m_runtime_exception++;
        return lh_ptr_deref(m_runtime_exception);
    }
    return lh_null;
}

lh_exception_catch_t *
lh_runtime_exception_catch_stack_prev(void)
{
    if (!lh_runtime_exception_catch_stack_is_begin())
    {
        m_runtime_exception--;
        return lh_ptr_deref(m_runtime_exception);
    }
    return lh_null;
}

lh_exception_catch_t *
lh_runtime_exception_catch_stack_push(lh_exception_catch_t *e)
{
    if (!lh_runtime_exception_catch_stack_is_end())
    {
        lh_ptr_deref(m_runtime_exception) = e;
        lh_runtime_exception_catch_stack_next();
        return e;
    }
    return lh_null;
}

lh_compiler_constructor(lh_runtime_exception_catch_stack_init)
{
    m_runtime_exception = m_runtime_exceptions;
}

lh_compiler_destructor(lh_runtime_exception_catch_stack_deinit)
{
    m_runtime_exception = lh_null;
}
