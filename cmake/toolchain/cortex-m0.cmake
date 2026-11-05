# Cross-compile LH for an ARM Cortex-M0 / M0+ board without an OS
# (arm-none-eabi GCC, for example the Arm GNU Toolchain).
#
#   cmake -S . -B build-m0 -G Ninja \
#         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain/cortex-m0.cmake \
#         -DLH_LIBRARY_OPTION_OS=OFF -DLH_LIBRARY_OPTION_ATOMIC=CRITICAL \
#         -DLH_LIBRARY_OPTION_REAL=NONE -DLH_LIBRARY_OPTION_THREAD_LOCAL=OFF \
#         -DLH_POSITION_INDEPENDENT_CODE=OFF -DLH_BUILD_TESTS=OFF
#   cmake --build build-m0 --target lh
#
# The M0 has no atomic read-modify-write instructions, hence ATOMIC=CRITICAL; it
# has no FPU, hence REAL=NONE; it has no thread-local storage and runs from a
# fixed address, hence THREAD_LOCAL and position-independent code OFF. The
# firmware links libgcc and provides memcpy, memmove, memset and strlen, which
# the compiler may emit for copies and loops. Only the library is built here:
# nothing runs. The rail base for such a board is lh/rail/bare.h.
# Set LH_ARM_TOOLCHAIN_DIR when the compiler is not on PATH.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0)

set(LH_ARM_TOOLCHAIN_DIR "" CACHE PATH "Directory of arm-none-eabi-gcc (empty: PATH)")
if (LH_ARM_TOOLCHAIN_DIR)
    set(_lh_arm_prefix "${LH_ARM_TOOLCHAIN_DIR}/")
else ()
    set(_lh_arm_prefix "")
endif ()

if (CMAKE_HOST_WIN32)
    set(_lh_arm_suffix ".exe")
else ()
    set(_lh_arm_suffix "")
endif ()

set(CMAKE_C_COMPILER "${_lh_arm_prefix}arm-none-eabi-gcc${_lh_arm_suffix}")
set(CMAKE_CXX_COMPILER "${_lh_arm_prefix}arm-none-eabi-g++${_lh_arm_suffix}")
set(CMAKE_ASM_COMPILER "${_lh_arm_prefix}arm-none-eabi-gcc${_lh_arm_suffix}")

# No OS to link a test executable against: try_compile builds a static library.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m0 -mthumb -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_INIT "-mcpu=cortex-m0 -mthumb -ffunction-sections -fdata-sections")
