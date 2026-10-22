#include <gtest/gtest.h>

#include <lh/sql/mysql.h>

#include <optional>
#include <string>
#include <vector>

namespace
{

using cell = std::optional<std::string>;
using row = std::vector<cell>;

/* What a MySQL server would do, as far as the backend can tell. */
struct fake_server
{
    std::vector<std::string> queries;
    std::vector<std::string> charsets;
    std::vector<row> rows;
    size_t columns = 0;
    std::string error;
    std::string connect;
    bool refuse_connect = false;
    bool reject_utf8mb4 = false;
    bool reject_charsets = false;
    bool fail_query = false;
    bool pending = false;
    int closes = 0;
    int frees = 0;
    unsigned long long insert_id = 0;
};

struct fake_result
{
    std::vector<row> rows;
    size_t next = 0;
    std::vector<lh_char_t *> pointers;
    std::vector<lh_ulong_t> lengths;
};

fake_server *g_server = nullptr;
int g_mysql = 0;

lh_ptr
fake_init(lh_ptr)
{
    return &g_mysql;
}

lh_ptr
fake_real_connect(lh_ptr mysql, const lh_char_t *host, const lh_char_t *user,
                  const lh_char_t *password, const lh_char_t *db, lh_uint_t port, const lh_char_t *,
                  lh_ulong_t)
{
    g_server->connect =
        std::string(host) + " " + user + " " + password + " " + db + " " + std::to_string(port);
    if (g_server->refuse_connect)
    {
        g_server->error = "Access denied for user";
        return nullptr;
    }
    return mysql;
}

void
fake_close(lh_ptr)
{
    ++g_server->closes;
}

const lh_char_t *
fake_error(lh_ptr)
{
    return g_server->error.c_str();
}

lh_sint_t
fake_real_query(lh_ptr, const lh_char_t *query, lh_ulong_t length)
{
    const std::string text(query, length);
    g_server->queries.push_back(text);
    if (g_server->fail_query)
    {
        g_server->error = "You have an error in your SQL syntax";
        return 1;
    }
    g_server->pending = text.rfind("SELECT", 0) == 0;
    return 0;
}

lh_ptr
fake_store_result(lh_ptr)
{
    if (!g_server->pending)
    {
        return nullptr;
    }
    g_server->pending = false;
    auto *result = new fake_result;
    result->rows = g_server->rows;
    return result;
}

void
fake_free_result(lh_ptr result)
{
    ++g_server->frees;
    delete static_cast<fake_result *>(result);
}

lh_char_t **
fake_fetch_row(lh_ptr handle)
{
    auto *result = static_cast<fake_result *>(handle);
    if (result->next >= result->rows.size())
    {
        return nullptr;
    }
    row &r = result->rows[result->next++];
    result->pointers.assign(r.size(), nullptr);
    result->lengths.assign(r.size(), 0);
    for (size_t i = 0; i < r.size(); ++i)
    {
        if (r[i])
        {
            result->pointers[i] = &(*r[i])[0];
            result->lengths[i] = static_cast<lh_ulong_t>(r[i]->size());
        }
    }
    return result->pointers.data();
}

lh_ulong_t *
fake_fetch_lengths(lh_ptr handle)
{
    return static_cast<fake_result *>(handle)->lengths.data();
}

lh_uint_t
fake_num_fields(lh_ptr)
{
    return static_cast<lh_uint_t>(g_server->columns);
}

lh_uint_t
fake_field_count(lh_ptr)
{
    return g_server->pending ? static_cast<lh_uint_t>(g_server->columns) : 0U;
}

lh_ullong_t
fake_insert_id(lh_ptr)
{
    return g_server->insert_id;
}

/* What mysql_real_escape_string does to the characters that matter here. */
lh_ulong_t
fake_real_escape_string(lh_ptr, lh_char_t *to, const lh_char_t *from, lh_ulong_t length)
{
    lh_ulong_t n = 0;
    for (lh_ulong_t i = 0; i < length; ++i)
    {
        const char c = from[i];
        if (c == '\'' || c == '\\' || c == '\0')
        {
            to[n++] = '\\';
            to[n++] = c == '\0' ? '0' : c;
        }
        else
        {
            to[n++] = c;
        }
    }
    to[n] = '\0';
    return n;
}

lh_sint_t
fake_set_character_set(lh_ptr, const lh_char_t *name)
{
    g_server->charsets.push_back(name);
    if (g_server->reject_charsets || (g_server->reject_utf8mb4 && std::string(name) == "utf8mb4"))
    {
        g_server->error = "Unknown character set";
        return 1;
    }
    return 0;
}

std::string
text(lh_str_view_t v)
{
    return std::string(lh_str_view_get_data(&v) ? lh_str_view_get_data(&v) : "",
                       lh_str_view_get_size(&v));
}

lh_str_view_t
view(const std::string &s)
{
    lh_str_view_t v;
    if (s.empty())
    {
        lh_str_view_init_empty(&v);
    }
    else
    {
        lh_str_init_by_size(&v, s.data(), s.size());
    }
    return v;
}

struct sql_mysql : ::testing::Test
{
    fake_server server;
    lh_sql_mysql_api_t api;
    lh_sql_db_t db;

