#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/os/shared.h>
#include <lh/os/shared/module.h>
#include <lh/util/addr.h>

namespace
{

struct derived_module
{
    lh_os_shared_module_t shared;
    int extra;
};

char in_image;

} // namespace

TEST(os_shared_module, init_deinit_empty)
{
    lh_os_shared_module_t module;

    lh_os_shared_module_init(lh_addr_of(module), sizeof(module));
    EXPECT_EQ(lh_os_shared_module_is_loaded(lh_addr_of(module)), lh_bool_false);
    EXPECT_EQ(lh_os_shared_module_get_loaded(lh_addr_of(module)), 0U);
    EXPECT_EQ(lh_os_shared_module_get(lh_addr_of(module), 0), lh_null);
    lh_os_shared_module_deinit(lh_addr_of(module));
}

TEST(os_shared_module, bind_this_image)
{
    lh_os_shared_module_t module;

    lh_os_shared_module_init(lh_addr_of(module), sizeof(module));
    ASSERT_EQ(lh_os_shared_module_bind(lh_addr_of(module), lh_addr_of(in_image)),
              lh_bool_true);
    EXPECT_EQ(lh_os_shared_module_is_loaded(lh_addr_of(module)), lh_bool_true);
    lh_os_shared_module_deinit(lh_addr_of(module));
    EXPECT_EQ(lh_os_shared_module_is_loaded(lh_addr_of(module)), lh_bool_false);
}

TEST(os_shared_module, open_missing_fails)
{
    lh_os_shared_module_t module;

    lh_os_shared_module_init(lh_addr_of(module), sizeof(module));
    EXPECT_EQ(lh_os_shared_module_open(lh_addr_of(module), "no-such-authum-module.dll"),
              lh_bool_false);
    EXPECT_EQ(lh_os_shared_module_is_loaded(lh_addr_of(module)), lh_bool_false);
    lh_os_shared_module_deinit(lh_addr_of(module));
}

TEST(os_shared_module, derived_child_keeps_extra)
{
    derived_module parent;
    derived_module child;
    derived_module *got;

    lh_os_shared_module_init(lh_addr_of(parent.shared), sizeof(parent));
    lh_os_shared_module_init(lh_addr_of(child.shared), sizeof(child));
    child.extra = 42;
    lh_os_shared_module_push(lh_addr_of(parent.shared), lh_addr_of(child.shared));
    EXPECT_EQ(lh_os_shared_module_get_loaded(lh_addr_of(parent.shared)), 1U);
    got = reinterpret_cast<derived_module *>(
        lh_os_shared_module_get(lh_addr_of(parent.shared), 0));
    ASSERT_TRUE(got != lh_null);
    EXPECT_EQ(got->extra, 42);
    lh_os_shared_module_deinit(lh_addr_of(parent.shared));
}

TEST(os_shared_module, get_sym_unloaded)
{
    lh_os_shared_module_t module;

    lh_os_shared_module_init(lh_addr_of(module), sizeof(module));
    EXPECT_EQ(lh_os_shared_module_get_sym(lh_addr_of(module), "x"), lh_null);
    lh_os_shared_module_deinit(lh_addr_of(module));
}
