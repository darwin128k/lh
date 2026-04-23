# ---------------------------------------------------------------------------
# lh_read_version(<file> <out_major> <out_minor> <out_patch>)
#
# Reads a VERSION file (format: X.Y.Z) and sets the given variables.
#
# Example:
#   lh_read_version("${CMAKE_SOURCE_DIR}/VERSION"
#       LH_VERSION_MAJOR LH_VERSION_MINOR LH_VERSION_PATCH)
# ---------------------------------------------------------------------------
function(lh_read_version file out_major out_minor out_patch)
    file(READ "${file}" _version)
    string(STRIP "${_version}" _version)
    string(REPLACE "." ";" _version "${_version}")
    list(GET _version 0 _major)
    list(GET _version 1 _minor)
    list(GET _version 2 _patch)
    set(${out_major} "${_major}" PARENT_SCOPE)
    set(${out_minor} "${_minor}" PARENT_SCOPE)
    set(${out_patch} "${_patch}" PARENT_SCOPE)
endfunction()
