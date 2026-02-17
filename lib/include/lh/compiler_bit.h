#ifndef LH_COMPILER_BIT_H
#define LH_COMPILER_BIT_H

#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) ||             \
    defined(__aarch64__) || defined(__ppc64__) || defined(__mips64)
#    define LH_COMPILER_BIT 64
#elif defined(__ILP32__) || defined(_WIN32) || defined(__i386__) ||            \
    defined(__arm__) || defined(__mips__) || defined(__ppc__)
#    define LH_COMPILER_BIT 32
#else
#    error "Unknown compiler bit width"
#endif

#endif // LH_COMPILER_BIT_H
