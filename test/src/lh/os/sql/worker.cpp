/**
 * @file worker.cpp
 * @brief The SQL worker: one connection opened once on its thread, used by every
 *        task in order, closed and released on the same thread.
 */

#include <gtest/gtest.h>

#include <lh/exec/mailbox.h>
#include <lh/os/sql/sqlite.h>
#include <lh/os/sql/worker.h>
#include <lh/os/tick.h>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace
{

/* What the connect and release calls saw. */
struct link
{
    std::atomic<int> connects{0};
    std::atomic<int> releases{0};
    std::thread::id connect_on;
    std::thread::id release_on;
    std::atomic<bool> fail{false};
};

lh_bool_t
connect_memory(lh_ptr user, lh_sql_db_t *db)
{
    auto *l = static_cast<link *>(user);
    l->connects.fetch_add(1);
    l->connect_on = std::this_thread::get_id();
    if (l->fail)
    {
        return lh_bool_false;
    }
    return lh_os_sql_sqlite_open(db, lh_str_view_lit(":memory:"));
}

void
release_thread(lh_ptr user)
{
    auto *l = static_cast<link *>(user);
    l->releases.fetch_add(1);
    l->release_on = std::this_thread::get_id();
}

/* One query task: a statement run on the worker, its outcome read at dispatch. */
struct query
{
    lh_exec_task_t task;
    std::string sql;
    bool ok = false;
    bool open = false;
    long long value = -1;
    std::thread::id ran_on;
    int completes = 0;
};

void
run_query(lh_ptr user, lh_ptr local)
{
    auto *q = static_cast<query *>(user);
    lh_os_sql_worker_t *worker = lh_os_sql_worker_from_local(local);
    q->ran_on = std::this_thread::get_id();
    q->open = lh_os_sql_worker_try_ensure_open(worker);
    if (!q->open)
    {
        return;
    }
    lh_sql_db_t *db = lh_os_sql_worker_get_db_mut(worker);
    lh_str_view_t text;
    lh_str_view_init_by_size(&text, q->sql.data(), q->sql.size());
    if (q->sql.rfind("SELECT", 0) != 0)
    {
        q->ok = lh_sql_db_exec(db, text);
        return;
    }
    lh_sql_stmt_t stmt;
    lh_sql_stmt_init(&stmt, db);
    if (lh_sql_db_prepare(db, &stmt, text) && lh_sql_stmt_step(&stmt) == lh_sql_step_row)
    {
        q->value = lh_sql_stmt_get_column_sint(&stmt, 0U);
        q->ok = true;
    }
    lh_sql_stmt_finalize(&stmt);
}

void
done_query(lh_ptr user)
{
    ++static_cast<query *>(user)->completes;
}

void
make(query &q, const std::string &sql)
{
    q.sql = sql;
    lh_exec_task_init(&q.task, run_query, done_query, lh_null, &q);
}

void
dispatch_all(lh_exec_mailbox_t *mailbox, unsigned count)
{
    unsigned delivered = 0U;
    const auto until = std::chrono::steady_clock::now() + std::chrono::seconds(20);
    while (delivered < count && std::chrono::steady_clock::now() < until)
    {
        delivered += lh_exec_mailbox_dispatch(mailbox);
        std::this_thread::yield();
    }
    ASSERT_EQ(delivered, count);
}

} // namespace

/* The tasks share one connection: a table made by the first is there for the
   next (":memory:" is one database per connection), and it was opened once. */
TEST(os_sql_worker, tasks_share_one_connection_opened_once)
{
    link l;
    lh_os_sql_worker_t worker;
    ASSERT_TRUE(lh_os_sql_worker_try_start(&worker, connect_memory, release_thread, &l));
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, 16U);
    std::vector<query> queries(5);
    make(queries[0], "CREATE TABLE t (v INTEGER)");
    make(queries[1], "INSERT INTO t VALUES (40)");
    make(queries[2], "INSERT INTO t VALUES (2)");
    make(queries[3], "SELECT SUM(v) FROM t");
    make(queries[4], "SELECT COUNT(*) FROM t");
    for (auto &q : queries)
    {
        ASSERT_TRUE(
            lh_exec_executor_try_submit(lh_os_sql_worker_get_executor(&worker), &q.task, &mailbox));
    }
    dispatch_all(&mailbox, 5U);
    EXPECT_TRUE(lh_os_sql_worker_try_stop(&worker, lh_exec_stop_kind_drain, 10000U));
    lh_os_sql_worker_destroy(&worker);
    for (auto &q : queries)
    {
        EXPECT_TRUE(q.ok) << q.sql;
        EXPECT_EQ(q.completes, 1);
        EXPECT_EQ(q.ran_on, l.connect_on);
    }
    EXPECT_EQ(queries[3].value, 42);
    EXPECT_EQ(queries[4].value, 2);
    EXPECT_EQ(l.connects.load(), 1);
    EXPECT_EQ(l.releases.load(), 1);
    EXPECT_NE(l.connect_on, std::this_thread::get_id());
    EXPECT_EQ(l.release_on, l.connect_on);
}

/* A connection that failed at the start is opened again by the next task. */
TEST(os_sql_worker, a_task_reconnects_a_closed_connection)
{
    link l;
    l.fail = true;
    lh_os_sql_worker_t worker;
    ASSERT_TRUE(lh_os_sql_worker_try_start(&worker, connect_memory, lh_null, &l));
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, 4U);
    query first;
    make(first, "SELECT 1");
    ASSERT_TRUE(
        lh_exec_executor_try_submit(lh_os_sql_worker_get_executor(&worker), &first.task, &mailbox));
    dispatch_all(&mailbox, 1U);
    EXPECT_FALSE(first.open);
    l.fail = false; /* the server is back */
    query second;
    make(second, "SELECT 7");
    ASSERT_TRUE(lh_exec_executor_try_submit(lh_os_sql_worker_get_executor(&worker), &second.task,
                                            &mailbox));
    dispatch_all(&mailbox, 1U);
    lh_os_sql_worker_destroy(&worker);
    EXPECT_TRUE(second.open);
    EXPECT_TRUE(second.ok);
    EXPECT_EQ(second.value, 7);
    EXPECT_EQ(l.connects.load(), 3); /* start, first task, second task */
}

TEST(os_sql_worker, refuses_a_missing_connect_call)
{
    lh_os_sql_worker_t worker;
    EXPECT_FALSE(lh_os_sql_worker_try_start(&worker, lh_null, lh_null, lh_null));
}
