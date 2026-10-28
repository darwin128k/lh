#include <lh/fs/stat.h>
#include <lh/assert.h>

lh_void
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

lh_void
lh_fs_stat_set_attr(lh_fs_stat_t *self, lh_fs_attr_t attr)
{
    lh_assert_runtime_ref(self);
    self->attr = attr;
}

lh_void
lh_fs_stat_set_kind(lh_fs_stat_t *self, lh_fs_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_void
lh_fs_stat_set_perm(lh_fs_stat_t *self, lh_fs_perm_t perm)
{
    lh_assert_runtime_ref(self);
    self->perm = perm;
}

lh_void
lh_fs_stat_set_size(lh_fs_stat_t *self, lh_fs_size_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

lh_void
lh_fs_stat_set_atime(lh_fs_stat_t *self, lh_fs_time_t atime)
{
    lh_assert_runtime_ref(self);
    self->atime = atime;
}

lh_void
lh_fs_stat_set_mtime(lh_fs_stat_t *self, lh_fs_time_t mtime)
{
    lh_assert_runtime_ref(self);
    self->mtime = mtime;
}

lh_void
lh_fs_stat_set_ctime(lh_fs_stat_t *self, lh_fs_time_t ctime)
{
    lh_assert_runtime_ref(self);
    self->ctime = ctime;
}

lh_void
lh_fs_stat_set(lh_fs_stat_t *self, lh_fs_kind_t kind, lh_fs_perm_t perm, lh_fs_size_t size,
               lh_fs_time_t atime, lh_fs_time_t mtime, lh_fs_time_t ctime, lh_fs_attr_t attr)
{
    lh_fs_stat_set_kind(self, kind);
    lh_fs_stat_set_perm(self, perm);
    lh_fs_stat_set_size(self, size);
    lh_fs_stat_set_atime(self, atime);
    lh_fs_stat_set_mtime(self, mtime);
    lh_fs_stat_set_ctime(self, ctime);
    lh_fs_stat_set_attr(self, attr);
}

lh_void
lh_fs_stat_init(lh_fs_stat_t *self)
{
    lh_fs_stat_set(self, lh_fs_kind_other, 0U, 0U, 0, 0, 0, 0U);
}
