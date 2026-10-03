#include <gtest/gtest.h>

#include <lh/entity.h>
#include <lh/expect/death.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/memory/tree.h>
#include <lh/null.h>
#include <lh/self.h>

#include <cstdlib>
#include <string>

namespace
{

/* What the callbacks below did, in order. */
std::string g_log;

struct entity_test_heap
{
    int live;
};

/* A class two levels deep: base -> widget -> button. */
struct widget
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    int widget_value;
};

struct button
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    int widget_value; /* widget's field, same place */
    int clicks;
};

#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

lh_ptr
entity_test_alloc(lh_self_ptr self, lh_usize_t size)
{
    ++static_cast<entity_test_heap *>(self)->live;
    return std::malloc(static_cast<std::size_t>(size));
}

lh_void
entity_test_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    --static_cast<entity_test_heap *>(self)->live;
    std::free(ptr);
}

lh_void
widget_construct(lh_entity_t *self)
{
    g_log += "W+";
    reinterpret_cast<widget *>(self)->widget_value = 7;
}

lh_void
widget_destruct(lh_entity_t *)
{
    g_log += "W-";
}

lh_void
widget_event(lh_entity_t *, lh_entity_event_t *event)
{
    g_log += "w" + std::to_string(lh_entity_event_get_code(event) & 0xFF);
}

lh_void
button_construct(lh_entity_t *self)
{
    g_log += "B+";
    // The base constructor already ran.
    EXPECT_EQ(reinterpret_cast<button *>(self)->widget_value, 7);
}

lh_void
button_destruct(lh_entity_t *)
{
    g_log += "B-";
}

lh_void
button_event(lh_entity_t *self, lh_entity_event_t *event)
{
    g_log += "b" + std::to_string(lh_entity_event_get_code(event) & 0xFF);
    if (lh_entity_event_get_code(event) == LH_ENTITY_EVENT_USER)
    {
        ++reinterpret_cast<button *>(self)->clicks;
    }
}

lh_void
log_handler(lh_entity_event_t *event, lh_ptr user_data)
{
    g_log += static_cast<const char *>(user_data);
    (void)event;
}

lh_void
stopping_handler(lh_entity_event_t *event, lh_ptr)
{
    g_log += "S";
    lh_entity_event_stop(event);
}

lh_void
self_removing_handler(lh_entity_event_t *event, lh_ptr user_data)
{
    g_log += "R";
    lh_entity_remove_handler(lh_entity_event_get_current(event), self_removing_handler, user_data);
}

LH_COMPILER_EXTERN_C_END

const lh_entity_class_t widget_class = lh_entity_class_initializer(
    &lh_entity_base_class, sizeof(widget), widget_construct, widget_destruct, widget_event);

const lh_entity_class_t button_class = lh_entity_class_initializer(
    &widget_class, sizeof(button), button_construct, button_destruct, button_event);

class Entity : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        heap = entity_test_heap();
        sized = lh_memory_allocator_initializer_with_context(entity_test_alloc, entity_test_dealloc,
                                                             lh_null, &heap);
        g_log.clear();
        root = lh_entity_create_root(&lh_entity_base_class, &sized);
    }

    void
    TearDown() override
    {
        lh_entity_delete(root);
        EXPECT_EQ(heap.live, 0);
    }

    entity_test_heap heap;
    lh_memory_sized_allocator_t sized;
    lh_entity_t *root;
};

TEST_F(Entity, tree_links)
{
    lh_entity_t *a = lh_entity_create(&lh_entity_base_class, root);
    lh_entity_t *b = lh_entity_create(&lh_entity_base_class, root);
    lh_entity_t *a1 = lh_entity_create(&lh_entity_base_class, a);

    EXPECT_EQ(lh_entity_get_parent(root), nullptr);
    EXPECT_EQ(lh_entity_get_parent(a), root);
    EXPECT_EQ(lh_entity_get_parent(a1), a);
    EXPECT_EQ(lh_entity_get_first_child(root), a);
    EXPECT_EQ(lh_entity_get_next_sibling(a), b);
    EXPECT_EQ(lh_entity_get_next_sibling(b), nullptr);
    EXPECT_EQ(lh_entity_get_first_child(b), nullptr);
    EXPECT_EQ(lh_entity_get_class(a), &lh_entity_base_class);
}

