#include <lh/exec/inline.h>
#include <lh/core/attribute/unused.h>
#include <lh/exec/executor/initializer.h>

static const lh_exec_executor_t m_exec_inline_executor =
    lh_exec_executor_initializer(lh_exec_inline_try_accept, lh_null);

lh_bool_t
lh_exec_inline_try_accept(lh_ptr context, lh_exec_task_t *task)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_exec_task_run_work(task, lh_null);
    lh_exec_mailbox_post(lh_exec_task_get_mailbox_mut(task), task);
    return lh_bool_true;
}

const lh_exec_executor_t *
lh_exec_inline_get_executor(void)
{
    return lh_addr_of(m_exec_inline_executor);
}
