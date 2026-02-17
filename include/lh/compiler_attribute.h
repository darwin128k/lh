#ifndef LH_COMPILER_ATTRIBUTE_H
#define LH_COMPILER_ATTRIBUTE_H

#include <lh/compiler_attribute_force_inline.h>
#include <lh/compiler_attribute_noreturn.h>
#include <lh/compiler_attribute_symbol.h>
#include <lh/compiler_attribute_target.h>
#include <lh/compiler_attribute_thread_local.h>
#include <lh/compiler_attribute_unused.h>

#define LH_COMPILER_ATTRIBUTE(name) LH_COMPILER_ATTRIBUTE_##name

#endif // LH_COMPILER_ATTRIBUTE_H
