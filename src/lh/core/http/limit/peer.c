/**
 * @file peer.c
 * @brief ::lh_http_limit_peer_t: per-address connection and rate limits.
 */

#include <lh/core/http/limit/peer.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>

lh_u32_t
lh_http_limit_peer_get_key(const lh_http_limit_peer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->key;
}

lh_void
lh_http_limit_peer_set_key(lh_http_limit_peer_t *self, lh_u32_t key)
{
    lh_assert_runtime_ref(self);
    self->key = key;
}

lh_bool_t
lh_http_limit_peer_is_used(const lh_http_limit_peer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->used;
}

lh_void
lh_http_limit_peer_set_used(lh_http_limit_peer_t *self, lh_bool_t used)
{
    lh_assert_runtime_ref(self);
    self->used = used;
}

lh_u32_t
lh_http_limit_peer_get_active(const lh_http_limit_peer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->active;
}

lh_void
lh_http_limit_peer_set_active(lh_http_limit_peer_t *self, lh_u32_t active)
{
    lh_assert_runtime_ref(self);
    self->active = active;
}

lh_u32_t
lh_http_limit_peer_get_count(const lh_http_limit_peer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_http_limit_peer_set_count(lh_http_limit_peer_t *self, lh_u32_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_u32_t
lh_http_limit_peer_get_pending(const lh_http_limit_peer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pending;
}

lh_void
lh_http_limit_peer_set_pending(lh_http_limit_peer_t *self, lh_u32_t pending)
{
    lh_assert_runtime_ref(self);
    self->pending = pending;
}

lh_s64_t
lh_http_limit_peer_get_window(const lh_http_limit_peer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->window;
}

lh_void
lh_http_limit_peer_set_window(lh_http_limit_peer_t *self, lh_s64_t window)
{
    lh_assert_runtime_ref(self);
    self->window = window;
}

lh_bool_t
lh_http_limit_peer_is_free(const lh_http_limit_peer_t *self, lh_s64_t now)
{
    return (!lh_http_limit_peer_is_used(self) ||
            (lh_math_is_zero(lh_http_limit_peer_get_active(self)) &&
             lh_math_ge(lh_math_sub(now, lh_http_limit_peer_get_window(self)),
                        LH_HTTP_LIMIT_PEER_WINDOW_MS)))
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_http_limit_peer_init(lh_http_limit_peer_t *self, lh_u32_t key, lh_s64_t now)
{
    lh_http_limit_peer_set_key(self, key);
    lh_http_limit_peer_set_used(self, lh_bool_true);
    lh_http_limit_peer_set_active(self, 1U);
    lh_http_limit_peer_set_count(self, 1U);
    lh_http_limit_peer_set_pending(self, 0U);
    lh_http_limit_peer_set_window(self, now);
}

lh_void
lh_http_limit_peer_release(lh_http_limit_peer_t *self)
{
    if (!lh_math_is_zero(lh_http_limit_peer_get_active(self)))
    {
        lh_http_limit_peer_set_active(self, lh_math_sub_one(lh_http_limit_peer_get_active(self)));
    }
}

lh_void
lh_http_limit_peer_add_pending(lh_http_limit_peer_t *self)
{
    lh_http_limit_peer_set_pending(self, lh_math_add_one(lh_http_limit_peer_get_pending(self)));
}

lh_void
lh_http_limit_peer_drop_pending(lh_http_limit_peer_t *self)
{
    if (!lh_math_is_zero(lh_http_limit_peer_get_pending(self)))
    {
        lh_http_limit_peer_set_pending(self, lh_math_sub_one(lh_http_limit_peer_get_pending(self)));
    }
}

lh_bool_t
lh_http_limit_peer_try_enter(lh_http_limit_peer_t *self, lh_s64_t now, lh_u32_t per_ip,
                             lh_u32_t per_second)
{
    if (lh_math_ge(lh_math_sub(now, lh_http_limit_peer_get_window(self)),
                   LH_HTTP_LIMIT_PEER_WINDOW_MS))
    {
        lh_http_limit_peer_set_window(self, now);
        lh_http_limit_peer_set_count(self, 0U);
    }
    if (lh_math_ge(lh_http_limit_peer_get_active(self), per_ip) ||
        lh_math_ge(lh_http_limit_peer_get_count(self), per_second))
    {
        return lh_bool_false;
    }
    lh_http_limit_peer_set_count(self, lh_math_add_one(lh_http_limit_peer_get_count(self)));
    lh_http_limit_peer_set_active(self, lh_math_add_one(lh_http_limit_peer_get_active(self)));
    return lh_bool_true;
}

lh_bool_t
lh_http_limit_peer_try_acquire(lh_http_limit_peer_t *peers, lh_usize_t count, lh_u32_t key,
                               lh_s64_t now, lh_u32_t per_ip, lh_u32_t per_second, lh_usize_t *slot)
{
    lh_usize_t free_slot = count;
    /* A full table must not refuse a new address: an idle peer whose window has
       not ended yet is reused, the oldest window first. */
    lh_usize_t idle_slot = count;

    lh_assert_runtime_ref(peers);
    lh_assert_runtime_ref(slot);
    for (lh_usize_t i = 0U; lh_math_lt(i, count); ++i)
    {
        lh_http_limit_peer_t *peer = peers + i;

        if (lh_http_limit_peer_is_used(peer) && lh_math_eq(lh_http_limit_peer_get_key(peer), key))
        {
            if (!lh_http_limit_peer_try_enter(peer, now, per_ip, per_second))
            {
                return lh_bool_false;
            }
            lh_ptr_deref(slot) = i;
            return lh_bool_true;
        }
        if (lh_http_limit_peer_is_free(peer, now))
        {
            free_slot = i;
        }
        else if (lh_math_is_zero(lh_http_limit_peer_get_active(peer)) &&
                 (lh_math_eq(idle_slot, count) ||
                  lh_math_lt(lh_http_limit_peer_get_window(peer),
                             lh_http_limit_peer_get_window(peers + idle_slot))))
        {
            idle_slot = i;
        }
    }
    if (lh_math_eq(free_slot, count))
    {
        free_slot = idle_slot;
    }
    if (lh_math_eq(free_slot, count))
    {
        return lh_bool_false;
    }
    lh_http_limit_peer_init(peers + free_slot, key, now);
    lh_ptr_deref(slot) = free_slot;
    return lh_bool_true;
}
