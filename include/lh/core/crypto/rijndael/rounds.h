/**
 * @file rounds.h
 * @brief ::lh_crypto_rijndael_rounds — the rounds of Rijndael on one state, inlined
 *        into the caller so a block size known there is a constant here.
 *
 * Rijndael takes 16-, 24- and 32-byte blocks, so ::lh_crypto_rijndael_cipher reads the
 * size from the key schedule at run time, and every step under it — SubBytes over the
 * state, ShiftRows and MixColumns over its columns, the round key's XOR — then loops
 * over a size the compiler cannot see. AES is the 16-byte case. With the rounds inlined
 * and called with the literal 16, each step is specialised for it; measured on an
 * STM32F407 (168 MHz, no AES-NI): the rounds of one AES-128 block took about 7 100
 * cycles with the size read at run time, and the same steps with 16 written in took
 * 3 266.
 *
 * One implementation, specialised twice by its caller; there is no second copy.
 */

#ifndef LH_CRYPTO_RIJNDAEL_ROUNDS_H
#define LH_CRYPTO_RIJNDAEL_ROUNDS_H

#include <lh/core/attribute/force_inline.h>
#include <lh/core/crypto/rijndael.h>
#include <lh/core/memory/std.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Encrypt (or, with @p decrypt, decrypt) @p state of @p block_size bytes in
 *        place: the initial AddRoundKey, the full rounds, and the last round without
 *        MixColumns, with the round keys of @p self.
 *
 * Inlined at every call: call it with a constant @p block_size where the caller knows
 * one. @p self is a key schedule made for @p block_size; nothing is checked here.
 */
LH_ATTRIBUTE_FORCE_INLINE lh_void
lh_crypto_rijndael_rounds(const lh_crypto_rijndael_t *self, lh_byte_ptr state,
                          lh_usize_t block_size, lh_bool_t decrypt)
{
    const lh_usize_t nb = block_size / 4U;
    const lh_usize_t nr = lh_crypto_rijndael_get_round_count(self);

    if (!decrypt)
    {
        lh_memory_std_xor(state, state, lh_crypto_rijndael_get_round_key(self, 0U), block_size);
        for (lh_usize_t round = 1U; round < nr; ++round)
        {
            lh_crypto_rijndael_sub_bytes(state, block_size, lh_bool_false);
            lh_crypto_rijndael_shift_rows(state, nb, lh_bool_false);
            lh_crypto_rijndael_mix_columns(state, nb, lh_bool_false);
            lh_memory_std_xor(state, state, lh_crypto_rijndael_get_round_key(self, round),
                              block_size);
        }
        lh_crypto_rijndael_sub_bytes(state, block_size, lh_bool_false);
        lh_crypto_rijndael_shift_rows(state, nb, lh_bool_false);
        lh_memory_std_xor(state, state, lh_crypto_rijndael_get_round_key(self, nr), block_size);
        return;
    }
    lh_memory_std_xor(state, state, lh_crypto_rijndael_get_round_key(self, nr), block_size);
    for (lh_usize_t round = nr; round > 1U; --round)
    {
        lh_crypto_rijndael_shift_rows(state, nb, lh_bool_true);
        lh_crypto_rijndael_sub_bytes(state, block_size, lh_bool_true);
        lh_memory_std_xor(state, state, lh_crypto_rijndael_get_round_key(self, round - 1U),
                          block_size);
        lh_crypto_rijndael_mix_columns(state, nb, lh_bool_true);
    }
    lh_crypto_rijndael_shift_rows(state, nb, lh_bool_true);
    lh_crypto_rijndael_sub_bytes(state, block_size, lh_bool_true);
    lh_memory_std_xor(state, state, lh_crypto_rijndael_get_round_key(self, 0U), block_size);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_RIJNDAEL_ROUNDS_H */
