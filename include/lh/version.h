/**
 * @file version.h
 * @brief Version struct and associated API.
 *
 * Provides ::lh_version_t — a three-component semantic version (major, minor,
 * patch) — along with functions for field access, comparison, SemVer
 * compatibility, and `major.minor.patch` text.
 */

#ifndef LH_VERSION_H
#define LH_VERSION_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/limits.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/version/major.h>
#include <lh/version/minor.h>
#include <lh/version/patch.h>
#include <lh/version/view/fields.h>

/**
 * @def LH_VERSION_COMPONENT_COUNT
 * @brief Number of numeric fields in a SemVer core version (`major.minor.patch`).
 */
#define LH_VERSION_COMPONENT_COUNT 3U

/**
 * @def LH_VERSION_MAJOR_MAX
 * @brief Largest valid ::lh_version_major_t.
 *
 * Expands to ::LH_U16_T_MAX.
 */
#define LH_VERSION_MAJOR_MAX LH_U16_T_MAX

/**
 * @def LH_VERSION_MINOR_MAX
 * @brief Largest valid ::lh_version_minor_t.
 *
 * Expands to ::LH_U8_T_MAX.
 */
#define LH_VERSION_MINOR_MAX LH_U8_T_MAX

/**
 * @def LH_VERSION_PATCH_MAX
 * @brief Largest valid ::lh_version_patch_t.
 *
 * Expands to ::LH_U8_T_MAX.
 */
#define LH_VERSION_PATCH_MAX LH_U8_T_MAX

/**
 * @def LH_VERSION_TEXT_MAX
 * @brief Longest `major.minor.patch` text, excluding a NUL terminator.
 *
 * `65535.255.255` is 13 characters.
 */
#define LH_VERSION_TEXT_MAX 13U

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

/* ── set ─────────────────────────────────────────────────────────────────── */

/**
 * @brief Copy the version state from @p other into @p self.
 * @param self  Destination version (not null).
 * @param other Source version (not null).
 */
void
lh_version_assign(lh_version_t *self, const lh_version_t *other);

/**
 * @brief Replace @p self with @p major, @p minor, and @p patch.
 *
 * @param self  Version struct to modify.
 * @param major New major component.
 * @param minor New minor component.
 * @param patch New patch component.
 */
void
lh_version_set(lh_version_t *self, lh_version_major_t major, lh_version_minor_t minor,
               lh_version_patch_t patch);

/**
 * @brief Write only the major component.
 */
void
lh_version_set_major(lh_version_t *self, lh_version_major_t major);

/**
 * @brief Write only the minor component.
 */
void
lh_version_set_minor(lh_version_t *self, lh_version_minor_t minor);

/**
 * @brief Write only the patch component.
 */
void
lh_version_set_patch(lh_version_t *self, lh_version_patch_t patch);

/* ── getters ─────────────────────────────────────────────────────────────── */

/**
 * @brief Return the major component of @p self.
 * @param self Version struct to read from.
 * @return Major version number.
 */
lh_version_major_t
lh_version_get_major(const lh_version_t *self);

/**
 * @brief Return the minor component of @p self.
 * @param self Version struct to read from.
 * @return Minor version number.
 */
lh_version_minor_t
lh_version_get_minor(const lh_version_t *self);

/**
 * @brief Return the patch component of @p self.
 * @param self Version struct to read from.
 * @return Patch version number.
 */
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
lh_bool_t
lh_version_is_at_least(const lh_version_t *self, const lh_version_t *minimum);

/**
 * @brief Test whether @p self and @p other hold the same three components.
 *
 * @param self  Version to compare.
 * @param other Version to compare against.
 * @return ::lh_bool_true if major, minor, and patch all match.
 */
lh_bool_t
lh_version_equals(const lh_version_t *self, const lh_version_t *other);

/**
 * @brief SemVer compatibility: can @p self satisfy a requirement of @p required.
 *
 * Major must match. Then:
 * - major `>= 1`: @p self is not less than @p required (same rule as
 *   ::lh_version_is_at_least);
 * - major `0` (`0.y.z` is unstable): minor must also match, and patch must
 *   be at least @p required's patch.
 *
 * @param self     Provided version (e.g. a loaded plugin).
 * @param required Floor the caller asked for.
 */
lh_bool_t
lh_version_is_compatible(const lh_version_t *self, const lh_version_t *required);

/* ── parse / format ──────────────────────────────────────────────────────── */

/**
 * @brief Parse a strict SemVer core version (`major.minor.patch`).
 *
 * Requires exactly three decimal components separated by `.`, all of
 * @p str_size consumed. Rejects a leading zero on any component longer
 * than one digit (`01.2.3`), whitespace, a leading sign, pre-release
 * (`1.2.3-rc.1`), and build metadata (`1.2.3+build`).
 *
 * Ranges: major `0`–::LH_VERSION_MAJOR_MAX, minor `0`–::LH_VERSION_MINOR_MAX,
 * patch `0`–::LH_VERSION_PATCH_MAX.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param out      Receives the parsed version on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a valid
 *         version, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_version_parse(lh_str_cptr str, lh_usize_t str_size, lh_version_t *out);

/**
 * @brief Format @p self as `major.minor.patch` text.
 *
 * No NUL terminator is appended; a buffer of ::LH_VERSION_TEXT_MAX + 1
 * always has room to add one after the returned length.
 *
 * @param self     Version to format.
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written (5–13), or 0 if @p str_size was too small.
 */
lh_usize_t
lh_version_format(const lh_version_t *self, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_VERSION_H */