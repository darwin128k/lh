/**
 * @file version.h
 * @brief Version struct and associated API.
 *
 * Provides ::lh_version_t — a three-component semantic version (major, minor,
 * patch) — along with functions for packing, unpacking, individual field
 * access, and comparison.
 */

#ifndef LH_VERSION_H
#define LH_VERSION_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/version/major.h>
#include <lh/version/minor.h>
#include <lh/version/patch.h>
#include <lh/version/view/fields.h>

LH_COMPILER_EXTERN_C_BEGIN

/* ── struct ──────────────────────────────────────────────────────────────── */

/**
 * @struct lh_version
 * @brief Three-component semantic version (major.minor.patch).
 *
 * Fields are injected via ::lh_version_fields and typed as
 * ::lh_version_major_t, ::lh_version_minor_t, and ::lh_version_patch_t.
 */
struct lh_version
{
    /** Major, minor, and patch fields of types ::lh_version_major_t,
     *  ::lh_version_minor_t, and ::lh_version_patch_t. */
    lh_version_fields(lh_version_major_t, lh_version_minor_t, lh_version_patch_t);
};

/**
 * @typedef lh_version_t
 * @brief Alias for `struct lh_version`.
 */
typedef struct lh_version lh_version_t;

/* ── pack / unpack ───────────────────────────────────────────────────────── */

/**
 * @brief Write individual components into a version struct.
 *
 * Each output pointer is optional: pass ::lh_null to skip that component.
 *
 * @param self  Version struct to modify.
 * @param major New major value, or ::lh_null to leave unchanged.
 * @param minor New minor value, or ::lh_null to leave unchanged.
 * @param patch New patch value, or ::lh_null to leave unchanged.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_version_pack(lh_version_t *self, const lh_version_major_t *major,
                const lh_version_minor_t *minor, const lh_version_patch_t *patch);

/**
 * @brief Read individual components out of a version struct.
 *
 * Each output pointer is optional: pass ::lh_null to skip that component.
 *
 * @param self  Version struct to read from.
 * @param major Output for major, or ::lh_null to skip.
 * @param minor Output for minor, or ::lh_null to skip.
 * @param patch Output for patch, or ::lh_null to skip.
 *
 * Example usage:
 * @code{.c}
 * lh_version_major_t major;
 * lh_version_unpack(&ver, &major, lh_null, lh_null);
 * @endcode
 */
LH_ATTRIBUTE_SYMBOL
void
lh_version_unpack(const lh_version_t *self, lh_version_major_t *major, lh_version_minor_t *minor,
                  lh_version_patch_t *patch);

/**
 * @brief Unpack @p self into @p other.
 * @param self  Source version (not null).
 * @param other Destination version (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_version_unpack_to_other(const lh_version_t *self, lh_version_t *other);

/* ── set ─────────────────────────────────────────────────────────────────── */

/**
 * @brief Copy the version state from @p other into @p self.
 * @param self  Destination version (not null).
 * @param other Source version (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_version_assign(lh_version_t *self, const lh_version_t *other);

/**
 * @brief Replace @p self with @p major, @p minor, and @p patch.
 *
 * Equivalent to ::lh_version_pack with all three component pointers provided.
 *
 * @param self  Version struct to modify.
 * @param major New major component.
 * @param minor New minor component.
 * @param patch New patch component.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_version_set(lh_version_t *self, lh_version_major_t major, lh_version_minor_t minor,
               lh_version_patch_t patch);

/* ── getters ─────────────────────────────────────────────────────────────── */

/**
 * @brief Return the major component of @p self.
 * @param self Version struct to read from.
 * @return Major version number.
 */
LH_ATTRIBUTE_SYMBOL
lh_version_major_t
lh_version_get_major(const lh_version_t *self);

/**
 * @brief Return the minor component of @p self.
 * @param self Version struct to read from.
 * @return Minor version number.
 */
LH_ATTRIBUTE_SYMBOL
lh_version_minor_t
lh_version_get_minor(const lh_version_t *self);

/**
 * @brief Return the patch component of @p self.
 * @param self Version struct to read from.
 * @return Patch version number.
 */
LH_ATTRIBUTE_SYMBOL
lh_version_patch_t
lh_version_get_patch(const lh_version_t *self);

/* ── comparison ──────────────────────────────────────────────────────────── */

/**
 * @brief Lexicographic order: true if @p self is not less than @p minimum
 * (major, then minor, then patch).
 *
 * @param self    Version under test (e.g. device or runtime).
 * @param minimum Required floor (e.g. minimum supported version).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_version_is_at_least(const lh_version_t *self, const lh_version_t *minimum);

LH_COMPILER_EXTERN_C_END

#endif /* LH_VERSION_H */