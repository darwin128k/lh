#include <gtest/gtest.h>

#include <cstdio>

#include <lh/os/fs/dir.h>
#include <lh/os/fs/stat.h>
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

TEST(os_fs_is_kind, tells_a_file_from_a_folder_and_from_nothing)
{
    path_of dir("lh_test_os_fs_is_kind");
    path_of file("lh_test_os_fs_is_kind.file");
    path_of missing("lh_test_os_fs_is_kind.missing");

    ASSERT_TRUE(lh_os_fs_dir_create(&dir.path));
    std::FILE *raw = std::fopen("lh_test_os_fs_is_kind.file", "wb");
    ASSERT_NE(raw, nullptr);
    std::fclose(raw);

    EXPECT_TRUE(lh_os_fs_is_dir(&dir.path));
    EXPECT_FALSE(lh_os_fs_is_file(&dir.path));
    EXPECT_TRUE(lh_os_fs_is_file(&file.path));
    EXPECT_FALSE(lh_os_fs_is_dir(&file.path));
    EXPECT_TRUE(lh_os_fs_is_kind(&file.path, lh_fs_kind_file));
    EXPECT_FALSE(lh_os_fs_is_file(&missing.path));
    EXPECT_FALSE(lh_os_fs_is_dir(&missing.path));

    std::remove("lh_test_os_fs_is_kind.file");
    EXPECT_EQ(lh_test_rmdir("lh_test_os_fs_is_kind"), 0);
}

} // namespace
