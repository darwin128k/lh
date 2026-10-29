/**
 * @file search.h
 * @brief Backend-private: what ::lh_os_system_shared_match looks for among the loaded
 *        images, and the full path it finds.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_SHARED_SEARCH_H
#define LH_SRC_OS_SYSTEM_POSIX_SHARED_SEARCH_H

#include <lh/str/ptr.h>
#include <lh/compiler/extern/c.h>

/** What ::lh_os_system_shared_match looks for, and the full path it finds. */
struct lh_os_system_shared_search
{
    lh_str_cptr name; /**< the file name looked for */
    lh_str_cptr path; /**< the full path of the image found; ::lh_null until found */
};

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The file name looked for (level 1).
 *
 * @param self The search; checked by ::lh_assert_runtime_ref.
 */
lh_str_cptr
lh_os_system_shared_search_get_name(const struct lh_os_system_shared_search *self);

/**
 * @brief Store the file name looked for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The search; checked by ::lh_assert_runtime_ref.
 * @param name The new value.
 */
lh_void
lh_os_system_shared_search_set_name(struct lh_os_system_shared_search *self, lh_str_cptr name);

/**
 * @brief The full path of the image found; ::lh_null until found (level 1).
 *
 * @param self The search; checked by ::lh_assert_runtime_ref.
 */
lh_str_cptr
lh_os_system_shared_search_get_path(const struct lh_os_system_shared_search *self);

/**
 * @brief Store the full path of the image found; ::lh_null until found (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The search; checked by ::lh_assert_runtime_ref.
 * @param path The new value.
 */
lh_void
lh_os_system_shared_search_set_path(struct lh_os_system_shared_search *self, lh_str_cptr path);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_POSIX_SHARED_SEARCH_H */
