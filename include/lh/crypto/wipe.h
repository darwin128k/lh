/**
 * @file wipe.h
 * @brief Erase secrets (keys, seeds, plaintext) from memory.
 */

#ifndef LH_CRYPTO_WIPE_H
#define LH_CRYPTO_WIPE_H

#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/size.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Overwrite @p size bytes at @p secret with zeros.
 *
 * Writes through a volatile pointer, so the compiler cannot drop the stores
 * as dead even when @p secret is never read again. A null @p secret is a no-op.
 */
lh_void
lh_crypto_wipe(lh_ptr secret, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRYPTO_WIPE_H */
