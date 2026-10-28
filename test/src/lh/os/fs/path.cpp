#include <gtest/gtest.h>

#include <lh/os/system/fs/path.h>
#include <lh/os/fs/path.h>
#include <lh/util/str/ptr.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#if defined(_WIN32)
#    include <windows.h>
#endif

namespace
{
namespace fs = std::filesystem;

fs::path
from_utf8(lh_str_cptr text)
{
#if defined(__cpp_char8_t)
    return fs::path(std::u8string(lh_cast_reinterpret(const char8_t *, text)));
#else
    return fs::u8path(text);
#endif
}

struct os_fs_path : testing::Test
{
    fs::path directory;
    lh_bool_t owned = lh_bool_false;
    lh_str_t result;

    lh_void
    SetUp() override
    {
        lh_str_init(&result);
        directory = fs::temp_directory_path() /
                    ("lh-canonical-" +
                     std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        owned = fs::create_directory(directory) ? lh_bool_true : lh_bool_false;
        ASSERT_TRUE(owned);
        ASSERT_TRUE(fs::create_directory(directory / "nested"));
        std::ofstream file(
            directory /
            from_utf8("\xd0\xb4\xd0\xb0\xd0\xbd\xd0\xbd\xd1\x8b\xd0\xb5\x2e\x74\x78\x74"));
        file << "data";
        ASSERT_TRUE(file.good());
    }

    lh_void
    TearDown() override
    {
        lh_str_deinit(&result);
        if (owned)
        {
            std::error_code ignored;
            fs::remove_all(directory, ignored);
        }
    }

    lh_bool_t
    canonical(const fs::path &path)
    {
        const auto text = path.u8string();
        return lh_os_system_fs_path_canonical(lh_cast_reinterpret(lh_str_cptr, text.c_str()),
                                              &result);
    }
};

TEST_F(os_fs_path, resolves_existing_relative_paths_and_dot_components)
{
    ASSERT_TRUE(canonical(directory));
    const std::string expected = lh_str_get_data(&result);
    ASSERT_TRUE(canonical(directory / "nested" / ".." / "."));
    EXPECT_EQ(lh_str_get_data(&result), expected);
    ASSERT_TRUE(lh_os_system_fs_path_canonical(".", &result));
    const fs::path resolved = from_utf8(lh_str_get_data(&result));
    EXPECT_TRUE(resolved.is_absolute());
    EXPECT_TRUE(fs::equivalent(resolved, fs::current_path()));
}

TEST_F(os_fs_path, resolves_unicode_files_and_supports_output_aliasing_input)
{
    ASSERT_TRUE(canonical(
        directory / from_utf8("\xd0\xb4\xd0\xb0\xd0\xbd\xd0\xbd\xd1\x8b\xd0\xb5\x2e\x74\x78\x74")));
    const std::string expected = lh_str_get_data(&result);
    EXPECT_TRUE(fs::equivalent(
        from_utf8(expected.c_str()),
        directory / from_utf8("\xd0\xb4\xd0\xb0\xd0\xbd\xd0\xbd\xd1\x8b\xd0\xb5\x2e\x74\x78\x74")));
    ASSERT_TRUE(lh_os_system_fs_path_canonical(lh_str_get_data(&result), &result));
    EXPECT_EQ(lh_str_get_data(&result), expected);
}

TEST_F(os_fs_path, missing_and_empty_paths_leave_the_result_unchanged)
{
    lh_str_append(&result, "sentinel", 8U);
    EXPECT_FALSE(canonical(directory / "missing" / "child"));
    EXPECT_STREQ(lh_str_get_data(&result), "sentinel");
    EXPECT_FALSE(lh_os_system_fs_path_canonical("", &result));
    EXPECT_STREQ(lh_str_get_data(&result), "sentinel");
}

TEST_F(os_fs_path, resolves_symbolic_links_to_their_targets)
{
    std::error_code error;
#if defined(_WIN32)
    /* The MinGW filesystem is built for XP and stubs out symlink creation.
     * Resolve the native API dynamically, like the backend under test. */
    using create_link_fn = BOOLEAN(WINAPI *)(LPCWSTR, LPCWSTR, DWORD);
    const auto create_link = lh_cast_reinterpret(
        create_link_fn, GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CreateSymbolicLinkW"));
    if (!create_link)
    {
        GTEST_SKIP() << "CreateSymbolicLinkW is unavailable";
    }
    const fs::path alias = directory / "alias", target = directory / "nested";
    if (!create_link(alias.c_str(), target.c_str(), 3UL))
    {
        if (GetLastError() != ERROR_INVALID_PARAMETER ||
            !create_link(alias.c_str(), target.c_str(), 1UL))
        {
            error =
                std::error_code(lh_cast_static(lh_sint_t, GetLastError()), std::system_category());
        }
    }
#else
    fs::create_directory_symlink(directory / "nested", directory / "alias", error);
#endif
    if (error)
    {
        GTEST_SKIP() << "Cannot create test symlink: " << error.message();
    }
    ASSERT_TRUE(canonical(directory / "nested"));
    const std::string expected = lh_str_get_data(&result);
    ASSERT_TRUE(canonical(directory / "alias"));
    EXPECT_EQ(lh_str_get_data(&result), expected);
}
TEST_F(os_fs_path, typed_canonicalization_supports_aliasing_and_preserves_output_on_failure)
{
    lh_fs_path_t path;
    lh_fs_path_init(&path);
    const auto native = (directory / "nested" / "..").u8string();
    lh_str_view_t view;
    lh_str_view_init(&view, lh_cast_reinterpret(lh_str_cptr, native.c_str()));
    lh_fs_path_set(&path, view, lh_os_system_fs_path_style_native());
    ASSERT_TRUE(lh_os_fs_path_canonical(&path, &path));
    EXPECT_TRUE(lh_fs_path_is_absolute(&path));
    const std::string before = lh_str_get_data(lh_fs_path_get_text(&path));
    lh_fs_path_t missing;
    lh_fs_path_init(&missing);
    lh_fs_path_set(&missing, lh_str_view_lit("lh-canonical-missing/child"), lh_fs_path_style_posix);
    EXPECT_FALSE(lh_os_fs_path_canonical(&missing, &path));
    EXPECT_EQ(lh_str_get_data(lh_fs_path_get_text(&path)), before);
    lh_fs_path_deinit(&missing);
    lh_fs_path_deinit(&path);
}

} // namespace
