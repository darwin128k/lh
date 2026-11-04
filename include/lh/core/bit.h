/**
 * @file bit.h
 * @brief Bit primitives: `lh_bit_*` operator macros, byte swap, endian
 *        pack / unpack, half-word split, packed fields, rotate and scan.
 *
 * - **Operator macros** (`<lh/core/bit/ops.h>`): `lh_bit_and`, `lh_bit_mask`,
 *   `lh_bit_shl`, … — the `lh_math_bit_*` macros under the `lh_bit_*` name.
 * - **Byte swap** (`<lh/core/bit/bswap.h>`), **endian** pack / unpack
 *   (`<lh/core/bit/endian.h>`, with the half-word split of `<lh/core/bit/half.h>`),
 *   **packed** fields (`<lh/core/bit/packed.h>`), **rotate** (`<lh/core/bit/rotate.h>`)
 *   and **scan** (`<lh/core/bit/scan.h>`).
 *
 * The headers under `lh/core/bit/` declare one thing each — include this file
 * when you need several.
 */

#ifndef LH_BIT_H
#define LH_BIT_H

#include <lh/core/bit/bswap.h>
#include <lh/core/bit/endian.h>
#include <lh/core/bit/packed.h>
#include <lh/core/bit/rotate.h>
#include <lh/core/bit/scan.h>

#endif /* LH_BIT_H */
