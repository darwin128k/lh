/*
 * The OS layer, measured through the system calls it makes: clocks, a file written
 * and read back, a directory listed, and TCP / UDP over loopback.
 *
 * These are system numbers as much as library numbers: they say what a call through
 * lh costs on this machine, and a disk or an antivirus can move them.
 */

#include <benchmark/benchmark.h>

#include <lh/os/fs/dir.h>
#include <lh/os/fs/file.h>
#include <lh/os/fs/stat.h>
#include <lh/os/net.h>
#include <lh/os/net/socket.h>
#include <lh/os/tick.h>
#include <lh/os/timestamp.h>

#include <cstdio>
#include <vector>

namespace
{

const char *const scratch_file = "lh_bench_os.tmp";

lh_fs_path_t
path_of(const char *text)
{
    lh_fs_path_t path;
    lh_str_view_t view;
    lh_fs_path_init(&path);
    lh_str_view_init(&view, text);
#if defined(_WIN32)
    lh_fs_path_set(&path, view, lh_fs_path_style_windows);
#else
    lh_fs_path_set(&path, view, lh_fs_path_style_posix);
#endif
    return path;
}

/** Winsock wants starting before the first socket; harmless elsewhere. */
struct net_started
{
    net_started()
    {
        (void)lh_os_net_init();
    }
};
const net_started net_started_once;

lh_net_ip4_socket_addr_t
loopback(lh_net_port_t port)
{
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t addr;
    lh_net_ip4_init(&ip, 127, 0, 0, 1);
    lh_net_ip4_socket_addr_init(&addr, &ip, port);
    return addr;
}

} // namespace

/* -- Clocks ---------------------------------------------------------------- */

static void
BM_os_tick_us(benchmark::State &state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_os_tick_us());
    }
}
BENCHMARK(BM_os_tick_us);

static void
BM_os_tick_ms(benchmark::State &state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_os_tick_ms());
    }
}
BENCHMARK(BM_os_tick_ms);

static void
BM_os_timestamp_now(benchmark::State &state)
{
    lh_timestamp_t t = 0;

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_os_timestamp_now(&t));
        benchmark::DoNotOptimize(t);
    }
}
BENCHMARK(BM_os_timestamp_now);

/* -- Files ----------------------------------------------------------------- */

/** Open, write the argument's bytes, close: a whole small-file save. */
static void
BM_os_fs_file_write(benchmark::State &state)
{
    const std::size_t n = static_cast<std::size_t>(state.range(0));
    std::vector<char> data(n, 'x');
    lh_fs_path_t path = path_of(scratch_file);
    lh_os_fs_file_t file;

    lh_os_fs_file_init(&file);
    for (auto _ : state)
    {
        if (!lh_os_fs_file_open(&file, &path, lh_fs_file_mode_write))
        {
            state.SkipWithError("open for write failed");
            break;
        }
        benchmark::DoNotOptimize(lh_os_fs_file_write_all(&file, data.data(), n));
        lh_os_fs_file_close(&file);
    }
    lh_os_fs_file_deinit(&file);
    lh_fs_path_deinit(&path);
    std::remove(scratch_file);
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(n));
}
BENCHMARK(BM_os_fs_file_write)->Arg(4096)->Arg(1 << 20)->Unit(benchmark::kMicrosecond);

/** Open, read the argument's bytes back (from the page cache), close. */
static void
BM_os_fs_file_read(benchmark::State &state)
{
    const std::size_t n = static_cast<std::size_t>(state.range(0));
    std::vector<char> data(n, 'x');
    lh_fs_path_t path = path_of(scratch_file);
    lh_os_fs_file_t file;

    lh_os_fs_file_init(&file);
    if (!lh_os_fs_file_open(&file, &path, lh_fs_file_mode_write) ||
        !lh_os_fs_file_write_all(&file, data.data(), n))
    {
        state.SkipWithError("could not prepare the file");
    }
    lh_os_fs_file_close(&file);
    for (auto _ : state)
    {
        std::size_t got = 0;
        lh_os_fs_file_open(&file, &path, lh_fs_file_mode_read);
        for (;;)
        {
            const lh_ssize_t r = lh_os_fs_file_read(&file, data.data() + got, n - got);
            if (r <= 0)
            {
                break;
            }
            got += static_cast<std::size_t>(r);
        }
        lh_os_fs_file_close(&file);
        benchmark::DoNotOptimize(got);
    }
    lh_os_fs_file_deinit(&file);
    lh_fs_path_deinit(&path);
    std::remove(scratch_file);
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(n));
}
BENCHMARK(BM_os_fs_file_read)->Arg(4096)->Arg(1 << 20)->Unit(benchmark::kMicrosecond);

static void
BM_os_fs_stat(benchmark::State &state)
{
    lh_fs_path_t path = path_of(".");
    lh_fs_stat_t st;

    lh_fs_stat_init(&st);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_os_fs_stat(&path, &st));
        benchmark::DoNotOptimize(st);
    }
    lh_fs_path_deinit(&path);
}
BENCHMARK(BM_os_fs_stat);

