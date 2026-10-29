#include <gtest/gtest.h>

#include <lh/os/system/fs/file.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system.h>

#if defined(_WIN32)

#    include <cstdio>
#    include <direct.h>

/* Handle-relative lookup on Windows: NtCreateFile relative to each directory. */
TEST(os_fs_file_at, opens_a_file_below_the_root_and_refuses_escapes)
{
    lh_fs_size_t size = 123U;

    _mkdir("lh-test-file-at");
    _mkdir("lh-test-file-at/a");
    std::FILE *file = std::fopen("lh-test-file-at/a/b.txt", "wb");
    ASSERT_NE(file, nullptr);
    std::fputs("hello", file);
    std::fclose(file);

    const lh_os_system_fs_file_handle_t root = lh_os_system_fs_file_open_root("lh-test-file-at");
    ASSERT_NE(root, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    const lh_os_system_fs_file_handle_t found =
        lh_os_system_fs_file_open_read_at(root, "a/b.txt", &size);
    ASSERT_NE(found, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(size, 5U);
    lh_os_system_fs_file_close(found);

    size = 123U;
    EXPECT_EQ(lh_os_system_fs_file_open_read_at(root, "../lh-test-file-at/a/b.txt", &size),
              LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(lh_os_system_fs_file_open_read_at(root, "a\b.txt", &size),
              LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(lh_os_system_fs_file_open_read_at(root, "a/missing.txt", &size),
              LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    /* A directory is not the file at the end. */
    EXPECT_EQ(lh_os_system_fs_file_open_read_at(root, "a", &size),
              LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(size, 123U);
    EXPECT_EQ(lh_os_system_fs_file_open_read_at(LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID, "a/b.txt",
                                                &size),
              LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    lh_os_system_fs_file_close(root);
    std::remove("lh-test-file-at/a/b.txt");
    _rmdir("lh-test-file-at/a");
    _rmdir("lh-test-file-at");
}

#else

#    include <cstdlib>
#    include <cstdio>
#    include <string>
#    include <fcntl.h>
#    include <sys/stat.h>
#    include <unistd.h>

namespace
{
struct os_fs_file_at : testing::Test
{
    lh_char_t directory[64] = "/tmp/lh-file-at-XXXXXX";
    int root = -1;

    std::string
    path(const lh_char_t *name) const
    {
        return std::string(directory) + "/" + name;
    }

    lh_void
    SetUp() override
    {
        ASSERT_NE(mkdtemp(directory), nullptr);
        root = open(directory, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
        ASSERT_GE(root, 0);
        ASSERT_EQ(mkdir(path("nested").c_str(), 0700), 0);
        std::FILE *file = std::fopen(path("nested/data").c_str(), "wb");
        ASSERT_NE(file, nullptr);
        EXPECT_EQ(std::fwrite("hello", 1, 5, file), 5U);
        ASSERT_EQ(std::fclose(file), 0);
        ASSERT_EQ(symlink("nested/data", path("link").c_str()), 0);
        ASSERT_EQ(symlink("nested", path("alias").c_str()), 0);
        ASSERT_EQ(symlink("/tmp", path("outside").c_str()), 0);
        ASSERT_EQ(mkfifo(path("fifo").c_str(), 0600), 0);
        ASSERT_EQ(link(path("nested/data").c_str(), path(".hidden").c_str()), 0);
    }

    lh_void
    TearDown() override
    {
        if (root >= 0)
        {
            close(root);
        }
        for (const lh_char_t *name : {"nested/data", "link", "alias", "outside", "fifo", ".hidden"})
        {
            unlink(path(name).c_str());
        }
        rmdir(path("nested").c_str());
        rmdir(directory);
    }
};

TEST_F(os_fs_file_at, opens_regular_file_and_borrows_root)
{
    lh_fs_size_t size = 99U;
    const auto file = lh_os_system_fs_file_open_read_at(root, "nested/data", &size);
    ASSERT_NE(file, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(size, 5U);
    lh_char_t data[6] = {};
    EXPECT_EQ(lh_os_system_fs_file_read(file, data, 5U), 5);
    EXPECT_STREQ(data, "hello");
    EXPECT_NE(fcntl(lh_cast_static(int, file), F_GETFD) & FD_CLOEXEC, 0);
    lh_os_system_fs_file_close(file);
    EXPECT_GE(fcntl(root, F_GETFD), 0);

    /* Hidden names are caller policy, not traversal. */
    const auto hidden = lh_os_system_fs_file_open_read_at(root, ".hidden", &size);
    ASSERT_NE(hidden, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    lh_os_system_fs_file_close(hidden);
}

TEST_F(os_fs_file_at, rejects_traversal_links_and_special_files_without_changing_size)
{
    const lh_char_t *paths[] = {"",
                                "/nested/data",
                                "../data",
                                "nested/../nested/data",
                                "nested/./data",
                                "nested//data",
                                "nested/data/",
                                "nested/",
                                "nested",
                                "link",
                                "alias/data",
                                "outside/anything",
                                "fifo",
                                "missing",
                                "nested/missing",
                                "nested/data/child",
                                "nested\\data",
                                "C:data"};
    for (const auto name : paths)
    {
        lh_fs_size_t size = 99U;
        EXPECT_EQ(lh_os_system_fs_file_open_read_at(root, name, &size),
                  LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID)
            << name;
        EXPECT_EQ(size, 99U) << name;
        EXPECT_GE(fcntl(root, F_GETFD), 0) << name;
    }
    lh_fs_size_t size = 99U;
    EXPECT_EQ(lh_os_system_fs_file_open_read_at(-1, "nested/data", &size),
              LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(size, 99U);
    EXPECT_EQ(lh_os_get_last_error_code(), lh_os_error_code_invalid_argument);
    EXPECT_EQ(lh_os_system_fs_file_open_read_at(AT_FDCWD, "nested/data", &size),
              LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    const std::string long_name(4096, 'a');
    EXPECT_EQ(lh_os_system_fs_file_open_read_at(root, long_name.c_str(), &size),
              LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(size, 99U);
}

TEST_F(os_fs_file_at, follows_the_open_root_after_its_path_is_renamed)
{
    const std::string moved = std::string(directory) + "-moved";
    ASSERT_EQ(rename(directory, moved.c_str()), 0);
    lh_fs_size_t size = 0;
    const auto file = lh_os_system_fs_file_open_read_at(root, "nested/data", &size);
    EXPECT_NE(file, LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID);
    EXPECT_EQ(size, 5U);
    if (file != LH_OS_SYSTEM_FS_FILE_HANDLE_INVALID)
    {
        lh_os_system_fs_file_close(file);
    }
    EXPECT_EQ(rename(moved.c_str(), directory), 0);
}
} // namespace
#endif

#include <lh/os/fs/file.h>

namespace
{

/* Appends from two opens land one after the other; the file then goes. */
TEST(os_fs_file, append_adds_at_the_end_and_remove_deletes)
{
    const lh_char_t *name = "lh-test-append.log";
    lh_os_fs_file_t file;
    lh_char_t text[8] = {};
    lh_fs_path_t path;
    lh_str_view_t view;

    lh_os_fs_file_init(&file);
    ASSERT_TRUE(lh_os_fs_file_open_cstr(&file, name, lh_fs_file_mode_write));
    lh_os_fs_file_close(&file);
    ASSERT_TRUE(lh_os_fs_file_open_cstr(&file, name, lh_fs_file_mode_append));
    ASSERT_TRUE(lh_os_fs_file_write_all(&file, "ab", 2U));
    lh_os_fs_file_close(&file);
    ASSERT_TRUE(lh_os_fs_file_open_cstr(&file, name, lh_fs_file_mode_append));
    ASSERT_TRUE(lh_os_fs_file_write_all(&file, "cd", 2U));
    lh_os_fs_file_close(&file);
    ASSERT_TRUE(lh_os_fs_file_open_cstr(&file, name, lh_fs_file_mode_read));
    EXPECT_EQ(lh_os_fs_file_read(&file, text, sizeof(text)), 4);
    lh_os_fs_file_close(&file);
    lh_os_fs_file_deinit(&file);
    EXPECT_STREQ(text, "abcd");

    lh_fs_path_init(&path);
    lh_str_view_init(&view, name);
    lh_fs_path_set(&path, view, lh_fs_path_style_posix);
    EXPECT_TRUE(lh_os_fs_file_remove(&path));
    EXPECT_FALSE(lh_os_fs_file_remove(&path));
    lh_fs_path_deinit(&path);
}

} // namespace
