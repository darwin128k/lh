/**
 * @file cidr.h
 * @brief ::lh_net_cidr_t — an IPv4 block written `a.b.c.d/n`, and whether an
 *        address falls into it.
 *
 * A bare address is the block of that one address (`/32`). The network is kept
 * with the host bits cleared, so `10.1.2.3/8` is the block `10.0.0.0/8`.
 */

#ifndef LH_NET_CIDR_H
#define LH_NET_CIDR_H

#include <lh/core/net/ip.h>

/** @brief An IPv4 block: its network and its mask, both in host order. */
struct lh_net_cidr
{
    lh_u32_t net;  /**< the network, host bits cleared */
    lh_u32_t mask; /**< the prefix as a mask: `/24` is `0xFFFFFF00` */
};
/** @copybrief lh_net_cidr */
typedef struct lh_net_cidr lh_net_cidr_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The network, host bits cleared, in host order (level 1).
 *
 * @param self The block; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_net_cidr_get_net(const lh_net_cidr_t *self);

/**
 * @brief Store the network (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The block; checked by ::lh_assert_runtime_ref.
 * @param net  The new value.
 */
lh_void
lh_net_cidr_set_net(lh_net_cidr_t *self, lh_u32_t net);

/**
 * @brief The prefix as a mask, in host order (level 1).
 *
 * @param self The block; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_net_cidr_get_mask(const lh_net_cidr_t *self);

/**
 * @brief Store the prefix as a mask (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The block; checked by ::lh_assert_runtime_ref.
 * @param mask The new value.
 */
lh_void
lh_net_cidr_set_mask(lh_net_cidr_t *self, lh_u32_t mask);

/**
 * @brief Make @p self the block of @p address with a prefix of @p bits (level 2).
 *
 * @param self    The block; checked by ::lh_assert_runtime_ref.
 * @param address Any address of the block; checked by ::lh_assert_runtime_ref.
 * @param bits    The prefix length, 0..32; more counts as 32.
 */
lh_void
lh_net_cidr_init(lh_net_cidr_t *self, const lh_net_ip4_t *address, lh_uint_t bits);

/**
 * @brief Whether @p address falls into @p self (level 2).
 *
 * @param self    The block; checked by ::lh_assert_runtime_ref.
 * @param address The address; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_net_cidr_contains(const lh_net_cidr_t *self, const lh_net_ip4_t *address);

/**
 * @brief Read `a.b.c.d` or `a.b.c.d/n` (n 0..32, no leading zero) (level 3).
 *
 * @param text      The text (not required to be NUL-terminated).
 * @param text_size Characters in @p text.
 * @param out       Receives the block; checked by ::lh_assert_runtime_ref.
 * @return ::lh_bool_false for malformed text; @p out is untouched then.
 */
lh_bool_t
lh_net_cidr_try_parse(const lh_str_ptr text, lh_usize_t text_size, lh_net_cidr_t *out);

/**
 * @brief Whether @p address falls into any of @p count blocks (level 3).
 *
 * @param list    The blocks; may be ::lh_null when @p count is 0.
 * @param count   Blocks in @p list.
 * @param address The address; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_net_cidr_list_contains(const lh_net_cidr_t *list, lh_usize_t count, const lh_net_ip4_t *address);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_CIDR_H */
