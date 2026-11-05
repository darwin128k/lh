#include <lh/core/runtime/critical.h>
#include <lh/core/critical/initializer.h>

/* One section for the whole program, empty until the host fills it: the core
   knows no platform. The host fills it before anything can come between. */
lh_critical_t m_runtime_critical = lh_critical_empty_initializer();

lh_critical_t *
lh_runtime_critical_get_mut(void)
{
    return lh_addr_of(m_runtime_critical);
}

const lh_critical_t *
lh_runtime_critical_get(void)
{
    return lh_addr_of(m_runtime_critical);
}

lh_usize_t
lh_runtime_critical_enter(void)
{
    return lh_critical_enter(lh_runtime_critical_get());
}

lh_void
lh_runtime_critical_leave(lh_usize_t state)
{
    lh_critical_leave(lh_runtime_critical_get(), state);
}
