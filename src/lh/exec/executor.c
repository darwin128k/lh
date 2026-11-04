#include <lh/exec/executor.h>
#include <lh/core/assert/runtime.h>

lh_exec_executor_accept_cb
lh_exec_executor_get_accept_cb(const lh_exec_executor_t *self)
{
    lh_assert_runtime_ref(self);
    return self->accept_cb;
}

lh_void
lh_exec_executor_set_accept_cb(lh_exec_executor_t *self, lh_exec_executor_accept_cb accept_cb)
{
    lh_assert_runtime_ref(self);
    self->accept_cb = accept_cb;
}

lh_ptr
lh_exec_executor_get_context(const lh_exec_executor_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_exec_executor_set_context(lh_exec_executor_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_void
lh_exec_executor_init(lh_exec_executor_t *self, lh_exec_executor_accept_cb accept_cb,
                      lh_ptr context)
{
    lh_exec_executor_set_accept_cb(self, accept_cb);
    lh_exec_executor_set_context(self, context);
}

lh_bool_t
lh_exec_executor_try_accept(const lh_exec_executor_t *self, lh_exec_task_t *task)
{
    return lh_exec_executor_get_accept_cb(self)(lh_exec_executor_get_context(self), task);
}

lh_bool_t
lh_exec_executor_try_submit(const lh_exec_executor_t *self, lh_exec_task_t *task,
                            lh_exec_mailbox_t *mailbox)
{
    if (!lh_exec_mailbox_try_reserve(mailbox))
    {
        return lh_bool_false;
    }
    lh_exec_task_set_mailbox(task, mailbox);
    if (lh_exec_executor_try_accept(self, task))
    {
        return lh_bool_true;
    }
    /* Refused: the task never reaches the mailbox, so its place goes back now. */
    lh_exec_mailbox_release(mailbox);
    return lh_bool_false;
}

lh_void
lh_exec_executor_submit(const lh_exec_executor_t *self, lh_exec_task_t *task,
                        lh_exec_mailbox_t *mailbox)
{
    lh_runtime_check_ifn(lh_exec_executor_try_submit(self, task, mailbox),
                         lh_runtime_error_code_not_accepted);
}
