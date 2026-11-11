#include <gtest/gtest.h>

#include <lh/core/str/template.h>
#include <lh/core/str/template/registry.h>

#include <string>

namespace
{

lh_str_view_t
view(const char *text)
{
    lh_str_view_t out;

    lh_str_view_init(&out, text);
    return out;
}

bool
add(lh_str_template_registry_t *registry, lh_ptr owner, const char *name, const char *value)
{
    const lh_str_view_t name_view = view(name);
    const lh_str_view_t value_view = view(value);

    return lh_str_template_registry_try_add(registry, owner, &name_view, &value_view) !=
           lh_bool_false;
}

bool
remove(lh_str_template_registry_t *registry, lh_ptr owner, const char *name)
{
    const lh_str_view_t name_view = view(name);

    return lh_str_template_registry_try_remove(registry, owner, &name_view) != lh_bool_false;
}

std::string
value_of(const lh_str_template_registry_t *registry, const char *name)
{
    const lh_str_view_t name_view = view(name);
    lh_str_view_t value;

    if (!lh_str_template_registry_try_get_value(registry, &name_view, &value))
    {
        return "<none>";
    }
    return std::string(lh_str_view_get_data(&value), lh_str_view_get_size(&value));
}

} // namespace

TEST(lh_str_template_registry, names_are_unique_and_valid)
{
    lh_str_template_registry_t registry;
    int a = 0;
    int b = 0;

    lh_str_template_registry_init(&registry);
    EXPECT_TRUE(add(&registry, &a, "maps", "/srv/maps"));
    EXPECT_FALSE(add(&registry, &b, "maps", "/elsewhere"));
    EXPECT_FALSE(add(&registry, &a, "bad-name", "/x"));
    EXPECT_FALSE(add(&registry, &a, "", "/x"));
    EXPECT_EQ(lh_str_template_registry_get_count(&registry), 1U);
    EXPECT_EQ(value_of(&registry, "maps"), "/srv/maps");
    EXPECT_EQ(value_of(&registry, "Maps"), "<none>");
    lh_str_template_registry_deinit(&registry);
}

TEST(lh_str_template_registry, only_the_owner_removes_by_name)
{
    lh_str_template_registry_t registry;
    int a = 0;
    int b = 0;

    lh_str_template_registry_init(&registry);
    EXPECT_TRUE(add(&registry, &a, "maps", "/srv/maps"));
    EXPECT_FALSE(remove(&registry, &b, "maps"));
    EXPECT_FALSE(remove(&registry, &a, "sounds"));
    EXPECT_TRUE(remove(&registry, &a, "maps"));
    EXPECT_EQ(value_of(&registry, "maps"), "<none>");
    lh_str_template_registry_deinit(&registry);
}

TEST(lh_str_template_registry, remove_owner_takes_all_of_theirs)
{
    lh_str_template_registry_t registry;
    int a = 0;
    int b = 0;

    lh_str_template_registry_init(&registry);
    EXPECT_TRUE(add(&registry, &a, "one", "1"));
    EXPECT_TRUE(add(&registry, &b, "two", "2"));
    EXPECT_TRUE(add(&registry, &a, "three", "3"));
    lh_str_template_registry_remove_owner(&registry, &a);
    EXPECT_EQ(lh_str_template_registry_get_count(&registry), 1U);
    EXPECT_EQ(value_of(&registry, "two"), "2");
    EXPECT_EQ(value_of(&registry, "one"), "<none>");
    lh_str_template_registry_deinit(&registry);
}

TEST(lh_str_template_registry, feeds_an_expansion)
{
    lh_str_template_registry_t registry;
    int a = 0;
    lh_str_t out;
    const lh_str_view_t pattern = view("{root}/{sub}");

    lh_str_template_registry_init(&registry);
    lh_str_init(&out);
    EXPECT_TRUE(add(&registry, &a, "root", "/srv"));
    EXPECT_TRUE(add(&registry, &a, "sub", "data"));
    ASSERT_TRUE(
        lh_str_template_try_expand(&pattern, lh_str_template_registry_lookup, &registry, &out));
    EXPECT_EQ(std::string(lh_str_get_data(&out), lh_str_get_size(&out)), "/srv/data");
    lh_str_deinit(&out);
    lh_str_template_registry_deinit(&registry);
}
