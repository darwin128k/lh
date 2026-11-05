#include <lh/rail/system/cpu.h>
#include <lh/core/numeric/fixed/types.h>

lh_usize_t
lh_rail_system_cpu_mask_interrupts(void)
{
    lh_u32_t primask;
    __asm__ volatile("mrs %0, primask" : "=r"(primask)::"memory");
    __asm__ volatile("cpsid i" ::: "memory");
    return primask;
}

lh_void
lh_rail_system_cpu_restore_interrupts(lh_usize_t state)
{
    __asm__ volatile("msr primask, %0" ::"r"(state) : "memory");
}

lh_void
lh_rail_system_cpu_wait_for_interrupt(void)
{
    __asm__ volatile("dsb\n\twfi" ::: "memory");
}
