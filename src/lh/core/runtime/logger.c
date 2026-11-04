#include <lh/core/runtime/logger.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/char/map.h>

/* One object for the whole program, empty until the host fills it: the core
   has no C library and no defaults. The host fills it before the first use
   and before it starts other threads, so every thread sees the same one. */
lh_logger_t m_runtime_logger = {lh_logger_level_flags_none,
                                lh_null,
                                lh_null,
                                lh_null,
                                lh_null,
                                lh_null,
                                lh_null,
                                lh_null,
                                lh_null,
                                lh_null};

lh_logger_t *
lh_runtime_logger_get_mut(void)
{
    return lh_addr_of(m_runtime_logger);
}

const lh_logger_t *
lh_runtime_logger_get(void)
{
    return lh_addr_of(m_runtime_logger);
}
