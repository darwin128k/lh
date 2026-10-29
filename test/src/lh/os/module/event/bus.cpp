#include <gtest/gtest.h>
#include <lh/os/module/event/bus.h>
#include <lh/runtime/allocator.h>
#include <lh/expect/death.h>
#include <functional>
#include <memory>
#include <string>
#include <array>
#include <cstdlib>

namespace
{

using handler = std::function<lh_os_module_event_result_t(const char *, void *)>;

lh_os_module_event_result_t
invoke(const lh_char_t *name, lh_ptr payload, lh_ptr context)
{
    return (*static_cast<handler *>(context))(name, payload);
}

class event_bus : public testing::Test
{
    lh_os_module_event_bus_t bus;

  protected:
    lh_os_module_event_bus_t *
    get_bus()
    {
        return &bus;
    }

    void
    SetUp() override
    {
        lh_os_module_event_bus_init(get_bus());
    }

    void
    TearDown() override
    {
        lh_os_module_event_bus_deinit(get_bus());
    }

    lh_os_module_event_id_t
    id(const char *name)
    {
        return lh_os_module_event_bus_get_id(get_bus(), name);
    }

    lh_os_module_event_subscription_t *
    subscribe(lh_os_module_event_id_t event, handler &fn, void *owner = nullptr)
    {
        return lh_os_module_event_bus_subscribe(get_bus(), event, owner, invoke, &fn, nullptr);
    }
};

TEST_F(event_bus, registry_owns_names_and_preserves_ids_across_clear_and_growth)
{
    char name[] = "test.first";
    const auto first = id(name);
    name[0] = 'X';
    EXPECT_NE(first, 0u);
    EXPECT_EQ(id("test.first"), first);
    auto *entry = lh_os_module_event_bus_get_entry(get_bus(), first);
    for (int i = 0; i < 1000; ++i)
    {
        EXPECT_NE(id(("test." + std::to_string(i)).c_str()), first);
    }
    EXPECT_EQ(lh_os_module_event_bus_get_entry(get_bus(), first), entry);
    EXPECT_STREQ(lh_os_module_event_entry_get_name(entry), "test.first");
    lh_os_module_event_bus_clear(get_bus());
    EXPECT_EQ(lh_os_module_event_bus_find_id(get_bus(), "test.first"), first);
    EXPECT_EQ(lh_os_module_event_bus_find_id(get_bus(), "test.unknown"), 0u);
    EXPECT_EQ(lh_os_module_event_bus_try_get_id(get_bus(), ""), 0u);
    EXPECT_EQ(lh_os_module_event_bus_try_get_id(get_bus(), nullptr), 0u);
    EXPECT_EQ(lh_os_module_event_bus_get_entry(get_bus(), 0), nullptr);
    EXPECT_EQ(lh_os_module_event_bus_get_entry(get_bus(), LH_USIZE_T_MAX), nullptr);
}

TEST_F(event_bus, publishes_only_matching_event_in_order_and_shares_query_payload)
{
    const auto event = id("test.query");
    std::string trace;
    int value = 3;
    handler first = [&](const char *name, void *payload)
    {
        EXPECT_STREQ(name, "test.query");
        EXPECT_EQ(payload, &value);
        ++*static_cast<int *>(payload);
        trace += 'a';
        return lh_os_module_event_result_handled;
    };
    handler second = [&](const char *, void *payload)
    {
        EXPECT_EQ(*static_cast<int *>(payload), 4);
        trace += 'b';
        return lh_os_module_event_result_continue;
    };
    handler unrelated = [](const char *, void *)
    {
        ADD_FAILURE() << "unrelated event dispatched";
        return lh_os_module_event_result_stop;
    };
    subscribe(event, first);
    subscribe(id("test.other"), unrelated);
    subscribe(event, second);
    EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), event, &value),
              lh_os_module_event_result_handled);
    EXPECT_EQ(trace, "ab");
    EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), 0, nullptr),
              lh_os_module_event_result_continue);
    EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), LH_USIZE_T_MAX, nullptr),
              lh_os_module_event_result_continue);
}

TEST_F(event_bus, stop_prevents_later_callbacks_and_duplicate_insertions_are_distinct)
{
    const auto event = id("test.stop");
    int calls = 0;
    handler stop = [&](const char *, void *)
    {
        ++calls;
        return lh_os_module_event_result_stop;
    };
    EXPECT_NE(subscribe(event, stop), subscribe(event, stop));
    EXPECT_EQ(lh_os_module_event_bus_get_count(get_bus()), 2u);
    EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), event, nullptr),
              lh_os_module_event_result_stop);
    EXPECT_EQ(calls, 1);
}

