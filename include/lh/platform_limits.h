#ifndef LH_PLATFORM_LIMITS_H
#define LH_PLATFORM_LIMITS_H

#include <limits.h>

#ifndef LH_CHAR_BIT

#    ifdef CHAR_BIT
#        define LH_CHAR_BIT CHAR_BIT
#    else
#        error "CHAR_BIT is not defined"
#    endif // CHAR_BIT

#endif // LH_CHAR_BIT

#endif // LH_PLATFORM_LIMITS_H
