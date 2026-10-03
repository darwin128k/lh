#include <gtest/gtest.h>

#include <lh/memory.h>
#include <lh/null.h>

#include <cstddef>

#if defined(_WIN32)
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <sys/mman.h>
#    include <unistd.h>
#endif

namespace
{

// Two pages, the second one inaccessible: a scan that reads past the
// terminator into it faults. lh_memory_scan has no haystack length, so block
// reads must stay within the page the terminator is on.
class GuardedPage
{
public:
    GuardedPage()
    {
#if defined(_WIN32)
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        m_page = static_cast<std::size_t>(info.dwPageSize);
        m_base = static_cast<unsigned char *>(
            VirtualAlloc(nullptr, 2 * m_page, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
        DWORD old;
        m_ok = m_base != nullptr && VirtualProtect(m_base + m_page, m_page, PAGE_NOACCESS, &old);
#else
        m_page = static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
        void *p = mmap(nullptr, 2 * m_page, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        m_base = p == MAP_FAILED ? nullptr : static_cast<unsigned char *>(p);
        m_ok = m_base != nullptr && mprotect(m_base + m_page, m_page, PROT_NONE) == 0;
#endif
    }

    ~GuardedPage()
    {
        if (m_base == nullptr)
        {
            return;
        }
#if defined(_WIN32)
        VirtualFree(m_base, 0, MEM_RELEASE);
#else
        munmap(m_base, 2 * m_page);
#endif
    }

    GuardedPage(const GuardedPage &) = delete;
    GuardedPage &operator=(const GuardedPage &) = delete;

    bool ok() const { return m_ok; }

    // Start of the last @p n bytes of the accessible page.
    unsigned char *tail(std::size_t n) { return m_base + m_page - n; }

private:
    unsigned char *m_base = nullptr;
    std::size_t m_page = 0;
    bool m_ok = false;
};

TEST(memory_scan_page, byte_scan_stops_before_guard_page)
{
    GuardedPage page;
    ASSERT_TRUE(page.ok());

    // Every terminator position within the last block, including the very
    // last byte of the page.
    for (std::size_t len = 1; len <= 40; ++len)
    {
        unsigned char *s = page.tail(len);
        for (std::size_t i = 0; i + 1 < len; ++i)
        {
            s[i] = 'a';
        }
        s[len - 1] = 0;

        const lh_byte_t nul = 0;
        EXPECT_EQ(lh_memory_scan(s, &nul, 1), static_cast<const lh_ptr>(s + len - 1)) << len;
    }
}

TEST(memory_scan_page, wide_scan_stops_before_guard_page)
{
    GuardedPage page;
    ASSERT_TRUE(page.ok());

    const lh_byte_t nul2[2] = {0, 0};
    for (std::size_t units = 1; units <= 20; ++units)
    {
        unsigned char *s = page.tail(2 * units);
        for (std::size_t i = 0; i + 2 < 2 * units; i += 2)
        {
            s[i] = 'a';
            s[i + 1] = 0;
        }
        s[2 * units - 2] = 0;
        s[2 * units - 1] = 0;

        EXPECT_EQ(lh_memory_scan_step(s, nul2, 2, 2),
                  static_cast<const lh_ptr>(s + 2 * units - 2))
            << units;
    }
}

} // namespace
