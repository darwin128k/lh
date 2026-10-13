/**
 * @file std.c
 * @brief ::lh_memory_std_* — folders under std/ are the namespaces.
 *
 * One translation unit: pieces share `static` dispatch state. CMake lists only
 * this file; the includes below are not separate link units.
 */
#include <lh/memory/std/shared.h>

#include "std/words.c"
#include "std/x86/cpuid.c"
#include "std/x86/sse2.c"
#include "std/x86/avx2.c"
#include "std/dispatch.c"
