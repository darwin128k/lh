# Cross-compile LH for an ARM Cortex-M4 board without an OS
# (arm-none-eabi GCC, for example the Arm GNU Toolchain).
#
#   cmake -S . -B build-m4 -G Ninja \
#         -DCMAKE_TOOLCHAIN_FILE=$PWD/cmake/toolchain/cortex-m4.cmake \
#         -DLH_LIBRARY_OPTION_OS=OFF -DLH_LIBRARY_OPTION_THREAD_LOCAL=OFF \
#         -DLH_POSITION_INDEPENDENT_CODE=OFF -DLH_BUILD_TESTS=OFF
#   cmake --build build-m4 --target lh
#
# The M4 has the atomic instructions the M0 lacks, so the default atomics do;
# it has no thread-local storage and runs from a fixed address, hence
# THREAD_LOCAL and position-independent code OFF. The float ABI is soft, so
# the library links with a firmware built either way. Programs link against
# newlib-nano with no system calls (nano.specs, nosys.specs) and drop what they
# do not reach (--gc-sections): what bench/mcu/ measures.
# Set LH_ARM_TOOLCHAIN_DIR when the compiler is not on PATH.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

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
set(CMAKE_SIZE "${_lh_arm_prefix}arm-none-eabi-size${_lh_arm_suffix}")
set(CMAKE_NM "${_lh_arm_prefix}arm-none-eabi-nm${_lh_arm_suffix}")

# No OS to link a test executable against: try_compile builds a static library.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(_lh_m4_cpu "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft")
set(CMAKE_C_FLAGS_INIT "${_lh_m4_cpu} -fno-pic -fno-pie -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_INIT "${_lh_m4_cpu} -fno-pic -fno-pie -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS_INIT
    "${_lh_m4_cpu} -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs")
