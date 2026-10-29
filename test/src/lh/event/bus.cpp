#include <gtest/gtest.h>
#include <lh/event/bus.h>
#include <lh/runtime/allocator.h>
#include <lh/expect/death.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace
{

using handler = std::function<lh_event_result_t(const char *, void *)>;

lh_event_result_t
invoke(const lh_char_t *name, lh_ptr payload, lh_ptr context)
{
    return (*static_cast<handler *>(context))(name, payload);
}

class event_bus : public testing::Test
{
    lh_event_bus_t bus;

protected:
    lh_event_bus_t *get_bus() { return &bus; }
    void SetUp() override { lh_event_bus_init(get_bus()); }
    void TearDown() override { lh_event_bus_deinit(get_bus()); }
    lh_event_id_t id(const char *name) { return lh_event_bus_get_id(get_bus(), name); }
    lh_event_subscription_t *subscribe(lh_event_id_t event, handler &fn, void *owner = nullptr)
    {
        return lh_event_bus_subscribe(get_bus(), event, owner, invoke, &fn, nullptr);
    }
};

TEST_F(event_bus, registry_owns_names_and_preserves_ids_across_clear_and_growth)
{
    char name[] = "test.first";
    const auto first = id(name);
    name[0] = 'X';
    EXPECT_NE(first, 0u);
    EXPECT_EQ(id("test.first"), first);
    auto *entry = lh_event_bus_get_entry(get_bus(), first);
    for (int i = 0; i < 1000; ++i)
    {
        EXPECT_NE(id(("test." + std::to_string(i)).c_str()), first);
    }
    EXPECT_EQ(lh_event_bus_get_entry(get_bus(), first), entry);
    EXPECT_STREQ(lh_event_entry_get_name(entry), "test.first");
    lh_event_bus_clear(get_bus());
    EXPECT_EQ(lh_event_bus_find_id(get_bus(), "test.first"), first);
    EXPECT_EQ(lh_event_bus_find_id(get_bus(), "test.unknown"), 0u);
    EXPECT_EQ(lh_event_bus_try_get_id(get_bus(), ""), 0u);
    EXPECT_EQ(lh_event_bus_try_get_id(get_bus(), nullptr), 0u);
    EXPECT_EQ(lh_event_bus_get_entry(get_bus(), 0), nullptr);
    EXPECT_EQ(lh_event_bus_get_entry(get_bus(), LH_USIZE_T_MAX), nullptr);
}

TEST_F(event_bus, publishes_only_matching_event_in_order_and_shares_query_payload)
{
    const auto event = id("test.query");
    std::string trace;
    int value = 3;
    handler first = [&](const char *name, void *payload) {
        EXPECT_STREQ(name, "test.query");
        EXPECT_EQ(payload, &value);
        ++*static_cast<int *>(payload);
        trace += 'a';
        return lh_event_result_handled;
    };
    handler second = [&](const char *, void *payload) {
        EXPECT_EQ(*static_cast<int *>(payload), 4);
        trace += 'b';
        return lh_event_result_continue;
    };
    handler unrelated = [](const char *, void *) {
        ADD_FAILURE() << "unrelated event dispatched";
        return lh_event_result_stop;
    };
    subscribe(event, first);
    subscribe(id("test.other"), unrelated);
    subscribe(event, second);
    EXPECT_EQ(lh_event_bus_publish(get_bus(), event, &value), lh_event_result_handled);
    EXPECT_EQ(trace, "ab");
    EXPECT_EQ(lh_event_bus_publish(get_bus(), 0, nullptr), lh_event_result_continue);
    EXPECT_EQ(lh_event_bus_publish(get_bus(), LH_USIZE_T_MAX, nullptr), lh_event_result_continue);
}

TEST_F(event_bus, stop_prevents_later_callbacks_and_duplicate_insertions_are_distinct)
{
    const auto event = id("test.stop");
    int calls = 0;
    handler stop = [&](const char *, void *) { ++calls; return lh_event_result_stop; };
    EXPECT_NE(subscribe(event, stop), subscribe(event, stop));
    EXPECT_EQ(lh_event_bus_get_count(get_bus()), 2u);
    EXPECT_EQ(lh_event_bus_publish(get_bus(), event, nullptr), lh_event_result_stop);
    EXPECT_EQ(calls, 1);
}

TEST_F(event_bus, self_and_next_removal_are_immediate_and_disposal_waits_for_outermost_publish)
{
    const auto event = id("test.remove");
    const auto nested = id("test.nested");
    lh_event_subscription_t *self = nullptr;
    lh_event_subscription_t *next = nullptr;
    int disposed = 0;
    int calls = 0;
    handler nested_fn = [&](const char *, void *) {
        EXPECT_EQ(disposed, 0);
        EXPECT_EQ(lh_event_bus_get_depth(get_bus()), 2u);
        return lh_event_result_continue;
    };
    handler fn = [&](const char *, void *) {
        ++calls;
        lh_event_bus_remove(get_bus(), self);
        lh_event_bus_remove(get_bus(), self);
        lh_event_bus_remove(get_bus(), next);
        EXPECT_EQ(lh_event_bus_publish(get_bus(), nested, nullptr), lh_event_result_continue);
        EXPECT_EQ(disposed, 0);
        return lh_event_result_continue;
    };
    handler forbidden = [](const char *, void *) {
        ADD_FAILURE() << "removed callback dispatched";
        return lh_event_result_continue;
    };
    self = subscribe(event, fn);
    // Disposal context is independent of the handler context on the next record.
    next = lh_event_bus_subscribe(get_bus(), event, nullptr,
        [](const char *, void *, void *) { ADD_FAILURE(); return lh_event_result_continue; },
        &disposed, [](void *value) { ++*static_cast<int *>(value); });
    subscribe(nested, nested_fn);
    EXPECT_EQ(lh_event_bus_publish(get_bus(), event, nullptr), lh_event_result_continue);
    EXPECT_EQ(disposed, 1);
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(lh_event_bus_get_count(get_bus()), 1u);
    EXPECT_EQ(lh_event_bus_get_depth(get_bus()), 0u);
}

TEST_F(event_bus, nested_registration_growth_and_new_subscription_join_current_delivery)
{
    const auto event = id("test.growth");
    std::string trace;
    handler added = [&](const char *, void *) { trace += 'b'; return lh_event_result_handled; };
    handler first = [&](const char *name, void *) {
        trace += 'a';
        for (int i = 0; i < 1000; ++i) { id(("extra." + std::to_string(i)).c_str()); }
        EXPECT_STREQ(name, "test.growth");
        subscribe(event, added);
        return lh_event_result_continue;
    };
    subscribe(event, first);
    EXPECT_EQ(lh_event_bus_publish(get_bus(), event, nullptr), lh_event_result_handled);
    EXPECT_EQ(trace, "ab");
}

TEST_F(event_bus, owner_drop_and_clear_inside_callback_skip_remaining_subscribers)
{
    const auto event = id("test.owner");
    int owner = 0;
    int calls = 0;
    handler fn = [&](const char *, void *) {
        ++calls;
        lh_event_bus_drop_owner(get_bus(), &owner);
        EXPECT_EQ(lh_event_bus_publish(get_bus(), event, nullptr), lh_event_result_continue);
        lh_event_bus_clear(get_bus());
        return lh_event_result_handled;
    };
    handler never = [](const char *, void *) { ADD_FAILURE(); return lh_event_result_continue; };
    subscribe(event, fn, &owner);
    subscribe(event, never, &owner);
    subscribe(id("test.other"), never);
    EXPECT_EQ(lh_event_bus_publish(get_bus(), event, nullptr), lh_event_result_handled);
    EXPECT_EQ(calls, 1);
    EXPECT_EQ(lh_event_bus_get_count(get_bus()), 0u);
    EXPECT_EQ(id("test.owner"), event);
}

TEST_F(event_bus, failed_registration_and_subscription_preserve_existing_state)
{
    const auto event = id("test.live");
    handler fn = [](const char *, void *) { return lh_event_result_handled; };
    subscribe(event, fn);
    lh_memory_allocator_t saved;
    lh_memory_allocator_assign(&saved, lh_runtime_allocator());
    auto restore = std::unique_ptr<lh_memory_allocator_t, std::function<void(lh_memory_allocator_t *)>>(
        &saved, [](auto *allocator) { lh_memory_allocator_assign(lh_runtime_allocator(), allocator); });
    lh_memory_allocator_set_alloc_cb(lh_runtime_allocator(), nullptr);
    EXPECT_EQ(lh_event_bus_try_get_id(get_bus(), "test.live"), event);
    EXPECT_EQ(lh_event_bus_try_get_id(get_bus(), "test.failed"), 0u);
    EXPECT_EQ(lh_event_bus_try_subscribe(get_bus(), event, nullptr, invoke, &fn, nullptr), nullptr);
    EXPECT_EQ(lh_event_bus_try_subscribe(get_bus(), 0, nullptr, invoke, &fn, nullptr), nullptr);
    EXPECT_EQ(lh_event_bus_try_subscribe(get_bus(), event, nullptr, nullptr, &fn, nullptr), nullptr);
    EXPECT_EQ(lh_event_bus_publish(get_bus(), event, nullptr), lh_event_result_handled);
    EXPECT_EQ(lh_event_bus_get_count(get_bus()), 1u);
    EXPECT_EQ(lh_event_bus_find_id(get_bus(), "test.failed"), 0u);
}

TEST_F(event_bus, checked_forms_report_failures)
{
    LH_EXPECT_CHECK_DEATH(lh_event_bus_get_id(get_bus(), nullptr));
    LH_EXPECT_CHECK_DEATH(lh_event_bus_subscribe(get_bus(), 0, nullptr, nullptr, nullptr, nullptr));
}

TEST_F(event_bus, reserve_failure_and_size_overflow_leave_array_intact)
{
    lh_array_t array;
    lh_array_init(&array, sizeof(int));
    auto cleanup = std::unique_ptr<lh_array_t, decltype(&lh_array_deinit)>(&array, lh_array_deinit);
    int value = 42;
    lh_array_push_back(&array, &value);
    const auto capacity = lh_array_get_capacity(&array);
    const auto begin = lh_array_get_begin(&array);
    EXPECT_FALSE(lh_array_try_reserve(&array, LH_USIZE_T_MAX));
    EXPECT_EQ(lh_array_get_capacity(&array), capacity);
    EXPECT_EQ(lh_array_get_begin(&array), begin);
    EXPECT_EQ(lh_array_get_size(&array), 1u);
    EXPECT_EQ(*static_cast<int *>(lh_array_get_ptr(&array, 0)), 42);
    lh_memory_allocator_t saved;
    lh_memory_allocator_assign(&saved, lh_runtime_allocator());
    auto restore = std::unique_ptr<lh_memory_allocator_t, std::function<void(lh_memory_allocator_t *)>>(
        &saved, [](auto *allocator) { lh_memory_allocator_assign(lh_runtime_allocator(), allocator); });
    lh_memory_allocator_set_alloc_cb(lh_runtime_allocator(), nullptr);
    EXPECT_TRUE(lh_array_try_reserve(&array, capacity));
    EXPECT_FALSE(lh_array_try_reserve(&array, capacity + 1));
    EXPECT_EQ(lh_array_get_capacity(&array), capacity);
    EXPECT_EQ(lh_array_get_begin(&array), begin);
    EXPECT_EQ(*static_cast<int *>(lh_array_get_ptr(&array, 0)), 42);
}

} // namespace
