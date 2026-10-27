/**
 * @file os.h
 * @brief Scoped OS resources for GoogleTest fixtures and fatal assertion cleanup.
 */
#ifndef LH_TEST_OS_H
#define LH_TEST_OS_H
#include <gtest/gtest.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/system/net/socket.h>
#include <lh/os/wake.h>
#include <lh/os/thread.h>
#include <chrono>
#include <utility>

namespace lh_test
{
template <class F> class scope_exit
{
    F action;

  public:
    explicit scope_exit(F fn) : action(std::move(fn))
    {
    }
    ~scope_exit()
    {
        action();
    }
    scope_exit(const scope_exit &) = delete;
    scope_exit &
    operator=(const scope_exit &) = delete;
};
template <class F> scope_exit(F) -> scope_exit<F>;

class network_test : public testing::Test
{
    bool initialized = false;

  protected:
    void
    SetUp() override
    {
        initialized = lh_os_net_init();
        ASSERT_TRUE(initialized);
    }
    void
    TearDown() override
    {
        if (initialized)
        {
            lh_os_net_deinit();
        }
    }
};

struct socket_owner
{
    lh_os_net_socket_t value;
    socket_owner()
    {
        lh_os_net_socket_init(&value);
    }
    ~socket_owner()
    {
        lh_os_net_socket_close(&value);
    }
    socket_owner(const socket_owner &) = delete;
    socket_owner &
    operator=(const socket_owner &) = delete;
};

struct wake_owner
{
    lh_os_wake_t value;
    wake_owner()
    {
        lh_os_wake_init(&value);
    }
    ~wake_owner()
    {
        lh_os_wake_close(&value);
    }
    wake_owner(const wake_owner &) = delete;
    wake_owner &
    operator=(const wake_owner &) = delete;
};

struct thread_owner
{
    lh_os_thread_t value;
    thread_owner()
    {
        lh_os_thread_init(&value);
    }
    ~thread_owner()
    {
        EXPECT_TRUE(lh_os_thread_join(&value));
    }
    thread_owner(const thread_owner &) = delete;
    thread_owner &
    operator=(const thread_owner &) = delete;
};

struct socket_pair
{
    lh_os_system_net_socket_handle_t handles[2] = {LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID,
                                                   LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID};
    ~socket_pair()
    {
        close(1);
        close(0);
    }
    bool
    open()
    {
        return lh_os_system_net_socket_pair(handles, 1000);
    }
    void
    close(unsigned i)
    {
        if (handles[i] != LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID)
        {
            lh_os_system_net_socket_close(handles[i]);
            handles[i] = LH_OS_SYSTEM_NET_SOCKET_HANDLE_INVALID;
        }
    }
    socket_pair() = default;
    socket_pair(const socket_pair &) = delete;
    socket_pair &
    operator=(const socket_pair &) = delete;
};

template <class Predicate>
bool
wait_until(Predicate predicate)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!predicate())
    {
        if (std::chrono::steady_clock::now() >= deadline || !lh_os_thread_sleep(1))
        {
            return false;
        }
    }
    return true;
}
} // namespace lh_test
#endif /* LH_TEST_OS_H */
