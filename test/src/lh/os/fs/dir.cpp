#include <gtest/gtest.h>

#include <cstdio>

#include <lh/os/fs/dir.h>
#include <lh/os/system/fs/path.h>

#if defined(_WIN32)
#    include <direct.h>
#    define lh_test_rmdir _rmdir
#else
#    include <unistd.h>
#    define lh_test_rmdir rmdir
#endif

namespace
{

struct path_of
{
    lh_fs_path_t path;

    explicit path_of(const char *text)
    {
        lh_str_view_t view;
        lh_fs_path_init(&path);
        lh_str_view_init(&view, text);
        lh_fs_path_set(&path, view, lh_os_system_fs_path_style_native());
    }

    ~path_of()
    {
        lh_fs_path_deinit(&path);
    }
};

const char *const k_dir = "lh_test_os_fs_dir_create";
const char *const k_file = "lh_test_os_fs_dir_create.file";

TEST(os_fs_dir, create_makes_a_directory_and_accepts_one_already_there)
{
    path_of p(k_dir);

    lh_test_rmdir(k_dir);
    ASSERT_TRUE(lh_os_fs_dir_create(&p.path));
    EXPECT_TRUE(lh_os_fs_dir_create(&p.path)); /* already there */

    lh_os_fs_dir_t dir;
    lh_os_fs_dir_init(&dir);
    EXPECT_TRUE(lh_os_fs_dir_open(&dir, &p.path));
    lh_os_fs_dir_close(&dir);
    EXPECT_EQ(lh_test_rmdir(k_dir), 0);
}

TEST(os_fs_dir, create_tree_makes_every_missing_parent)
{
    path_of leaf("lh_test_os_fs_tree/a/b");

    lh_test_rmdir("lh_test_os_fs_tree/a/b");
    lh_test_rmdir("lh_test_os_fs_tree/a");
    lh_test_rmdir("lh_test_os_fs_tree");
    ASSERT_TRUE(lh_os_fs_dir_create_tree(&leaf.path));
    EXPECT_TRUE(lh_os_fs_dir_create_tree(&leaf.path)); /* already there */
    EXPECT_EQ(lh_test_rmdir("lh_test_os_fs_tree/a/b"), 0);
    EXPECT_EQ(lh_test_rmdir("lh_test_os_fs_tree/a"), 0);
    EXPECT_EQ(lh_test_rmdir("lh_test_os_fs_tree"), 0);
}

TEST(os_fs_dir, create_fails_when_a_file_is_in_the_way)
{
    path_of p(k_file);
    std::FILE *const f = std::fopen(k_file, "w");

    ASSERT_NE(f, nullptr);
    std::fclose(f);
    EXPECT_FALSE(lh_os_fs_dir_create(&p.path));
    std::remove(k_file);
}

} // namespace