TEST_F(Entity, cast_root_and_foreach_child)
{
    lh_entity_t *w = lh_entity_create(&widget_class, root);
    lh_entity_t *b = lh_entity_create(&button_class, w);
    lh_entity_t *plain = lh_entity_create(&lh_entity_base_class, w);

    EXPECT_EQ(lh_entity_cast(b, &widget_class), b);  // a button is a widget
    EXPECT_EQ(lh_entity_cast(b, &button_class), b);
    EXPECT_EQ(lh_entity_cast(w, &button_class), nullptr); // a widget is not a button
    EXPECT_EQ(lh_entity_cast(plain, &widget_class), nullptr);

    EXPECT_EQ(lh_entity_get_root(b), root);
    EXPECT_EQ(lh_entity_get_root(root), root);

    std::string order;
    lh_entity_foreach_child(child, w)
    {
        order += child == b ? "b" : child == plain ? "p" : "?";
    }
    EXPECT_EQ(order, "bp");
    lh_entity_foreach_child(child, plain)
    {
        (void)child;
        order += "x"; // no children: never runs
    }
    EXPECT_EQ(order, "bp");
}

TEST_F(Entity, constructors_base_first_destructors_derived_first)
{
    lh_entity_t *b = lh_entity_create(&button_class, root);
    EXPECT_EQ(g_log, "W+B+");
    EXPECT_EQ(reinterpret_cast<button *>(b)->clicks, 0); // zeroed
    g_log.clear();

    lh_entity_delete(b);
    // DELETE (code 1) reaches the classes first, then the destructors run.
    EXPECT_EQ(g_log, "b1w1B-W-");
    EXPECT_EQ(lh_entity_get_first_child(root), nullptr);
}

TEST_F(Entity, deleting_a_parent_deletes_its_children)
{
    lh_entity_t *panel = lh_entity_create(&widget_class, root);
    lh_entity_create(&button_class, panel);
    lh_entity_create(&button_class, panel);
    const int live_with_panel = heap.live;
    g_log.clear();

    lh_entity_delete(panel);
    // The panel goes first (its children are still there), then each child.
    EXPECT_EQ(g_log, "w1W-b1w1B-W-b1w1B-W-");
    EXPECT_EQ(heap.live, live_with_panel - 3);
}

TEST_F(Entity, events_reach_classes_then_handlers)
{
    lh_entity_t *b = lh_entity_create(&button_class, root);
    lh_entity_add_handler(b, log_handler, const_cast<char *>("h1"));
    lh_entity_add_handler(b, log_handler, const_cast<char *>("h2"));
    g_log.clear();

    EXPECT_FALSE(lh_entity_send_event(b, LH_ENTITY_EVENT_USER, lh_null));
    EXPECT_EQ(g_log, "b0w0h1h2");
    EXPECT_EQ(reinterpret_cast<button *>(b)->clicks, 1);
}

TEST_F(Entity, bubbling_only_with_the_flag)
{
    lh_entity_t *panel = lh_entity_create(&lh_entity_base_class, root);
    lh_entity_t *b = lh_entity_create(&lh_entity_base_class, panel);
    lh_entity_add_handler(panel, log_handler, const_cast<char *>("P"));
    lh_entity_add_handler(b, log_handler, const_cast<char *>("B"));

    lh_entity_send_event(b, LH_ENTITY_EVENT_USER, lh_null);
    EXPECT_EQ(g_log, "B");

    g_log.clear();
    lh_entity_add_flags(b, lh_entity_flags_event_bubble);
    EXPECT_TRUE(lh_entity_has_flags(b, lh_entity_flags_event_bubble));
    lh_entity_send_event(b, LH_ENTITY_EVENT_USER, lh_null);
    EXPECT_EQ(g_log, "BP");

    g_log.clear();
    lh_entity_clear_flags(b, lh_entity_flags_event_bubble);
    lh_entity_send_event(b, LH_ENTITY_EVENT_USER, lh_null);
    EXPECT_EQ(g_log, "B");
}

