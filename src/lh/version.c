#include <lh/version.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/optional/ref.h>
#include <lh/str/format/text.h>
#include <lh/str/parse/uint.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>
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
    static const lh_uint_t component_max[LH_VERSION_COMPONENT_COUNT] = {
        LH_VERSION_MAJOR_MAX, LH_VERSION_MINOR_MAX, LH_VERSION_PATCH_MAX};
    lh_uint_t component[LH_VERSION_COMPONENT_COUNT];
    lh_usize_t pos = 0;
    lh_usize_t component_index;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    for (component_index = 0; component_index < LH_VERSION_COMPONENT_COUNT; component_index++)
    {
        lh_bool_t is_last = component_index == LH_VERSION_COMPONENT_COUNT - 1U;
        lh_str_cptr field;
        lh_usize_t field_size;
        lh_bool_t had_delim;

        if (!lh_str_ptr_split_next(str, str_size, '.', lh_addr_of(pos), lh_addr_of(field),
                                   lh_addr_of(field_size), lh_addr_of(had_delim)))
        {
            return lh_bool_false;
        }
        if (had_delim == is_last)
        {
            return lh_bool_false;
        }
        if (!lh_str_ptr_parse_uint(field, field_size, component_max[component_index],
                                   lh_addr_of(component[component_index])))
        {
            return lh_bool_false;
        }
    }

    lh_version_set(out, (lh_version_major_t)component[0], (lh_version_minor_t)component[1],
                   (lh_version_patch_t)component[2]);
    return lh_bool_true;
}

lh_usize_t
lh_version_format(const lh_version_t *self, lh_str_ptr str, lh_usize_t str_size)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(str);

    return lh_str_ptr_format_text(str, str_size, "%u.%u.%u", (lh_uint_t)lh_version_get_major(self),
                                  (lh_uint_t)lh_version_get_minor(self),
                                  (lh_uint_t)lh_version_get_patch(self));
}
