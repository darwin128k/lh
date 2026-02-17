#include <lh/null.h>
#include <lh/optional_ref.h>
#include <lh/version.h>

void lh_version_unpack(const lh_version_t *self, lh_ushort_t *major,
                       lh_uchar_t *minor, lh_uchar_t *patch) {
    lh_optional_ref(major) { lh_ptr_deref(major) = self->major; }
    lh_optional_ref(minor) { lh_ptr_deref(minor) = self->minor; }
    lh_optional_ref(patch) { lh_ptr_deref(patch) = self->patch; }
}

lh_ushort_t lh_version_get_major(const lh_version_t *self) {
    lh_ushort_t major;
    lh_version_unpack(self, &major, lh_null, lh_null);
    return major;
}

lh_uchar_t lh_version_get_minor(const lh_version_t *self) {
    lh_uchar_t minor;
    lh_version_unpack(self, lh_null, &minor, lh_null);
    return minor;
}

lh_uchar_t lh_version_get_patch(const lh_version_t *self) {
    lh_uchar_t patch;
    lh_version_unpack(self, lh_null, lh_null, &patch);
    return patch;
}