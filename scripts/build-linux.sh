#!/usr/bin/env sh
set -eu

config="Release"
build_dir="build/linux"
generator=""
c_compiler=""
cxx_compiler=""
target=""
shared="ON"
docs="ON"
tests="ON"
clean=0
run_tests=0
parallel="$(getconf _NPROCESSORS_ONLN 2>/dev/null || printf '1')"

usage() {
    cat <<'USAGE'
Usage: scripts/build-linux.sh [options]

Options:
  --config <name>      Debug, Release, RelWithDebInfo, or MinSizeRel (default: Release)
  --build-dir <path>   Build directory inside the repo (default: build/linux)
  --generator <name>   CMake generator, for example Ninja
  --cc <path>          C compiler passed to CMake
  --cxx <path>         C++ compiler passed to CMake
  --target <name>      Build a specific target
  --shared             Build shared library (default)
  --static             Build static library
  --docs               Configure documentation target (default)
  --no-docs            Disable documentation target
  --tests              Build tests (default)
  --no-tests           Disable tests
  --clean              Remove build directory before configuring
  --run-tests          Run ctest after build
  --parallel <n>       Parallel build jobs
  -h, --help           Show this help
USAGE
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --config)
            config="$2"
            shift 2
            ;;
        --build-dir)
            build_dir="$2"
            shift 2
            ;;
        --generator)
            generator="$2"
            shift 2
            ;;
        --cc)
            c_compiler="$2"
            shift 2
            ;;
        --cxx)
            cxx_compiler="$2"
            shift 2
            ;;
        --target)
            target="$2"
            shift 2
            ;;
        --shared)
            shared="ON"
            shift
            ;;
        --static)
            shared="OFF"
            shift
            ;;
        --docs)
            docs="ON"
            shift
            ;;
        --no-docs)
            docs="OFF"
            shift
            ;;
        --tests)
            tests="ON"
            shift
            ;;
        --no-tests)
            tests="OFF"
            shift
            ;;
        --clean)
            clean=1
            shift
            ;;
        --run-tests)
            run_tests=1
            shift
            ;;
        --parallel)
            parallel="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            printf 'Unknown option: %s\n\n' "$1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

case "$config" in
    Debug|Release|RelWithDebInfo|MinSizeRel) ;;
    *)
        printf 'Unsupported config: %s\n' "$config" >&2
        exit 2
        ;;
esac

script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
repo_root="$(CDPATH= cd -- "$script_dir/.." && pwd)"
build_path="$(mkdir -p "$(dirname -- "$repo_root/$build_dir")" && cd -- "$repo_root/$build_dir/.." && pwd)/$(basename -- "$build_dir")"

case "$build_path" in
    "$repo_root"/*) ;;
    *)
        printf 'Build directory must be inside the repository: %s\n' "$build_path" >&2
        exit 2
        ;;
esac

if [ "$build_path" = "$repo_root" ]; then
    printf 'Build directory cannot be the repository root.\n' >&2
    exit 2
fi

if [ "$clean" -eq 1 ] && [ -e "$build_path" ]; then
    printf 'Removing %s\n' "$build_path"
    rm -rf -- "$build_path"
fi

if [ -n "$generator" ]; then
    set -- -S "$repo_root" -B "$build_path" -G "$generator"
else
    set -- -S "$repo_root" -B "$build_path"
fi

set -- "$@" \
    "-DCMAKE_BUILD_TYPE=$config" \
    "-DLH_BUILD_SHARED=$shared" \
    "-DLH_BUILD_DOCS=$docs" \
    "-DLH_BUILD_TESTS=$tests"

if [ -n "$c_compiler" ]; then
    set -- "$@" "-DCMAKE_C_COMPILER=$c_compiler"
fi

if [ -n "$cxx_compiler" ]; then
    set -- "$@" "-DCMAKE_CXX_COMPILER=$cxx_compiler"
fi

printf 'Configuring lh (%s)\n' "$config"
cmake "$@"

printf 'Building lh (%s)\n' "$config"
if [ -n "$target" ]; then
    cmake --build "$build_path" --config "$config" --parallel "$parallel" --target "$target"
else
    cmake --build "$build_path" --config "$config" --parallel "$parallel"
fi

if [ "$run_tests" -eq 1 ]; then
    if [ "$tests" != "ON" ]; then
        printf 'run-tests requires tests to be enabled.\n' >&2
        exit 2
    fi

    printf 'Running tests\n'
    ctest --test-dir "$build_path" -C "$config" --output-on-failure
fi