TEST_F(event_bus, self_and_next_removal_are_immediate_and_disposal_waits_for_outermost_publish)
{
    const auto event = id("test.remove");
    const auto nested = id("test.nested");
    lh_os_module_event_subscription_t *self = nullptr;
    lh_os_module_event_subscription_t *next = nullptr;
    int disposed = 0;
    int calls = 0;
    handler nested_fn = [&](const char *, void *)
    {
        EXPECT_EQ(disposed, 0);
        EXPECT_EQ(lh_os_module_event_bus_get_depth(get_bus()), 2u);
        return lh_os_module_event_result_continue;
    };
    handler fn = [&](const char *, void *)
    {
        ++calls;
        lh_os_module_event_bus_remove(get_bus(), self);
        lh_os_module_event_bus_remove(get_bus(), self);
        lh_os_module_event_bus_remove(get_bus(), next);
        EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), nested, nullptr),
                  lh_os_module_event_result_continue);
        EXPECT_EQ(disposed, 0);
        return lh_os_module_event_result_continue;
    };
    handler forbidden = [](const char *, void *)
    {
        ADD_FAILURE() << "removed callback dispatched";
        return lh_os_module_event_result_continue;
    };
    self = subscribe(event, fn);
    // Disposal context is independent of the handler context on the next record.
    next = lh_os_module_event_bus_subscribe(
        get_bus(), event, nullptr,
        [](const char *, void *, void *)
        {
            ADD_FAILURE();
            return lh_os_module_event_result_continue;
        },
        &disposed, [](void *value) { ++*static_cast<int *>(value); });
    subscribe(nested, nested_fn);
    EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), event, nullptr),
              lh_os_module_event_result_continue);
    EXPECT_EQ(disposed, 1);
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(lh_os_module_event_bus_get_count(get_bus()), 1u);
    EXPECT_EQ(lh_os_module_event_bus_get_depth(get_bus()), 0u);
}

TEST_F(event_bus, nested_registration_growth_and_new_subscription_join_current_delivery)
{
    const auto event = id("test.growth");
    std::string trace;
    handler added = [&](const char *, void *)
    {
        trace += 'b';
        return lh_os_module_event_result_handled;
    };
    handler first = [&](const char *name, void *)
    {
        trace += 'a';
        for (int i = 0; i < 1000; ++i)
        {
            id(("extra." + std::to_string(i)).c_str());
        }
        EXPECT_STREQ(name, "test.growth");
        subscribe(event, added);
        return lh_os_module_event_result_continue;
    };
    subscribe(event, first);
    EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), event, nullptr),
              lh_os_module_event_result_handled);
    EXPECT_EQ(trace, "ab");
}

TEST_F(event_bus, owner_drop_and_clear_inside_callback_skip_remaining_subscribers)
{
    const auto event = id("test.owner");
    int owner = 0;
    int calls = 0;
    handler fn = [&](const char *, void *)
    {
        ++calls;
        lh_os_module_event_bus_drop_owner(get_bus(), &owner);
        EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), event, nullptr),
                  lh_os_module_event_result_continue);
        lh_os_module_event_bus_clear(get_bus());
        return lh_os_module_event_result_handled;
    };
    handler never = [](const char *, void *)
    {
        ADD_FAILURE();
        return lh_os_module_event_result_continue;
    };
    subscribe(event, fn, &owner);
    subscribe(event, never, &owner);
    subscribe(id("test.other"), never);
    EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), event, nullptr),
              lh_os_module_event_result_handled);
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(lh_os_module_event_bus_get_count(get_bus()), 0u);
    EXPECT_EQ(id("test.owner"), event);
}

TEST_F(event_bus, failed_registration_and_subscription_preserve_existing_state)
{
    const auto event = id("test.live");
    handler fn = [](const char *, void *) { return lh_os_module_event_result_handled; };
    subscribe(event, fn);
    lh_memory_allocator_t saved;
    lh_memory_allocator_assign(&saved, lh_runtime_allocator());
    auto restore =
        std::unique_ptr<lh_memory_allocator_t, std::function<void(lh_memory_allocator_t *)>>(
            &saved,
            [](auto *allocator) { lh_memory_allocator_assign(lh_runtime_allocator(), allocator); });
    lh_memory_allocator_set_alloc_cb(lh_runtime_allocator(), nullptr);
    EXPECT_EQ(lh_os_module_event_bus_try_get_id(get_bus(), "test.live"), event);
    EXPECT_EQ(lh_os_module_event_bus_try_get_id(get_bus(), "test.failed"), 0u);
    EXPECT_EQ(lh_os_module_event_bus_try_subscribe(get_bus(), event, nullptr, invoke, &fn, nullptr),
              nullptr);
    EXPECT_EQ(lh_os_module_event_bus_try_subscribe(get_bus(), 0, nullptr, invoke, &fn, nullptr),
              nullptr);
    EXPECT_EQ(
        lh_os_module_event_bus_try_subscribe(get_bus(), event, nullptr, nullptr, &fn, nullptr),
        nullptr);
    EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), event, nullptr),
              lh_os_module_event_result_handled);
    EXPECT_EQ(lh_os_module_event_bus_get_count(get_bus()), 1u);
    EXPECT_EQ(lh_os_module_event_bus_find_id(get_bus(), "test.failed"), 0u);
}

