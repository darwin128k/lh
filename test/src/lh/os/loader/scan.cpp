#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include <lh/os/fs/dir.h>
#include <lh/os/loader/scan.h>
#include <lh/os/system/shared.h>

#if defined(_WIN32)
#    include <direct.h>
#    define lh_test_rmdir _rmdir
#else
#    include <unistd.h>
#    define lh_test_rmdir rmdir
#endif

namespace
{

std::string
ext()
{
    lh_str_view_t view;
    lh_os_system_shared_get_ext(&view);
    return std::string(lh_str_view_get_data(&view), lh_str_view_get_size(&view));
}

struct path_of
{
    lh_fs_path_t path;

    explicit path_of(const std::string &text)
    {
        lh_str_view_t view;
        lh_fs_path_init(&path);
        lh_str_view_init_by_size(&view, text.data(), text.size());
        lh_fs_path_set(&path, view, lh_fs_path_style_posix);
    }

    ~path_of()
    {
        lh_fs_path_deinit(&path);
    }

    lh_fs_path_t *
    get()
    {
        return &path;
    }
};

void
touch(const std::string &name)
{
    std::FILE *raw = std::fopen(name.c_str(), "wb");
    ASSERT_NE(raw, nullptr);
    std::fclose(raw);
}

lh_void
collect(lh_ptr user, const lh_fs_path_t *image)
{
    lh_str_t text;
    const lh_str_ptr cstr = lh_fs_path_to_cstr(image, lh_fs_path_style_posix, &text);
    static_cast<std::vector<std::string> *>(user)->push_back(cstr);
    lh_str_deinit(&text);
}

TEST(os_loader_scan, hands_every_module_folder_image_and_every_file)
{
    const std::string root = "lh_test_loader_scan";

    ASSERT_TRUE(lh_os_fs_dir_create(path_of(root).get()));
    ASSERT_TRUE(lh_os_fs_dir_create(path_of(root + "/a").get()));
    ASSERT_TRUE(lh_os_fs_dir_create(path_of(root + "/empty").get()));
    touch(root + "/a/a" + ext());
    touch(root + "/c" + ext());

    std::vector<std::string> found;
    ASSERT_TRUE(lh_os_loader_scan(path_of(root).get(), collect, &found));
    std::sort(found.begin(), found.end());
    ASSERT_EQ(found.size(), 2U);
    EXPECT_EQ(found[0], root + "/a/a" + ext());
    EXPECT_EQ(found[1], root + "/c" + ext());

    std::remove((root + "/a/a" + ext()).c_str());
    std::remove((root + "/c" + ext()).c_str());
    EXPECT_EQ(lh_test_rmdir((root + "/a").c_str()), 0);
    EXPECT_EQ(lh_test_rmdir((root + "/empty").c_str()), 0);
    EXPECT_EQ(lh_test_rmdir(root.c_str()), 0);
}

TEST(os_loader_scan, a_missing_folder_holds_nothing_and_an_empty_path_fails)
{
    std::vector<std::string> found;
    EXPECT_TRUE(lh_os_loader_scan(path_of("lh_test_loader_scan.missing").get(), collect, &found));
    EXPECT_TRUE(found.empty());
    EXPECT_FALSE(lh_os_loader_scan(path_of("").get(), collect, &found));
}

} // namespace
