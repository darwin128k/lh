#include <lh/version.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/text.h>
#include <lh/util/addr.h>

void
lh_version_set_major(lh_version_t *self, lh_version_major_t major)
{
    lh_assert_runtime_ref(self);
    self->major = major;
}

void
lh_version_set_minor(lh_version_t *self, lh_version_minor_t minor)
{
    lh_assert_runtime_ref(self);
    self->minor = minor;
}

void
lh_version_set_patch(lh_version_t *self, lh_version_patch_t patch)
{
    lh_assert_runtime_ref(self);
    self->patch = patch;
}

void
lh_version_set(lh_version_t *self, lh_version_major_t major, lh_version_minor_t minor,
               lh_version_patch_t patch)
{
    lh_version_set_major(self, major);
    lh_version_set_minor(self, minor);
    lh_version_set_patch(self, patch);
}

void
lh_version_assign(lh_version_t *self, const lh_version_t *other)
{
    lh_version_set(self, lh_version_get_major(other), lh_version_get_minor(other),
                   lh_version_get_patch(other));
}

lh_version_major_t
lh_version_get_major(const lh_version_t *self)
{
    lh_assert_runtime_ref(self);
    return self->major;
}

lh_version_minor_t
lh_version_get_minor(const lh_version_t *self)
{
    lh_assert_runtime_ref(self);
    return self->minor;
}

lh_version_patch_t
lh_version_get_patch(const lh_version_t *self)
{
    lh_assert_runtime_ref(self);
    return self->patch;
}

lh_bool_t
lh_version_is_at_least(const lh_version_t *self, const lh_version_t *minimum)
{
    const lh_version_major_t self_major = lh_version_get_major(self);
    const lh_version_major_t minimum_major = lh_version_get_major(minimum);
    if (self_major != minimum_major)
    {
        return self_major > minimum_major ? lh_bool_true : lh_bool_false;
    }

    const lh_version_minor_t self_minor = lh_version_get_minor(self);
    const lh_version_minor_t minimum_minor = lh_version_get_minor(minimum);
    if (self_minor != minimum_minor)
    {
        return self_minor > minimum_minor ? lh_bool_true : lh_bool_false;
    }

    return lh_version_get_patch(self) >= lh_version_get_patch(minimum) ? lh_bool_true
                                                                       : lh_bool_false;
}

lh_bool_t
lh_version_equals(const lh_version_t *self, const lh_version_t *other)
{
    if (lh_version_get_major(self) != lh_version_get_major(other))
    {
        return lh_bool_false;
    }
    if (lh_version_get_minor(self) != lh_version_get_minor(other))
    {
        return lh_bool_false;
    }
    return lh_version_get_patch(self) == lh_version_get_patch(other) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_version_is_compatible(const lh_version_t *self, const lh_version_t *required)
{
    lh_version_major_t self_major = lh_version_get_major(self);
    lh_version_major_t required_major = lh_version_get_major(required);

    if (self_major != required_major)
    {
        return lh_bool_false;
    }

    if (required_major == 0)
    {
        if (lh_version_get_minor(self) != lh_version_get_minor(required))
        {
            return lh_bool_false;
        }
        return lh_version_get_patch(self) >= lh_version_get_patch(required) ? lh_bool_true
                                                                            : lh_bool_false;
    }

    return lh_version_is_at_least(self, required);
}

lh_bool_t
lh_version_parse(lh_str_cptr str, lh_usize_t str_size, lh_version_t *out)
{
    lh_uint_t major;
    lh_uint_t minor;
    lh_uint_t patch;

    if (!lh_str_ptr_parse_text(str, str_size, "%u.%u.%u", lh_addr_of(major), lh_addr_of(minor),
                               lh_addr_of(patch)))
    {
        return lh_bool_false;
    }
    if (major > LH_VERSION_MAJOR_MAX || minor > LH_VERSION_MINOR_MAX || patch > LH_VERSION_PATCH_MAX)
    {
        return lh_bool_false;
    }

    lh_version_set(out, lh_cast_static(lh_version_major_t, major), lh_cast_static(lh_version_minor_t, minor),
                   lh_cast_static(lh_version_patch_t, patch));
    return lh_bool_true;
}

lh_usize_t
lh_version_format(const lh_version_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    return lh_str_ptr_format_text(str, str_size, "%u.%u.%u", lh_cast_static(lh_uint_t, lh_version_get_major(self)),
                                  lh_cast_static(lh_uint_t, lh_version_get_minor(self)),
                                  lh_cast_static(lh_uint_t, lh_version_get_patch(self)));
}