TEST_F(event_bus, checked_forms_report_failures)
{
    LH_EXPECT_CHECK_DEATH(lh_os_module_event_bus_get_id(get_bus(), nullptr));
    LH_EXPECT_CHECK_DEATH(
        lh_os_module_event_bus_subscribe(get_bus(), 0, nullptr, nullptr, nullptr, nullptr));
}

TEST_F(event_bus, cached_ids_retry_failures_and_survive_subscription_clear)
{
    lh_os_module_event_id_t cached = 0;
    EXPECT_EQ(lh_os_module_event_bus_try_get_cached_id(get_bus(), &cached, nullptr), 0u);
    EXPECT_NE(lh_os_module_event_bus_try_get_cached_id(get_bus(), &cached, "test.cached"), 0u);
    const auto original = cached;
    EXPECT_EQ(lh_os_module_event_bus_get_cached_id(get_bus(), &cached, "test.cached"), original);
    lh_os_module_event_id_t invalid = 0;
    LH_EXPECT_CHECK_DEATH(lh_os_module_event_bus_get_cached_id(get_bus(), &invalid, nullptr));
    lh_os_module_event_bus_clear(get_bus());
    EXPECT_EQ(lh_os_module_event_bus_try_get_cached_id(get_bus(), &cached, "test.cached"),
              original);
    EXPECT_EQ(lh_os_module_event_bus_find_id(get_bus(), "test.cached"), original);
}

TEST_F(event_bus, entry_owns_its_name_and_checked_constructor_reports_failure)
{
    char name[] = "entry";
    auto *entry = lh_os_module_event_entry_create(name, 5);
    name[0] = 'X';
    EXPECT_STREQ(lh_os_module_event_entry_get_name(entry), "entry");
    EXPECT_EQ(lh_os_module_event_entry_get_length(entry), 5u);
    EXPECT_TRUE(lh_list_is_empty(lh_os_module_event_entry_get_subscriptions(entry)));
    lh_os_module_event_entry_destroy(entry);
    EXPECT_EQ(lh_os_module_event_entry_try_create(nullptr, 1), nullptr);
    EXPECT_EQ(lh_os_module_event_entry_try_create("", 0), nullptr);
    EXPECT_EQ(lh_os_module_event_entry_try_create("x", LH_USIZE_T_MAX), nullptr);
    LH_EXPECT_CHECK_DEATH(lh_os_module_event_entry_create(nullptr, 1));
}

using allocation_state = std::array<int, 2>;

void *
fail_after(lh_self_ptr self, lh_usize_t size)
{
    auto *state = static_cast<allocation_state *>(self);
    if ((*state)[0]-- == 0)
    {
        return nullptr;
    }
    void *block = std::malloc(size);
    if (block)
    {
        ++(*state)[1];
    }
    return block;
}

void
count_free(lh_self_ptr self, lh_ptr block)
{
    auto *state = static_cast<allocation_state *>(self);
    if (block)
    {
        --(*state)[1];
    }
    std::free(block);
}

