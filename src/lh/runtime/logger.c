#include <lh/runtime/logger.h>
#include <lh/attribute/thread_local.h>
#include <lh/config.h>
#include <lh/library/option.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/cast/static.h>

#if (LH_LIBRARY_OPTION_RUNTIME_LOGGER_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include <lh/self.h>
#    include <lh/str/cstr.h>
#    include <lh/str/format/text.h>

#    include <stdio.h>

/* A record longer than this is written as its format mask, unformatted: the
   formatter is all-or-nothing, and a record that says something is better
   than one that is silently lost. */
#    define LH_RUNTIME_LOGGER_LINE 1024u

static lh_ssize_t
lh_runtime_logger_stderr(lh_self_ptr self, lh_logger_level_t level, lh_str_cptr fmt, va_list args)
{
    char line[LH_RUNTIME_LOGGER_LINE];
    lh_usize_t n = lh_str_ptr_format_text_v(line, sizeof(line) - 1u, fmt, args);
    const char *text = line;

    (void)self;
    (void)level;
    if (n == 0u)
    {
        text = fmt;
        n = lh_str_len(fmt);
    }
    if (fwrite(text, 1u, lh_cast_static(size_t, n), stderr) != lh_cast_static(size_t, n)|| fputc('\n', stderr) == EOF)
    {
        return -1;
    }
    return (lh_ssize_t)(n + 1u);
}

LH_ATTRIBUTE_THREAD_LOCAL
lh_logger_t m_runtime_logger = {
    lh_logger_level_flags_all, lh_runtime_logger_stderr, lh_runtime_logger_stderr, lh_runtime_logger_stderr,
    lh_runtime_logger_stderr,  lh_runtime_logger_stderr, lh_runtime_logger_stderr, lh_runtime_logger_stderr,
    lh_runtime_logger_stderr,  lh_null};
#elif (LH_LIBRARY_OPTION_RUNTIME_LOGGER_USE_STDLIB == LH_LIBRARY_OPTION_OFF)
LH_ATTRIBUTE_THREAD_LOCAL
lh_logger_t m_runtime_logger = {lh_logger_level_flags_none, lh_null, lh_null, lh_null, lh_null,
                                lh_null,                    lh_null, lh_null, lh_null, lh_null};
#endif

lh_logger_t *
lh_runtime_logger(void)
{
    return lh_addr_of(m_runtime_logger);
}