    void
    SetUp() override
    {
        g_server = &server;
        lh_sql_mysql_api_init(&api);
        api.init = fake_init;
        api.real_connect = fake_real_connect;
        api.close = fake_close;
        api.error = fake_error;
        api.real_query = fake_real_query;
        api.store_result = fake_store_result;
        api.free_result = fake_free_result;
        api.fetch_row = fake_fetch_row;
        api.fetch_lengths = fake_fetch_lengths;
        api.num_fields = fake_num_fields;
        api.field_count = fake_field_count;
        api.insert_id = fake_insert_id;
        api.real_escape_string = fake_real_escape_string;
        api.set_character_set = fake_set_character_set;
        lh_sql_db_init(&db);
    }

    void
    TearDown() override
    {
        lh_sql_db_deinit(&db);
        g_server = nullptr;
    }

    bool
    open(lh_uint_t port = 0)
    {
        return lh_sql_db_open_mysql_with(&db, &api, lh_str_view_lit("db.host"),
                                         lh_str_view_lit("hlds"), lh_str_view_lit(""),
                                         lh_str_view_lit("authum"), port);
    }
};

TEST_F(sql_mysql, opens_with_utf8mb4_on_the_default_port)
{
    ASSERT_TRUE(open()) << text(lh_sql_db_get_error(&db));
    EXPECT_TRUE(lh_sql_db_is_open(&db));
    EXPECT_EQ(std::string(lh_sql_driver_get_name(lh_sql_db_get_driver(&db))), "mysql");
    EXPECT_EQ(server.connect, "db.host hlds  authum 3306");
    EXPECT_EQ(server.charsets, (std::vector<std::string>{"utf8mb4"}));
}

TEST_F(sql_mysql, falls_back_to_utf8_on_an_old_server)
{
    server.reject_utf8mb4 = true;
    ASSERT_TRUE(open(3307)) << text(lh_sql_db_get_error(&db));
    EXPECT_EQ(server.connect, "db.host hlds  authum 3307");
    EXPECT_EQ(server.charsets, (std::vector<std::string>{"utf8mb4", "utf8"}));
}

TEST_F(sql_mysql, without_a_safe_character_set_it_does_not_connect)
{
    server.reject_charsets = true;
    EXPECT_FALSE(open());
    EXPECT_FALSE(lh_sql_db_is_open(&db));
    EXPECT_EQ(text(lh_sql_db_get_error(&db)), "Unknown character set");
    EXPECT_EQ(server.closes, 1);
}

TEST_F(sql_mysql, a_refused_connection_says_why_and_closes_the_handle)
{
    server.refuse_connect = true;
    EXPECT_FALSE(open());
    EXPECT_FALSE(lh_sql_db_is_open(&db));
    EXPECT_EQ(text(lh_sql_db_get_error(&db)), "Access denied for user");
    EXPECT_EQ(server.closes, 1);
}

TEST_F(sql_mysql, a_client_that_is_not_loaded_cannot_connect)
{
    lh_sql_mysql_api_t empty;
    lh_sql_mysql_api_init(&empty);
    EXPECT_FALSE(lh_sql_db_open_mysql_with(&db, &empty, lh_str_view_lit("h"), lh_str_view_lit("u"),
                                           lh_str_view_lit("p"), lh_str_view_lit("d"), 0));
    EXPECT_EQ(text(lh_sql_db_get_error(&db)), "MySQL client library not found");
    EXPECT_FALSE(lh_sql_db_open_mysql_with(&db, nullptr, lh_str_view_lit("h"), lh_str_view_lit("u"),
                                           lh_str_view_lit("p"), lh_str_view_lit("d"), 0));
}

TEST_F(sql_mysql, exec_sends_the_text_and_drops_rows_nobody_asked_for)
{
    ASSERT_TRUE(open());
    EXPECT_TRUE(lh_sql_db_exec(&db, lh_str_view_lit("CREATE TABLE t (x INT)")));
    server.columns = 1;
    server.rows = {{cell("1")}};
    EXPECT_TRUE(lh_sql_db_exec(&db, lh_str_view_lit("SELECT 1")));
    EXPECT_EQ(server.frees, 1);

    server.fail_query = true;
    EXPECT_FALSE(lh_sql_db_exec(&db, lh_str_view_lit("SELEKT")));
    EXPECT_EQ(text(lh_sql_db_get_error(&db)), "You have an error in your SQL syntax");
    EXPECT_EQ(server.queries,
              (std::vector<std::string>{"CREATE TABLE t (x INT)", "SELECT 1", "SELEKT"}));
}

TEST_F(sql_mysql, binds_become_escaped_literals_in_the_text)
{
    ASSERT_TRUE(open());
    lh_sql_stmt_t stmt;
    ASSERT_TRUE(lh_sql_db_prepare(&db, &stmt,
                                  lh_str_view_lit("SELECT name, steam, note FROM bans WHERE id = ? "
                                                  "AND name = ? AND note = ? AND tag = '?'")));
    EXPECT_TRUE(lh_sql_stmt_bind_sint(&stmt, 1, -9223372036854775807LL - 1));
    EXPECT_TRUE(lh_sql_stmt_bind_str(&stmt, 2, view(std::string("O'Br\\ien\0x", 10))));
    EXPECT_TRUE(lh_sql_stmt_bind_null(&stmt, 3));
    EXPECT_FALSE(lh_sql_stmt_bind_null(&stmt, 0)); /* parameters count from 1 */

    server.columns = 3;
    server.rows = {{cell("Gordon"), cell("76561198000000000"), std::nullopt},
                   {cell(std::string("a\0b", 3)), cell(""), cell("-12")}};

    ASSERT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_row) << text(lh_sql_db_get_error(&db));
    ASSERT_EQ(server.queries.size(), 1U);
    EXPECT_EQ(server.queries[0],
              std::string("SELECT name, steam, note FROM bans WHERE id = -9223372036854775808 AND "
                          "name = 'O\\'Br\\\\ien\\0x' AND note = NULL AND tag = '?'"));

