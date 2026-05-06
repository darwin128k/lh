#include <lh/lh.h>
#include <lh/util/addr.h>
#include <lh/version/fallback.h>
#include <lh/version/initializer.h>

const static lh_version_t m_version =
    lh_version_initializer(LH_VERSION_MAJOR, LH_VERSION_MINOR, LH_VERSION_PATCH);

const lh_version_t *
lh_get_version(void) {
    return lh_addr_of(m_version);
}
