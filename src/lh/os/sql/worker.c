/**
 * @file worker.c
 * @brief Implementation of `lh/os/sql/worker.h`.
 */

#include <lh/os/sql/worker.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/attribute/unused.h>
#include <lh/rail/context/initializer.h>

const lh_rail_thread_t *
lh_os_sql_worker_get_thread(const lh_os_sql_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->thread);
}

lh_rail_thread_t *
lh_os_sql_worker_get_thread_mut(lh_os_sql_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->thread);
}

const lh_sql_db_t *
lh_os_sql_worker_get_db(const lh_os_sql_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->db);
}

lh_sql_db_t *
lh_os_sql_worker_get_db_mut(lh_os_sql_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->db);
}

lh_os_sql_worker_connect_cb
lh_os_sql_worker_get_connect_cb(const lh_os_sql_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->connect_cb;
}

lh_void
lh_os_sql_worker_set_connect_cb(lh_os_sql_worker_t *self, lh_os_sql_worker_connect_cb value)
{
    lh_assert_runtime_ref(self);
    self->connect_cb = value;
}

lh_os_sql_worker_release_cb
lh_os_sql_worker_get_release_cb(const lh_os_sql_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->release_cb;
}

lh_void
lh_os_sql_worker_set_release_cb(lh_os_sql_worker_t *self, lh_os_sql_worker_release_cb value)
{
    lh_assert_runtime_ref(self);
    self->release_cb = value;
}

lh_ptr
lh_os_sql_worker_get_user(const lh_os_sql_worker_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_os_sql_worker_set_user(lh_os_sql_worker_t *self, lh_ptr value)
{
    lh_assert_runtime_ref(self);
    self->user = value;
}

lh_os_sql_worker_t *
lh_os_sql_worker_from_local(lh_ptr local)
{
    return lh_ptr_rcast(lh_os_sql_worker_t, local);
}

lh_bool_t
lh_os_sql_worker_try_connect(lh_os_sql_worker_t *self)
{
    return lh_os_sql_worker_get_connect_cb(self)(lh_os_sql_worker_get_user(self),
                                                 lh_os_sql_worker_get_db_mut(self));
}

lh_void
lh_os_sql_worker_close(lh_ptr user, lh_ptr local)
{
    lh_os_sql_worker_t *self = lh_ptr_rcast(lh_os_sql_worker_t, user);
    LH_ATTRIBUTE_UNUSED(local);
    lh_sql_db_deinit(lh_os_sql_worker_get_db_mut(self));
    lh_os_sql_worker_release_cb release_cb = lh_os_sql_worker_get_release_cb(self);
    if (lh_null_ne(release_cb))
    {
        release_cb(lh_os_sql_worker_get_user(self));
    }
}

lh_bool_t
lh_os_sql_worker_try_start(lh_os_sql_worker_t *self, lh_os_sql_worker_connect_cb connect_cb,
                           lh_os_sql_worker_release_cb release_cb, lh_ptr user)
{
    if (lh_null_eq(connect_cb))
    {
        return lh_bool_false;
    }
    lh_os_sql_worker_set_connect_cb(self, connect_cb);
    lh_os_sql_worker_set_release_cb(self, release_cb);
    lh_os_sql_worker_set_user(self, user);
    const lh_rail_context_t context =
        lh_rail_context_initializer(lh_os_sql_worker_open, lh_os_sql_worker_close, self);
    return lh_rail_thread_try_start(lh_os_sql_worker_get_thread_mut(self), lh_addr_of(context));
}

const lh_rail_executor_t *
lh_os_sql_worker_get_executor(const lh_os_sql_worker_t *self)
{
    return lh_rail_thread_get_executor(lh_os_sql_worker_get_thread(self));
}

lh_bool_t
lh_os_sql_worker_try_stop(lh_os_sql_worker_t *self, lh_rail_stop_kind_t kind, lh_u32_t timeout_ms)
{
    return lh_rail_thread_try_stop(lh_os_sql_worker_get_thread_mut(self), kind, timeout_ms);
}

lh_void
lh_os_sql_worker_destroy(lh_os_sql_worker_t *self)
{
    lh_rail_thread_destroy(lh_os_sql_worker_get_thread_mut(self));
}

lh_bool_t
lh_os_sql_worker_try_ensure_open(lh_os_sql_worker_t *self)
{
    if (lh_sql_db_is_open(lh_os_sql_worker_get_db(self)))
    {
        return lh_bool_true;
    }
    lh_sql_db_close(lh_os_sql_worker_get_db_mut(self));
    return lh_os_sql_worker_try_connect(self);
}

lh_ptr
lh_os_sql_worker_open(lh_ptr user)
{
    lh_os_sql_worker_t *self = lh_ptr_rcast(lh_os_sql_worker_t, user);
    lh_sql_db_init(lh_os_sql_worker_get_db_mut(self));
    /* A failed connect is the tasks' to see (the connection stays closed). */
    LH_ATTRIBUTE_UNUSED(lh_os_sql_worker_try_connect(self));
    return self;
}