lh_entity_t *g_seen_target;
lh_entity_t *g_seen_current;
lh_ptr g_seen_param;

LH_COMPILER_EXTERN_C_BEGIN

lh_void
recording_handler(lh_entity_event_t *event, lh_ptr)
{
    g_seen_target = lh_entity_event_get_target(event);
    g_seen_current = lh_entity_event_get_current(event);
    g_seen_param = lh_entity_event_get_param(event);
}

LH_COMPILER_EXTERN_C_END

TEST_F(Entity, bubbled_event_knows_target_and_current)
{
    lh_entity_t *panel = lh_entity_create(&lh_entity_base_class, root);
    lh_entity_t *b = lh_entity_create(&lh_entity_base_class, panel);
    lh_entity_add_flags(b, lh_entity_flags_event_bubble);
    lh_entity_add_handler(panel, recording_handler, lh_null);
    int param = 5;

    lh_entity_send_event(b, LH_ENTITY_EVENT_USER + 1, &param);
    EXPECT_EQ(g_seen_target, b);
    EXPECT_EQ(g_seen_current, panel);
    EXPECT_EQ(g_seen_param, &param);
}

TEST_F(Entity, stop_ends_handlers_and_bubbling)
{
    lh_entity_t *panel = lh_entity_create(&lh_entity_base_class, root);
    lh_entity_t *b = lh_entity_create(&lh_entity_base_class, panel);
    lh_entity_add_flags(b, lh_entity_flags_event_bubble);
    lh_entity_add_handler(panel, log_handler, const_cast<char *>("P"));
    lh_entity_add_handler(b, stopping_handler, lh_null);
    lh_entity_add_handler(b, log_handler, const_cast<char *>("X"));

    EXPECT_TRUE(lh_entity_send_event(b, LH_ENTITY_EVENT_USER, lh_null));
    EXPECT_EQ(g_log, "S");
}

TEST_F(Entity, remove_handler_and_self_removal)
{
    lh_entity_t *e = lh_entity_create(&lh_entity_base_class, root);
    lh_entity_add_handler(e, self_removing_handler, lh_null);
    lh_entity_add_handler(e, log_handler, const_cast<char *>("L"));

    lh_entity_send_event(e, LH_ENTITY_EVENT_USER, lh_null);
    lh_entity_send_event(e, LH_ENTITY_EVENT_USER, lh_null);
    EXPECT_EQ(g_log, "RLL"); // removed itself after the first event

    EXPECT_TRUE(lh_entity_remove_handler(e, log_handler, const_cast<char *>("L")));
    EXPECT_FALSE(lh_entity_remove_handler(e, log_handler, const_cast<char *>("L")));
}

TEST_F(Entity, set_parent_moves_the_subtree)
{
    lh_entity_t *left = lh_entity_create(&lh_entity_base_class, root);
    lh_entity_t *right = lh_entity_create(&lh_entity_base_class, root);
    lh_entity_t *item = lh_entity_create(&lh_entity_base_class, left);
    lh_entity_create(&lh_entity_base_class, item);

    lh_entity_set_parent(item, right);
    EXPECT_EQ(lh_entity_get_parent(item), right);
    EXPECT_EQ(lh_entity_get_first_child(left), nullptr);
    EXPECT_EQ(lh_entity_get_first_child(right), item);

    lh_entity_delete(left); // item no longer goes with it
    EXPECT_EQ(lh_entity_get_first_child(right), item);

    LH_EXPECT_DEATH(lh_entity_set_parent(right, item)); // a cycle
}

TEST_F(Entity, memory_allocated_under_an_entity_goes_with_it)
{
    lh_entity_t *e = lh_entity_create(&lh_entity_base_class, root);
    const int before = heap.live;
    lh_memory_tree_alloc_child(e, 256); // e.g. a text buffer the entity owns
    EXPECT_EQ(heap.live, before + 1);
    lh_entity_delete(e);
    EXPECT_EQ(heap.live, before - 1);
}

} // namespace
