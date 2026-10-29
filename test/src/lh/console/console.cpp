#include <gtest/gtest.h>
#include <lh/console.h>
#include <functional>
#include <string>

namespace
{

using handler = std::function<lh_bool_t(const lh_console_call_t *)>;

lh_bool_t
invoke(const lh_console_call_t *call, lh_ptr user)
{
    return (*static_cast<handler *>(user))(call);
}

lh_void
append(lh_ptr out, const lh_char_t *text)
{
    static_cast<std::string *>(out)->append(text);
}

lh_console_command_t
make_command(const char *name, handler *fn, const char *usage = nullptr, const char *help = nullptr)
{
    lh_console_command_t command{};

    lh_console_command_set_name(&command, name);
    lh_console_command_set_usage(&command, usage);
    lh_console_command_set_help(&command, help);
    lh_console_command_set_fn(&command, invoke);
    lh_console_command_set_user(&command, fn);
    return command;
}

class console : public testing::Test
{
    lh_console_t registry;

  protected:
    lh_console_t *
    get_console()
    {
        return &registry;
    }

    lh_console_result_t
    run(const char *scope, const char *name, std::initializer_list<const char *> args = {},
        std::string *out = nullptr)
    {
        return lh_console_run(get_console(), scope, name, args.size(), args.begin(),
                              out != nullptr ? append : nullptr, out, nullptr);
    }

    void
    SetUp() override
    {
        lh_console_init(get_console());
    }

    void
    TearDown() override
    {
        lh_console_deinit(get_console());
    }
};

int owner_a;
int owner_b;

} // namespace

TEST(console_word, letters_digits_and_marks)
{
    EXPECT_TRUE(lh_console_is_word("qproto"));
    EXPECT_TRUE(lh_console_is_word("a-b_c.9"));
    EXPECT_FALSE(lh_console_is_word(""));
    EXPECT_FALSE(lh_console_is_word(nullptr));
    EXPECT_FALSE(lh_console_is_word("a b"));
    EXPECT_FALSE(lh_console_is_word("a;"));
}

TEST_F(console, run_passes_scope_name_and_arguments)
{
    std::string seen;
    handler fn = [&](const lh_console_call_t *call)
    {
        seen = std::string(lh_console_call_get_scope(call)) + " " + lh_console_call_get_name(call) +
               " " + lh_console_call_get_arg(call, 0) + " " + lh_console_call_get_arg(call, 1) +
               "|" + lh_console_call_get_arg(call, 2) + "|";
        return lh_bool_true;
    };
    lh_console_command_t command = make_command("Info", &fn);

    ASSERT_TRUE(lh_console_add(get_console(), &owner_a, "QProto", &command));
    EXPECT_EQ(run("qproto", "info", {"x", "y"}), lh_console_result_done);
    EXPECT_EQ(seen, "QProto Info x y||");
    EXPECT_EQ(run("qproto", "missing"), lh_console_result_not_found);
    EXPECT_EQ(run("other", "info"), lh_console_result_not_found);
}

TEST_F(console, refused_arguments_report_bad_usage_and_the_command)
{
    handler fn = [](const lh_console_call_t *) { return lh_bool_false; };
    lh_console_command_t command = make_command("kick", &fn, "<name>", "Kick a player");
    const lh_console_entry_t *ran = nullptr;

    ASSERT_TRUE(lh_console_add(get_console(), &owner_a, "game", &command));
    EXPECT_EQ(lh_console_run(get_console(), "game", "kick", 0U, nullptr, nullptr, nullptr, &ran),
              lh_console_result_bad_usage);
    ASSERT_NE(ran, nullptr);
    EXPECT_STREQ(lh_str_get_data(lh_console_entry_get_usage(ran)), "<name>");
    EXPECT_STREQ(lh_str_get_data(lh_console_entry_get_help(ran)), "Kick a player");
}

TEST_F(console, print_and_printf_write_lines_to_the_sink)
{
    handler fn = [](const lh_console_call_t *call)
    {
        lh_console_call_print(call, "one");
        lh_console_call_printf(call, "%s=%d", "two", 2);
        return lh_bool_true;
    };
    lh_console_command_t command = make_command("say", &fn);
    std::string out;

    ASSERT_TRUE(lh_console_add(get_console(), &owner_a, "s", &command));
    EXPECT_EQ(run("s", "say", {}, &out), lh_console_result_done);
    EXPECT_EQ(out, "one\ntwo=2\n");
    EXPECT_EQ(run("s", "say"), lh_console_result_done);
}

TEST_F(console, add_dedupes_the_same_command_and_refuses_another)
{
    handler one = [](const lh_console_call_t *) { return lh_bool_true; };
    handler two = [](const lh_console_call_t *) { return lh_bool_true; };
    lh_console_command_t first = make_command("cmd", &one);
    lh_console_command_t second = make_command("CMD", &two);

    EXPECT_TRUE(lh_console_add(get_console(), &owner_a, "s", &first));
    EXPECT_TRUE(lh_console_add(get_console(), &owner_a, "s", &first));
    EXPECT_FALSE(lh_console_add(get_console(), &owner_b, "s", &first));
    EXPECT_FALSE(lh_console_add(get_console(), &owner_a, "s", &second));
    EXPECT_EQ(lh_console_get_count(get_console()), 1U);
}

