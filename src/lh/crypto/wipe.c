#include <lh/byte.h>
#include <lh/crypto/wipe.h>
#include <lh/null.h>

lh_void
lh_crypto_wipe(lh_ptr secret, lh_usize_t size)
{
    volatile lh_byte_t *bytes = (volatile lh_byte_t *)secret;

    if (lh_null_eq(secret))
    {
        return;
    }
    while (size > 0U)
    {
        *bytes = 0U;
        ++bytes;
        --size;
    }
}
