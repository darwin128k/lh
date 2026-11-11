#include <gtest/gtest.h>

#include <cstdio>
#include <string>

#include <lh/os/fs/dir.h>
#include <lh/os/loader/image.h>
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

TEST(os_loader_image, has_ext_looks_at_the_last_segment_in_any_case)
{
    std::string upper = ext();
    for (char &c : upper)
    {
        c = static_cast<char>(c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
    }
    EXPECT_TRUE(lh_os_loader_image_has_ext(path_of("mods/a" + ext()).get()));
    EXPECT_TRUE(lh_os_loader_image_has_ext(path_of("A" + upper).get()));
    EXPECT_FALSE(lh_os_loader_image_has_ext(path_of("mods/a.txt").get()));
    EXPECT_FALSE(lh_os_loader_image_has_ext(path_of("a" + ext() + "/b").get()));
    EXPECT_FALSE(lh_os_loader_image_has_ext(path_of("").get()));
}

TEST(os_loader_image, is_named_wants_the_stem_and_the_suffix_exactly)
{
    const lh_str_view_t stem = lh_str_view_lit("authum");

    EXPECT_TRUE(lh_os_loader_image_is_named(path_of("x/authum" + ext()).get(), &stem));
    EXPECT_TRUE(lh_os_loader_image_is_named(path_of("AUTHUM" + ext()).get(), &stem));
    EXPECT_FALSE(lh_os_loader_image_is_named(path_of("authumx" + ext()).get(), &stem));
    EXPECT_FALSE(lh_os_loader_image_is_named(path_of("xauthum" + ext()).get(), &stem));
    EXPECT_FALSE(lh_os_loader_image_is_named(path_of("authum").get(), &stem));
    EXPECT_FALSE(lh_os_loader_image_is_named(path_of("").get(), &stem));
}

TEST(os_loader_image, try_resolve_takes_a_file_as_it_is_and_a_folder_by_its_name)
{
    const std::string root = "lh_test_loader_image";
    const std::string plain = root + "/plain" + ext();

    ASSERT_TRUE(lh_os_fs_dir_create(path_of(root).get()));
    std::FILE *raw = std::fopen(plain.c_str(), "wb");
    ASSERT_NE(raw, nullptr);
    std::fclose(raw);

    path_of base(root);
    lh_fs_path_t out;
    lh_fs_path_init(&out);

    ASSERT_TRUE(lh_os_loader_image_try_resolve(&base.path, path_of("plain" + ext()).get(), &out));
    EXPECT_TRUE(lh_fs_path_equals(&out, path_of(plain).get(), lh_bool_false));

    ASSERT_TRUE(lh_os_loader_image_try_resolve(&base.path, path_of("mod").get(), &out));
    EXPECT_TRUE(lh_fs_path_equals(&out, path_of(root + "/mod/mod" + ext()).get(), lh_bool_false));

    ASSERT_TRUE(lh_os_loader_image_try_resolve(nullptr, path_of("a/b").get(), &out));
    EXPECT_TRUE(lh_fs_path_equals(&out, path_of("a/b/b" + ext()).get(), lh_bool_false));

    EXPECT_FALSE(lh_os_loader_image_try_resolve(&base.path, path_of("").get(), &out));

    lh_fs_path_deinit(&out);
    std::remove(plain.c_str());
    EXPECT_EQ(lh_test_rmdir(root.c_str()), 0);
}

} // namespace
