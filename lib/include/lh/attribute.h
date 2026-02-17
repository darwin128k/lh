#ifndef LH_ATTRIBUTE_H
#define LH_ATTRIBUTE_H

#include <lh/attribute_force_inline.h>
#include <lh/attribute_noreturn.h>
#include <lh/attribute_symbol.h>
#include <lh/attribute_target.h>
#include <lh/attribute_thread_local.h>
#include <lh/attribute_unused.h>

#define LH_ATTRIBUTE(name) LH_ATTRIBUTE_##name

#endif // LH_ATTRIBUTE_H