/** The current directory listed to the end. */
static void
BM_os_fs_dir_list(benchmark::State &state)
{
    lh_fs_path_t path = path_of(".");
    lh_os_fs_dir_t dir;
    std::int64_t entries = 0;

    lh_os_fs_dir_init(&dir);
    for (auto _ : state)
    {
        lh_str_view_t name;
        lh_os_fs_dir_entry_kind_t kind;
        std::int64_t n = 0;

        if (!lh_os_fs_dir_open(&dir, &path))
        {
            state.SkipWithError("open dir failed");
            break;
        }
        while (lh_os_fs_dir_read(&dir, &name, &kind))
        {
            ++n;
        }
        lh_os_fs_dir_close(&dir);
        entries = n;
    }
    lh_fs_path_deinit(&path);
    state.counters["entries"] = static_cast<double>(entries);
}
BENCHMARK(BM_os_fs_dir_list)->Unit(benchmark::kMicrosecond);

/* -- Sockets over loopback ------------------------------------------------- */

/** One request and its echo of the argument's bytes over a connected TCP pair. */
static void
BM_os_net_tcp_roundtrip(benchmark::State &state)
{
    const std::size_t n = static_cast<std::size_t>(state.range(0));
    std::vector<char> out(n, 'q');
    std::vector<char> in(n);
    lh_os_net_socket_t listener, client, server;
    lh_net_ip4_socket_addr_t addr = loopback(0);
    lh_net_ip4_socket_addr_t bound;
    lh_net_ip4_socket_addr_t peer;

    lh_os_net_socket_init(&listener);
    lh_os_net_socket_init(&client);
    lh_os_net_socket_init(&server);
    if (!lh_os_net_socket_open(&listener, lh_os_system_net_socket_type_tcp) ||
        !lh_os_net_socket_bind(&listener, &addr) || !lh_os_net_socket_listen(&listener, 1) ||
        !lh_os_net_socket_get_local_addr(&listener, &bound) ||
        !lh_os_net_socket_open(&client, lh_os_system_net_socket_type_tcp) ||
        !lh_os_net_socket_connect(&client, &bound) || !lh_os_net_socket_accept(&listener, &server, &peer))
    {
        state.SkipWithError("loopback TCP setup failed");
    }
    lh_os_net_socket_set_no_delay(&client, lh_bool_true);
    lh_os_net_socket_set_no_delay(&server, lh_bool_true);
    for (auto _ : state)
    {
        std::size_t done = 0;
        lh_os_net_socket_send(&client, out.data(), n);
        while (done < n)
        {
            const lh_ssize_t r = lh_os_net_socket_recv(&server, in.data() + done, n - done);
            if (r <= 0)
            {
                break;
            }
            done += static_cast<std::size_t>(r);
        }
        lh_os_net_socket_send(&server, in.data(), n);
        done = 0;
        while (done < n)
        {
            const lh_ssize_t r = lh_os_net_socket_recv(&client, in.data() + done, n - done);
            if (r <= 0)
            {
                break;
            }
            done += static_cast<std::size_t>(r);
        }
        benchmark::DoNotOptimize(done);
    }
    lh_os_net_socket_close(&server);
    lh_os_net_socket_close(&client);
    lh_os_net_socket_close(&listener);
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(2 * n));
}
BENCHMARK(BM_os_net_tcp_roundtrip)->Arg(64)->Arg(4096)->Arg(65536)->Unit(benchmark::kMicrosecond);

/** One datagram of the argument's bytes sent to a bound socket and received. */
static void
BM_os_net_udp_send_recv(benchmark::State &state)
{
    const std::size_t n = static_cast<std::size_t>(state.range(0));
    std::vector<char> out(n, 'u');
    std::vector<char> in(n);
    lh_os_net_socket_t rx, tx;
    lh_net_ip4_socket_addr_t addr = loopback(0);
    lh_net_ip4_socket_addr_t bound;
    lh_net_ip4_socket_addr_t from;

    lh_os_net_socket_init(&rx);
    lh_os_net_socket_init(&tx);
    if (!lh_os_net_socket_open(&rx, lh_os_system_net_socket_type_udp) || !lh_os_net_socket_bind(&rx, &addr) ||
        !lh_os_net_socket_get_local_addr(&rx, &bound) ||
        !lh_os_net_socket_open(&tx, lh_os_system_net_socket_type_udp))
    {
        state.SkipWithError("loopback UDP setup failed");
    }
    for (auto _ : state)
    {
        lh_os_net_socket_sendto(&tx, out.data(), n, &bound);
        benchmark::DoNotOptimize(lh_os_net_socket_recvfrom(&rx, in.data(), n, &from));
    }
    lh_os_net_socket_close(&tx);
    lh_os_net_socket_close(&rx);
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(n));
}
BENCHMARK(BM_os_net_udp_send_recv)->Arg(64)->Arg(1400)->Unit(benchmark::kMicrosecond);