    EXPECT_EQ(lh_sql_stmt_get_column_count(&stmt), 3U);
    EXPECT_EQ(text(lh_sql_stmt_get_column_str(&stmt, 0)), "Gordon");
    EXPECT_EQ(lh_sql_stmt_get_column_sint(&stmt, 1), 76561198000000000LL);
    EXPECT_TRUE(lh_sql_stmt_is_column_null(&stmt, 2));
    EXPECT_EQ(text(lh_sql_stmt_get_column_str(&stmt, 2)), "");
    EXPECT_TRUE(lh_sql_stmt_is_column_null(&stmt, 3)); /* past the last column */

    ASSERT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_row);
    EXPECT_EQ(text(lh_sql_stmt_get_column_str(&stmt, 0)), std::string("a\0b", 3));
    EXPECT_FALSE(lh_sql_stmt_is_column_null(&stmt, 1));
    EXPECT_EQ(text(lh_sql_stmt_get_column_str(&stmt, 1)), "");
    EXPECT_EQ(lh_sql_stmt_get_column_sint(&stmt, 1), 0);
    EXPECT_EQ(lh_sql_stmt_get_column_sint(&stmt, 2), -12);

    EXPECT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_done);
    lh_sql_stmt_finalize(&stmt);
    EXPECT_EQ(server.frees, 1);
    EXPECT_EQ(server.queries.size(), 1U); /* stepping never resends */
}

