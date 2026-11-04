#include <lh/exec/task.h>
#include <lh/core/assert/runtime.h>

lh_exec_task_work_cb
lh_exec_task_get_work_cb(const lh_exec_task_t *self)
{
    lh_assert_runtime_ref(self);
    return self->work_cb;
}

lh_void
lh_exec_task_set_work_cb(lh_exec_task_t *self, lh_exec_task_work_cb work_cb)
{
    lh_assert_runtime_ref(self);
    self->work_cb = work_cb;
}

lh_exec_task_done_cb
lh_exec_task_get_complete_cb(const lh_exec_task_t *self)
{
    lh_assert_runtime_ref(self);
    return self->complete_cb;
}

lh_void
lh_exec_task_set_complete_cb(lh_exec_task_t *self, lh_exec_task_done_cb complete_cb)
{
    lh_assert_runtime_ref(self);
    self->complete_cb = complete_cb;
}

lh_exec_task_done_cb
lh_exec_task_get_destroy_cb(const lh_exec_task_t *self)
{
    lh_assert_runtime_ref(self);
    return self->destroy_cb;
}

lh_void
lh_exec_task_set_destroy_cb(lh_exec_task_t *self, lh_exec_task_done_cb destroy_cb)
{
    lh_assert_runtime_ref(self);
    self->destroy_cb = destroy_cb;
}

lh_ptr
lh_exec_task_get_user(const lh_exec_task_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_exec_task_set_user(lh_exec_task_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}

const lh_atomic_ptr_t *
lh_exec_task_get_next(const lh_exec_task_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->next);
}

lh_atomic_ptr_t *
lh_exec_task_get_next_mut(lh_exec_task_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->next);
}

const struct lh_exec_mailbox *
lh_exec_task_get_mailbox(const lh_exec_task_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mailbox;
}

struct lh_exec_mailbox *
lh_exec_task_get_mailbox_mut(lh_exec_task_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mailbox;
}

lh_void
lh_exec_task_set_mailbox(lh_exec_task_t *self, struct lh_exec_mailbox *mailbox)
{
    lh_assert_runtime_ref(self);
    self->mailbox = mailbox;
}

lh_bool_t
lh_exec_task_is_cancelled(const lh_exec_task_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cancelled;
}

lh_void
lh_exec_task_set_cancelled(lh_exec_task_t *self, lh_bool_t cancelled)
{
    lh_assert_runtime_ref(self);
    self->cancelled = cancelled;
}

lh_exec_task_t *
lh_exec_task_get_next_task(const lh_exec_task_t *self)
{
    return lh_cast_static(lh_exec_task_t *, lh_atomic_ptr_get_acquire(lh_exec_task_get_next(self)));
}

lh_void
lh_exec_task_init(lh_exec_task_t *self, lh_exec_task_work_cb work_cb,
                  lh_exec_task_done_cb complete_cb, lh_exec_task_done_cb destroy_cb, lh_ptr user)
{
    lh_exec_task_set_work_cb(self, work_cb);
    lh_exec_task_set_complete_cb(self, complete_cb);
    lh_exec_task_set_destroy_cb(self, destroy_cb);
    lh_exec_task_set_user(self, user);
    lh_atomic_ptr_set_relaxed(lh_exec_task_get_next_mut(self), lh_null);
    lh_exec_task_set_mailbox(self, lh_null);
    lh_exec_task_set_cancelled(self, lh_bool_false);
}

lh_void
lh_exec_task_run_work(const lh_exec_task_t *self, lh_ptr local)
{
    lh_exec_task_get_work_cb(self)(lh_exec_task_get_user(self), local);
}

lh_void
lh_exec_task_run_complete(const lh_exec_task_t *self)
{
    lh_exec_task_done_cb complete_cb = lh_exec_task_get_complete_cb(self);
    if (lh_ptr_is_set(complete_cb))
    {
        complete_cb(lh_exec_task_get_user(self));
    }
}

lh_void
lh_exec_task_run_destroy(const lh_exec_task_t *self)
{
    lh_exec_task_done_cb destroy_cb = lh_exec_task_get_destroy_cb(self);
    if (lh_ptr_is_set(destroy_cb))
    {
        destroy_cb(lh_exec_task_get_user(self));
    }
}
