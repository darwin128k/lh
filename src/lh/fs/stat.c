#include <lh/fs/stat.h>
#include <lh/assert.h>
#include <lh/util/ptr.h>

void
lh_fs_stat_init(lh_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    self->kind = lh_fs_kind_other;
    self->perm = 0U;
    self->size = 0U;
    self->atime = 0;
    self->mtime = 0;
    self->ctime = 0;
    self->attr = 0U;
}

void
lh_fs_stat_assign(lh_fs_stat_t *self, const lh_fs_stat_t *other)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);
    lh_ptr_deref(self) = lh_ptr_deref(other);
}

lh_fs_kind_t
lh_fs_stat_get_kind(const lh_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_fs_perm_t
lh_fs_stat_get_perm(const lh_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->perm;
}

lh_fs_size_t
lh_fs_stat_get_size(const lh_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_fs_time_t
lh_fs_stat_get_atime(const lh_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->atime;
}

lh_fs_time_t
lh_fs_stat_get_mtime(const lh_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mtime;
}

lh_fs_time_t
lh_fs_stat_get_ctime(const lh_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ctime;
}

lh_fs_attr_t
lh_fs_stat_get_attr(const lh_fs_stat_t *self)
{
    lh_assert_runtime_ref(self);
    return self->attr;
}

void
lh_fs_stat_set(lh_fs_stat_t *self, lh_fs_kind_t kind, lh_fs_perm_t perm, lh_fs_size_t size,
               lh_fs_time_t atime, lh_fs_time_t mtime, lh_fs_time_t ctime, lh_fs_attr_t attr)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
    self->perm = perm;
    self->size = size;
    self->atime = atime;
    self->mtime = mtime;
    self->ctime = ctime;
    self->attr = attr;
}

void
lh_fs_stat_set_attr(lh_fs_stat_t *self, lh_fs_attr_t attr)
{
    lh_assert_runtime_ref(self);
    self->attr = attr;
}
