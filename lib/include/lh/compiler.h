#ifndef LH_COMPILER_H
#define LH_COMPILER_H

#include <lh/compiler_attribute.h>
#include <lh/compiler_constructor.h>
#include <lh/compiler_destructor.h>
#include <lh/compiler_extern_c.h>
#include <lh/compiler_os.h>

#define LH_COMPILER(name) LH_COMPILER_##name

#endif // LH_COMPILER_H
