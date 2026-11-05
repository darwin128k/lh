#include <lh/rail/system/sim/cpu.h>

/* The model's one core: its mask and its one interrupt line. */
lh_usize_t m_rail_system_sim_cpu_mask = 0U;
lh_rail_system_sim_cpu_interrupt_cb m_rail_system_sim_cpu_interrupt = lh_null;
lh_ptr m_rail_system_sim_cpu_interrupt_user = lh_null;

lh_rail_system_sim_cpu_interrupt_cb
lh_rail_system_sim_cpu_get_interrupt(void)
{
    return m_rail_system_sim_cpu_interrupt;
}

lh_ptr
lh_rail_system_sim_cpu_get_interrupt_user(void)
{
    return m_rail_system_sim_cpu_interrupt_user;
}

lh_usize_t
lh_rail_system_sim_cpu_get_mask(void)
{
    return m_rail_system_sim_cpu_mask;
}

lh_void
lh_rail_system_sim_cpu_set_mask(lh_usize_t mask)
{
    m_rail_system_sim_cpu_mask = mask;
}

lh_void
lh_rail_system_sim_cpu_set_interrupt(lh_rail_system_sim_cpu_interrupt_cb interrupt, lh_ptr user)
{
    m_rail_system_sim_cpu_interrupt = interrupt;
    m_rail_system_sim_cpu_interrupt_user = user;
}

lh_bool_t
lh_rail_system_sim_cpu_is_masked(void)
{
    return lh_math_ne(lh_rail_system_sim_cpu_get_mask(), 0U);
}

lh_usize_t
lh_rail_system_cpu_mask_interrupts(void)
{
    const lh_usize_t before = lh_rail_system_sim_cpu_get_mask();
    lh_rail_system_sim_cpu_set_mask(1U);
    return before;
}

lh_void
lh_rail_system_cpu_restore_interrupts(lh_usize_t state)
{
    lh_rail_system_sim_cpu_set_mask(state);
}

lh_void
lh_rail_system_cpu_wait_for_interrupt(void)
{
    lh_rail_system_sim_cpu_interrupt_cb interrupt = lh_rail_system_sim_cpu_get_interrupt();
    if (lh_null_eq(interrupt))
    {
        return;
    }
    /* The pending interrupt runs as soon as it is unmasked, then the mask is back. */
    const lh_usize_t mask = lh_rail_system_sim_cpu_get_mask();
    lh_rail_system_sim_cpu_set_mask(0U);
    interrupt(lh_rail_system_sim_cpu_get_interrupt_user());
    lh_rail_system_sim_cpu_set_mask(mask);
}