TEST_F(sql_mysql, an_unbound_parameter_sends_nothing)
{
    ASSERT_TRUE(open());
    lh_sql_stmt_t stmt;
    ASSERT_TRUE(lh_sql_db_prepare(&db, &stmt, lh_str_view_lit("SELECT ? , ?")));
    EXPECT_TRUE(lh_sql_stmt_bind_sint(&stmt, 2, 2));
    EXPECT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_error);
    EXPECT_EQ(text(lh_sql_db_get_error(&db)), "a parameter is not bound");
    EXPECT_TRUE(server.queries.empty());
    lh_sql_stmt_finalize(&stmt);
}

TEST_F(sql_mysql, reset_or_a_new_bind_runs_the_statement_again)
{
    ASSERT_TRUE(open());
    server.insert_id = 41;
    lh_sql_stmt_t stmt;
    ASSERT_TRUE(lh_sql_db_prepare(&db, &stmt, lh_str_view_lit("INSERT INTO t VALUES (?)")));
    EXPECT_TRUE(lh_sql_stmt_bind_sint(&stmt, 1, 1));
    EXPECT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_done);
    EXPECT_EQ(lh_sql_db_get_last_insert_id(&db), 41);

    EXPECT_TRUE(lh_sql_stmt_bind_sint(&stmt, 1, 2));
    EXPECT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_done);
    EXPECT_TRUE(lh_sql_stmt_reset(&stmt));
    EXPECT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_done);
    EXPECT_EQ(server.queries,
              (std::vector<std::string>{"INSERT INTO t VALUES (1)", "INSERT INTO t VALUES (2)",
                                        "INSERT INTO t VALUES (2)"}));
    lh_sql_stmt_finalize(&stmt);
}

TEST_F(sql_mysql, a_failing_statement_reports_the_server_error)
{
    ASSERT_TRUE(open());
    lh_sql_stmt_t stmt;
    ASSERT_TRUE(lh_sql_db_prepare(&db, &stmt, lh_str_view_lit("SELECT nope")));
    server.fail_query = true;
    EXPECT_EQ(lh_sql_stmt_step(&stmt), lh_sql_step_error);
    EXPECT_EQ(text(lh_sql_db_get_error(&db)), "You have an error in your SQL syntax");
    lh_sql_stmt_finalize(&stmt);

    EXPECT_FALSE(lh_sql_db_prepare(&db, &stmt, lh_str_view_lit("")));
    lh_sql_stmt_finalize(&stmt);
}

TEST_F(sql_mysql, deinit_closes_the_connection_once)
{
    ASSERT_TRUE(open());
    lh_sql_db_close(&db);
    lh_sql_db_close(&db);
    EXPECT_EQ(server.closes, 1);
}

TEST(sql_mysql_api, a_library_that_is_not_a_client_does_not_load)
{
    lh_sql_mysql_api_t api;
    lh_sql_mysql_api_init(&api);
    EXPECT_FALSE(lh_sql_mysql_api_load(&api, lh_str_view_lit("lh_no_such_mysql_client")));
    EXPECT_FALSE(lh_sql_mysql_api_is_loaded(&api));
    lh_sql_mysql_api_deinit(&api);
}

TEST(sql_mysql_api, the_default_client_connects_or_says_why_not)
{
    /* Whether this machine has a client or not, nothing listens on port 1. */
    lh_sql_db_t db;
    lh_sql_db_init(&db);
    EXPECT_FALSE(lh_sql_db_open_mysql(&db, lh_str_view_lit("127.0.0.1"), lh_str_view_lit("u"),
                                      lh_str_view_lit("p"), lh_str_view_lit("d"), 1));
    EXPECT_FALSE(text(lh_sql_db_get_error(&db)).empty());
    lh_sql_db_deinit(&db);
}

} // namespace