TEST_F(event_bus, every_registration_allocation_failure_rolls_back_and_can_be_retried)
{
    lh_memory_allocator_t saved;
    lh_memory_allocator_assign(&saved, lh_runtime_allocator());
    auto restore =
        std::unique_ptr<lh_memory_allocator_t, std::function<void(lh_memory_allocator_t *)>>(
            &saved,
            [](auto *allocator) { lh_memory_allocator_assign(lh_runtime_allocator(), allocator); });
    for (int failure = 0; failure < 3; ++failure)
    {
        SCOPED_TRACE(failure);
        allocation_state state = {failure, 0};
        lh_memory_allocator_init(lh_runtime_allocator(), fail_after, count_free);
        lh_memory_allocator_set_context(lh_runtime_allocator(), &state);
        lh_os_module_event_bus_t local;
        lh_os_module_event_bus_init(&local);
        EXPECT_EQ(lh_os_module_event_bus_try_get_id(&local, "test.retry"), 0u);
        EXPECT_EQ(lh_os_module_event_bus_find_id(&local, "test.retry"), 0u);
        EXPECT_EQ(state[1], 0);
        state[0] = 100;
        const auto event = lh_os_module_event_bus_try_get_id(&local, "test.retry");
        EXPECT_NE(event, 0u);
        int disposed = 0;
        state[0] = 0;
        EXPECT_EQ(lh_os_module_event_bus_try_subscribe(
                      &local, event, nullptr, [](const char *, void *, void *)
                      { return lh_os_module_event_result_continue; }, &disposed,
                      [](void *value) { ++*static_cast<int *>(value); }),
                  nullptr);
        EXPECT_EQ(disposed, 0);
        lh_os_module_event_bus_deinit(&local);
        EXPECT_EQ(state[1], 0);
    }
}

TEST_F(event_bus, finds_only_live_records_matching_owner_and_context)
{
    const auto event = id("test.find");
    int owner = 0;
    int other = 0;
    handler fn = [](const char *, void *) { return lh_os_module_event_result_continue; };
    auto *first = subscribe(event, fn, &owner);
    auto *second = subscribe(event, fn, &other);
    auto same_context = [](void *context, void *key) -> lh_bool_t
    { return context == key ? lh_bool_true : lh_bool_false; };
    EXPECT_EQ(lh_os_module_event_bus_find_subscription(get_bus(), event, &owner, same_context, &fn),
              first);
    EXPECT_EQ(lh_os_module_event_bus_find_subscription(get_bus(), event, &other, nullptr, nullptr),
              second);
    EXPECT_EQ(
        lh_os_module_event_bus_find_subscription(get_bus(), event, &owner, same_context, &other),
        nullptr);
    lh_os_module_event_bus_remove(get_bus(), first);
    EXPECT_EQ(lh_os_module_event_bus_find_subscription(get_bus(), event, &owner, nullptr, nullptr),
              nullptr);
    EXPECT_EQ(lh_os_module_event_bus_find_subscription(get_bus(), 0, &other, nullptr, nullptr),
              nullptr);
}

TEST_F(event_bus, subscribe_once_returns_the_record_made_with_the_same_owner_callback_and_context)
{
    int calls = 0;
    handler fn = [&](const char *, void *)
    {
        ++calls;
        return lh_os_module_event_result_continue;
    };
    handler other = [&](const char *, void *) { return lh_os_module_event_result_continue; };
    int owner = 0;
    const lh_os_module_event_id_t event = id("test.once");

    auto *first = lh_os_module_event_bus_try_subscribe_once(get_bus(), event, &owner, invoke, &fn);
    ASSERT_NE(first, nullptr);
    EXPECT_EQ(lh_os_module_event_bus_try_subscribe_once(get_bus(), event, &owner, invoke, &fn),
              first)
        << "asking again is the same subscription";
    EXPECT_EQ(lh_os_module_event_bus_get_count(get_bus()), 1u);
    EXPECT_NE(lh_os_module_event_bus_try_subscribe_once(get_bus(), event, &owner, invoke, &other),
              first)
        << "another context is another subscription";
    EXPECT_NE(lh_os_module_event_bus_try_subscribe_once(get_bus(), event, nullptr, invoke, &fn),
              first)
        << "another owner is another subscription";
    EXPECT_EQ(lh_os_module_event_bus_get_count(get_bus()), 3u);

    EXPECT_EQ(lh_os_module_event_bus_find_exact(get_bus(), event, &owner, invoke, &fn), first);
    EXPECT_TRUE(lh_os_module_event_bus_remove_exact(get_bus(), event, &owner, invoke, &fn));
    EXPECT_FALSE(lh_os_module_event_bus_remove_exact(get_bus(), event, &owner, invoke, &fn))
        << "already removed";
    EXPECT_EQ(lh_os_module_event_bus_find_exact(get_bus(), event, &owner, invoke, &fn), nullptr);
    EXPECT_EQ(lh_os_module_event_bus_publish(get_bus(), event, nullptr),
              lh_os_module_event_result_continue);
    EXPECT_EQ(calls, 1) << "the subscription without an owner still runs";
    EXPECT_EQ(lh_os_module_event_bus_try_subscribe_once(get_bus(), 0, &owner, invoke, &fn),
              nullptr);
    EXPECT_EQ(lh_os_module_event_bus_try_subscribe_once(get_bus(), event, &owner, nullptr, &fn),
              nullptr);
}

} // namespace
