/**
 * @file bit.h
 * @brief Bit primitives: `lh_bit_*` operator macros, byte swap, endian
 *        pack / unpack, half-word split, packed fields, rotate and scan.
 *
 * - **Operator macros** (`<lh/bit/ops.h>`): `lh_bit_and`, `lh_bit_mask`,
 *   `lh_bit_shl`, … — the `lh_math_bit_*` macros under the `lh_bit_*` name.
 * - **Byte swap** (`<lh/bit/bswap.h>`), **endian** pack / unpack
 *   (`<lh/bit/endian.h>`, with the half-word split of `<lh/bit/half.h>`),
 *   **packed** fields (`<lh/bit/packed.h>`), **rotate** (`<lh/bit/rotate.h>`)
 *   and **scan** (`<lh/bit/scan.h>`).
 *
 * The headers under `lh/bit/` declare one thing each — include this file
 * when you need several.
 */

#ifndef LH_BIT_H
#define LH_BIT_H

#include <lh/bit/bswap.h>
#include <lh/bit/endian.h>
#include <lh/bit/packed.h>
#include <lh/bit/rotate.h>
#include <lh/bit/scan.h>

#endif /* LH_BIT_H */
