#include <gtest/gtest.h>

#include <lh/os/shared.h>
#include <lh/os/system/fs/path.h>

#if defined(_WIN32)
#    include <windows.h>
#else
#    include <unistd.h>
#endif

#if !defined(_WIN32)
/* Hidden: in the executable's .symtab, not among its exports. */
extern "C" __attribute__((noinline, used, visibility("hidden"))) int
lh_test_os_shared_local(void)
{
    return 42;
}
#endif

namespace
{

/* A system library every process of this OS can open, and a function in it
   whose answer can be checked against the direct call. */
#if defined(_WIN32)
const char *const k_library = "kernel32.dll";
const char *const k_function = "GetCurrentProcessId";
typedef DWORD(WINAPI process_id_fn)(void);
unsigned long
direct_process_id()
{
    return GetCurrentProcessId();
}
#else
const char *const k_library = "libc.so.6";
const char *const k_function = "getpid";
typedef pid_t(process_id_fn)(void);
unsigned long
direct_process_id()
{
    return static_cast<unsigned long>(getpid());
}
#endif

struct opened
{
    lh_os_shared_t image;
    bool ok;

    opened()
    {
        lh_fs_path_t path;
        lh_str_view_t name;

        lh_os_shared_init(&image);
        lh_fs_path_init(&path);
        lh_str_view_init(&name, k_library);
        lh_fs_path_set(&path, name, lh_os_system_fs_path_style_native());
        ok = lh_os_shared_open(&image, &path);
        lh_fs_path_deinit(&path);
    }

    ~opened()
    {
        lh_os_shared_deinit(&image);
    }
};

TEST(os_shared, get_fn_hands_back_a_callable_function)
{
    opened o;
    ASSERT_TRUE(o.ok) << k_library;

    const lh_os_shared_fn_t fn = lh_os_shared_get_fn(&o.image, k_function);
    ASSERT_NE(fn, nullptr);
    process_id_fn *process_id = reinterpret_cast<process_id_fn *>(fn);
    EXPECT_EQ(static_cast<unsigned long>(process_id()), direct_process_id());
}

TEST(os_shared, get_fn_is_null_for_a_missing_name_or_a_closed_image)
{
    opened o;
    ASSERT_TRUE(o.ok) << k_library;
    EXPECT_EQ(lh_os_shared_get_fn(&o.image, "lh_no_such_function_anywhere"), nullptr);

    lh_os_shared_close(&o.image);
    EXPECT_EQ(lh_os_shared_get_fn(&o.image, k_function), nullptr);
}


TEST(os_shared, bind_name_takes_an_image_already_loaded)
{
    lh_os_shared_t image;

    /* The test links the C runtime / kernel32, so the image is in the process. */
    lh_os_shared_init(&image);
    ASSERT_TRUE(lh_os_shared_bind_name(&image, k_library)) << k_library;
    EXPECT_NE(lh_os_shared_get_sym(&image, k_function), nullptr);
    EXPECT_TRUE(lh_os_shared_close(&image));
    lh_os_shared_deinit(&image);
}

TEST(os_shared, bind_name_never_loads)
{
    lh_os_shared_t image;

    lh_os_shared_init(&image);
    EXPECT_FALSE(lh_os_shared_bind_name(&image, "lh_no_such_library_loaded.so"));
    EXPECT_FALSE(lh_os_shared_is_open(&image));
    lh_os_shared_deinit(&image);
}

TEST(os_shared, get_local_sym_finds_what_the_image_keeps_to_itself)
{
    lh_os_shared_t image;

    lh_os_shared_init(&image);
    ASSERT_TRUE(lh_os_shared_bind_executable(&image));
#if defined(_WIN32)
    /* PE: no symbol table in the image. */
    EXPECT_EQ(lh_os_shared_get_local_sym(&image, "lh_test_os_shared_local"), nullptr);
#else
    EXPECT_EQ(lh_os_shared_get_local_sym(&image, "lh_test_os_shared_local"),
              reinterpret_cast<lh_ptr>(&lh_test_os_shared_local));
#endif
    EXPECT_EQ(lh_os_shared_get_local_sym(&image, "lh_no_such_symbol_anywhere"), nullptr);
    lh_os_shared_close(&image);
    lh_os_shared_deinit(&image);
}

} // namespace
