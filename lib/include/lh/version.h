#ifndef LH_VERSION_H
#define LH_VERSION_H

#include <lh/attribute.h>
#include <lh/numeric_types.h>
#include <lh/version_fields.h>

typedef struct lh_version {
    lh_version_fields(lh_ushort_t, lh_uchar_t, lh_uchar_t);
} lh_version_t;

LH_COMPILER(EXTERN_C_BEGIN)

LH_ATTRIBUTE(SYMBOL)
void lh_version_unpack(const lh_version_t *self, lh_ushort_t *major,
                       lh_uchar_t *minor, lh_uchar_t *patch);

LH_ATTRIBUTE(SYMBOL)
lh_ushort_t lh_version_get_major(const lh_version_t *self);

LH_ATTRIBUTE(SYMBOL)
lh_uchar_t lh_version_get_minor(const lh_version_t *self);

LH_ATTRIBUTE(SYMBOL)
lh_uchar_t lh_version_get_patch(const lh_version_t *self);

LH_COMPILER(EXTERN_C_END)

#endif // LH_VERSION_H
