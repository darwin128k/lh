/**
 * @file entity.cpp
 * @brief Tests for `lh/gx/entity.h`.
 *
 * The entity never remembers that it is invalid: when something of it
 * changes, it reports where it was and where it is now. These tests are
 * about that report — that it happens, that it carries the old and the new
 * place, and that an entity with nobody listening says nothing.
 */

#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gx/entity.h>
#include <lh/gx/rect.h>
#include <lh/gx/scalar.h>
#include <lh/test/out.h>

namespace
{

/** What the listener heard. */
struct heard
{
    lh_u32_t count;
    lh_gx_rect_t before;
    lh_gx_rect_t after;
};

/** The listener itself: it remembers what it heard, the entity does not. */
void
on_change(lh_ptr context, const lh_gx_rect_t *before, const lh_gx_rect_t *after)
{
    heard *h = lh_ptr_rcast(heard, context);

    ++h->count;
    h->before = *before;
    h->after = *after;
}

TEST(gx_entity, a_new_entity_is_visible_and_empty)
{
    lh_gx_entity_t entity;

    lh_gx_entity_init(lh_addr_of(entity), lh_gx_entity_kind_label);

    EXPECT_EQ(lh_gx_entity_get_kind(lh_addr_of(entity)), lh_gx_entity_kind_label);
    EXPECT_EQ(lh_gx_entity_get_flags(lh_addr_of(entity)),
              LH_GX_ENTITY_FLAG_VISIBLE | LH_GX_ENTITY_FLAG_ENABLED);
    EXPECT_EQ(lh_gx_rect_is_empty(lh_gx_entity_get_rect(lh_addr_of(entity))), lh_bool_true);
    EXPECT_EQ(lh_gx_entity_get_parent(lh_addr_of(entity)), LH_GX_ENTITY_NO_LINK);
    EXPECT_EQ(lh_gx_entity_get_first_child(lh_addr_of(entity)), LH_GX_ENTITY_NO_LINK);
    EXPECT_EQ(lh_gx_entity_get_child_count(lh_addr_of(entity)), 0u);
    EXPECT_EQ(lh_gx_entity_get_next(lh_addr_of(entity)), LH_GX_ENTITY_NO_LINK);
    EXPECT_EQ(lh_gx_entity_get_user(lh_addr_of(entity)), lh_null);
}

TEST(gx_entity, an_entity_without_a_listener_says_nothing)
{
    lh_gx_entity_t entity;

    lh_gx_entity_init(lh_addr_of(entity), lh_gx_entity_kind_label);
    lh_gx_rect_t rect;

    lh_gx_rect_init(lh_addr_of(rect), 10, 20, 30, 40);
    lh_gx_entity_set_rect(lh_addr_of(entity), lh_addr_of(rect));

    EXPECT_EQ(lh_gx_rect_eq(lh_gx_entity_get_rect(lh_addr_of(entity)), lh_addr_of(rect)),
              lh_bool_true);
}

TEST(gx_entity, a_moved_entity_reports_where_it_was_and_where_it_is)
{
    lh_gx_entity_t entity;
    heard h = {0u, {}, {}};
    lh_gx_rect_t rect;

    lh_gx_entity_init(lh_addr_of(entity), lh_gx_entity_kind_label);
    lh_gx_entity_set_damage_fn(lh_addr_of(entity), on_change);
    lh_gx_entity_set_damage_context(lh_addr_of(entity), lh_addr_of(h));

    lh_gx_rect_init(lh_addr_of(rect), 10, 20, 30, 40);
    lh_gx_entity_set_rect(lh_addr_of(entity), lh_addr_of(rect));

    EXPECT_EQ(h.count, 1u);
    EXPECT_EQ(lh_gx_rect_is_empty(lh_addr_of(h.before)), lh_bool_true);
    EXPECT_EQ(lh_gx_rect_eq(lh_addr_of(h.after), lh_addr_of(rect)), lh_bool_true);
}

TEST(gx_entity, a_changed_look_reports_the_place_it_stays_at)
{
    lh_gx_entity_t entity;
    heard h = {0u, {}, {}};
    lh_gx_rect_t rect;

    lh_gx_entity_init(lh_addr_of(entity), lh_gx_entity_kind_label);
    lh_gx_rect_init(lh_addr_of(rect), 10, 20, 30, 40);
    lh_gx_entity_set_rect(lh_addr_of(entity), lh_addr_of(rect));

    lh_gx_entity_set_damage_fn(lh_addr_of(entity), on_change);
    lh_gx_entity_set_damage_context(lh_addr_of(entity), lh_addr_of(h));

    lh_gx_entity_set_flags(lh_addr_of(entity), LH_GX_ENTITY_FLAG_VISIBLE);
    EXPECT_EQ(h.count, 1u);
    EXPECT_EQ(lh_gx_rect_eq(lh_addr_of(h.before), lh_addr_of(rect)), lh_bool_true);
    EXPECT_EQ(lh_gx_rect_eq(lh_addr_of(h.after), lh_addr_of(rect)), lh_bool_true);

    lh_gx_entity_set_style(lh_addr_of(entity), 3U);
    EXPECT_EQ(h.count, 2u);
    EXPECT_EQ(lh_gx_entity_get_style(lh_addr_of(entity)), 3U);

    lh_gx_entity_set_kind(lh_addr_of(entity), lh_gx_entity_kind_button);
    EXPECT_EQ(h.count, 3u);
    EXPECT_EQ(lh_gx_entity_get_kind(lh_addr_of(entity)), lh_gx_entity_kind_button);
}

TEST(gx_entity, what_an_entity_says_is_borrowed)
{
    lh_gx_entity_t entity;

    lh_gx_entity_init(lh_addr_of(entity), lh_gx_entity_kind_label);
    lh_gx_entity_set_text_cstr(lh_addr_of(entity), "Hello");

    EXPECT_EQ(lh_str_view_get_size(lh_gx_entity_get_text(lh_addr_of(entity))), 5u);
}

} // namespace
