#include <bench/bench.h>

int
main()
{
    bench::Registry::instance().run_all();
    return 0;
}
