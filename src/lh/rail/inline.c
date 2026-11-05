#include <lh/rail/inline.h>
#include <lh/core/attribute/unused.h>
#include <lh/rail/executor/initializer.h>

static const lh_rail_executor_t m_rail_inline_executor =
    lh_rail_executor_initializer(lh_rail_inline_try_accept, lh_null);

lh_bool_t
lh_rail_inline_try_accept(lh_ptr context, lh_rail_task_t *task)
{
    LH_ATTRIBUTE_UNUSED(context);
    lh_rail_task_run_work(task, lh_null);
    lh_rail_mailbox_post(lh_rail_task_get_mailbox_mut(task), task);
    return lh_bool_true;
}

const lh_rail_executor_t *
lh_rail_inline_get_executor(void)
{
    return lh_addr_of(m_rail_inline_executor);
}
