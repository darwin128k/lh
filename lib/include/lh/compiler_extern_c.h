#ifndef LH_COMPILER_EXTERN_C_H
#define LH_COMPILER_EXTERN_C_H

#include <lh/compiler_cxx.h>
#include <lh/compiler_extern.h>

#ifndef LH_COMPILER_EXTERN_C

#    ifdef LH_COMPILER_CXX
#        define LH_COMPILER_EXTERN_C LH_COMPILER_EXTERN "C"
#    else
#        define LH_COMPILER_EXTERN_C
#    endif

#endif // LH_COMPILER_EXTERN_C

#ifndef LH_COMPILER_EXTERN_C_BEGIN
#    ifdef LH_COMPILER_CXX
#        define LH_COMPILER_EXTERN_C_BEGIN LH_COMPILER_EXTERN_C {
#    else
#        define LH_COMPILER_EXTERN_C_BEGIN
#    endif
#endif // LH_COMPILER_EXTERN_C_BEGIN

#ifndef LH_COMPILER_EXTERN_C_END
#    ifdef LH_COMPILER_CXX
#        define LH_COMPILER_EXTERN_C_END }
#    else
#        define LH_COMPILER_EXTERN_C_END
#    endif
#endif // LH_COMPILER_EXTERN_C_END

#endif // LH_COMPILER_EXTERN_C_H
