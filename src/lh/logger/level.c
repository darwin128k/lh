/**
 * @file level.c
 * @brief Names of ::lh_logger_level_t.
 */

#include <lh/logger/level.h>
#include <lh/assert/runtime.h>
#include <lh/util/return.h>
#include <lh/util/str/ptr.h>

/* The PSR-3 names in level order, then the short forms. */
static const lh_char_t *const m_logger_level_names[] = {
    "emergency", "alert", "critical", "error", "warning", "notice", "info", "debug",
    "crit",      "err",   "warn"};
static const lh_logger_level_t m_logger_level_values[] = {
    lh_logger_level_emergency, lh_logger_level_alert,   lh_logger_level_critical,
    lh_logger_level_error,     lh_logger_level_warning, lh_logger_level_notice,
    lh_logger_level_info,      lh_logger_level_debug,   lh_logger_level_critical,
    lh_logger_level_error,     lh_logger_level_warning};

const lh_char_t *
lh_logger_level_get_name(lh_logger_level_t level)
{
    return level <= lh_logger_level_debug ? m_logger_level_names[level] : "?";
}

lh_bool_t
lh_logger_level_try_parse(const lh_char_t *name, lh_logger_level_t *out)
{
    lh_assert_runtime_ref(out);
    lh_return_if(lh_null_eq(name), lh_bool_false);
    for (lh_usize_t i = 0U; i < lh_array_ptr_get_size(m_logger_level_names); ++i)
    {
        if (lh_str_ptr_equals(m_logger_level_names[i], name, lh_bool_true))
        {
            lh_ptr_deref(out) = m_logger_level_values[i];
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}
