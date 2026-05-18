#include <lh/version.h>
#include <lh/optional/ref.h>
#include <lh/util/ptr.h>

void
lh_version_pack(lh_version_t *self, const lh_version_major_t *major,
                const lh_version_minor_t *minor, const lh_version_patch_t *patch)
{
    lh_optional_ref(major)
    {
        self->major = lh_ptr_deref(major);
    }

    lh_optional_ref(minor)
    {
        self->minor = lh_ptr_deref(minor);
    }

    lh_optional_ref(patch)
    {
        self->patch = lh_ptr_deref(patch);
    }
}

void
lh_version_unpack(const lh_version_t *self, lh_version_major_t *major, lh_version_minor_t *minor,
                  lh_version_patch_t *patch)
{
    lh_optional_ref(major)
    {
        lh_ptr_deref(major) = self->major;
    }

    lh_optional_ref(minor)
    {
        lh_ptr_deref(minor) = self->minor;
    }

    lh_optional_ref(patch)
    {
        lh_ptr_deref(patch) = self->patch;
    }
}

void
lh_version_assign(lh_version_t *self, const lh_version_t *other)
{
    lh_version_major_t major;
    lh_version_minor_t minor;
    lh_version_patch_t patch;
    lh_version_unpack(other, lh_addr_of(major), lh_addr_of(minor), lh_addr_of(patch));
    lh_version_set(self, major, minor, patch);
}

void
lh_version_set(lh_version_t *self, lh_version_major_t major, lh_version_minor_t minor,
               lh_version_patch_t patch)
{
    lh_version_pack(self, lh_addr_of(major), lh_addr_of(minor), lh_addr_of(patch));
}

void
lh_version_unpack_to_other(const lh_version_t *self, lh_version_t *other)
{
    lh_version_assign(other, self);
}

lh_version_major_t
lh_version_get_major(const lh_version_t *self)
{
    lh_version_major_t major;
    lh_version_unpack(self, lh_addr_of(major), lh_null, lh_null);
    return major;
}

lh_version_minor_t
lh_version_get_minor(const lh_version_t *self)
{
    lh_version_minor_t minor;
    lh_version_unpack(self, lh_null, lh_addr_of(minor), lh_null);
    return minor;
}

lh_version_patch_t
lh_version_get_patch(const lh_version_t *self)
{
    lh_version_patch_t patch;
    lh_version_unpack(self, lh_null, lh_null, lh_addr_of(patch));
    return patch;
}

lh_bool_t
lh_version_is_at_least(const lh_version_t *self, const lh_version_t *minimum)
{
    if (self->major != minimum->major)
    {
        return self->major > minimum->major ? lh_bool_true : lh_bool_false;
    }
    if (self->minor != minimum->minor)
    {
        return self->minor > minimum->minor ? lh_bool_true : lh_bool_false;
    }
    return self->patch >= minimum->patch ? lh_bool_true : lh_bool_false;
}
