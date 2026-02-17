#ifndef LH_COMPILER_OS_H
#define LH_COMPILER_OS_H

#ifndef LH_COMPILER_OS_UNKNOWN
#    define LH_COMPILER_OS_UNKNOWN 0
#endif // LH_COMPILER_OS_UNKNOWN

#ifndef LH_COMPILER_OS_LINUX
#    define LH_COMPILER_OS_LINUX 1
#endif // LH_COMPILER_OS_LINUX

#ifndef LH_COMPILER_OS_MAC
#    define LH_COMPILER_OS_MAC 2
#endif // LH_COMPILER_OS_MAC

#ifndef LH_COMPILER_OS_WINDOWS
#    define LH_COMPILER_OS_WINDOWS 3
#endif // LH_COMPILER_OS_WINDOWS

#ifndef LH_COMPILER_OS
#    if defined(__linux__) || defined(__linux) || defined(linux)
#        define LH_COMPILER_OS LH_COMPILER_OS_LINUX
#    elif defined(__APPLE__) && defined(__MACH__)
#        define LH_COMPILER_OS LH_COMPILER_OS_MAC
#    elif defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#        define LH_COMPILER_OS LH_COMPILER_OS_WINDOWS
#    else
#        define LH_COMPILER_OS LH_COMPILER_OS_UNKNOWN
#    endif
#endif // LH_COMPILER_OS

#endif // LH_COMPILER_OS_H