TEST_F(console, add_refuses_bad_words_and_missing_handler)
{
    handler fn = [](const lh_console_call_t *) { return lh_bool_true; };
    lh_console_command_t bad_name = make_command("a b", &fn);
    lh_console_command_t no_fn = make_command("ok", &fn);

    lh_console_command_set_fn(&no_fn, nullptr);
    EXPECT_FALSE(lh_console_add(get_console(), &owner_a, "s", &bad_name));
    EXPECT_FALSE(lh_console_add(get_console(), &owner_a, "s s", &no_fn));
    EXPECT_FALSE(lh_console_add(get_console(), &owner_a, "s", &no_fn));
    EXPECT_FALSE(lh_console_add(get_console(), &owner_a, "s", nullptr));
    EXPECT_EQ(lh_console_get_count(get_console()), 0U);
}

TEST_F(console, remove_needs_the_owner)
{
    handler fn = [](const lh_console_call_t *) { return lh_bool_true; };
    lh_console_command_t command = make_command("cmd", &fn);

    ASSERT_TRUE(lh_console_add(get_console(), &owner_a, "s", &command));
    EXPECT_FALSE(lh_console_remove(get_console(), &owner_b, "s", "cmd"));
    EXPECT_TRUE(lh_console_remove(get_console(), &owner_a, "S", "CMD"));
    EXPECT_FALSE(lh_console_remove(get_console(), &owner_a, "s", "cmd"));
    EXPECT_EQ(run("s", "cmd"), lh_console_result_not_found);
}

TEST_F(console, drop_owner_scope_and_listing)
{
    handler fn = [](const lh_console_call_t *) { return lh_bool_true; };
    lh_console_command_t one = make_command("one", &fn);
    lh_console_command_t two = make_command("two", &fn);
    std::string names;

    ASSERT_TRUE(lh_console_add(get_console(), &owner_a, "a", &one));
    ASSERT_TRUE(lh_console_add(get_console(), &owner_b, "b", &one));
    ASSERT_TRUE(lh_console_add(get_console(), &owner_a, "a", &two));
    EXPECT_TRUE(lh_console_has_scope(get_console(), "B"));
    for (const lh_console_entry_t *entry = lh_console_get_first(get_console()); entry != nullptr;
         entry = lh_console_get_next(get_console(), entry))
    {
        names += std::string(lh_str_get_data(lh_console_entry_get_scope(entry))) + "." +
                 lh_str_get_data(lh_console_entry_get_name(entry)) + " ";
    }
    EXPECT_EQ(names, "a.one b.one a.two ");
    lh_console_drop_owner(get_console(), &owner_a);
    EXPECT_EQ(lh_console_get_count(get_console()), 1U);
    EXPECT_FALSE(lh_console_has_scope(get_console(), "a"));
    lh_console_clear(get_console());
    EXPECT_EQ(lh_console_get_count(get_console()), 0U);
    EXPECT_EQ(lh_console_get_first(get_console()), nullptr);
}

TEST_F(console, handler_may_remove_itself_and_add_others)
{
    handler other = [](const lh_console_call_t *) { return lh_bool_true; };
    lh_console_command_t added = make_command("added", &other);
    lh_console_t *registry = get_console();
    handler self = [&](const lh_console_call_t *call)
    {
        EXPECT_TRUE(lh_console_remove(registry, &owner_a, "s", "self"));
        EXPECT_EQ(lh_console_find(registry, "s", "self"), nullptr);
        EXPECT_STREQ(lh_console_call_get_name(call), "self");
        EXPECT_TRUE(lh_console_add(registry, &owner_a, "s", &added));
        return lh_bool_true;
    };
    lh_console_command_t command = make_command("self", &self);

    ASSERT_TRUE(lh_console_add(registry, &owner_a, "s", &command));
    EXPECT_EQ(run("s", "self"), lh_console_result_done);
    EXPECT_EQ(run("s", "self"), lh_console_result_not_found);
    EXPECT_EQ(run("s", "added"), lh_console_result_done);
    EXPECT_EQ(lh_console_get_count(registry), 1U);
}

TEST_F(console, nested_run_defers_freeing_until_the_outermost_returns)
{
    lh_console_t *registry = get_console();
    handler inner = [&](const lh_console_call_t *)
    {
        lh_console_drop_owner(registry, &owner_b);
        return lh_bool_true;
    };
    lh_console_command_t inner_command = make_command("inner", &inner);
    handler outer = [&](const lh_console_call_t *)
    {
        EXPECT_EQ(lh_console_run(registry, "s", "inner", 0U, nullptr, nullptr, nullptr, nullptr),
                  lh_console_result_done);
        EXPECT_EQ(lh_console_get_count(registry), 1U);
        return lh_bool_true;
    };
    lh_console_command_t outer_command = make_command("outer", &outer);

    ASSERT_TRUE(lh_console_add(registry, &owner_a, "s", &outer_command));
    ASSERT_TRUE(lh_console_add(registry, &owner_b, "s", &inner_command));
    EXPECT_EQ(run("s", "outer"), lh_console_result_done);
    EXPECT_EQ(lh_console_get_count(registry), 1U);
}
