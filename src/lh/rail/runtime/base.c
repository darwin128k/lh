#include <lh/rail/runtime/base.h>
#include <lh/rail/base/initializer.h>

/* One base for the whole program, empty until the host fills it: rail has no
   platform of its own. The host fills it before it starts any rail thread or
   loop, so every thread sees the same calls. */
lh_rail_base_t m_rail_runtime_base = lh_rail_base_empty_initializer();

lh_rail_base_t *
lh_rail_runtime_base_get_mut(lh_void)
{
    return lh_addr_of(m_rail_runtime_base);
}

const lh_rail_base_t *
lh_rail_runtime_base_get(lh_void)
{
    return lh_addr_of(m_rail_runtime_base);
}
