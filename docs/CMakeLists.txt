# Find Doxygen and skip docs target if unavailable.
find_package(Doxygen)

if (NOT DOXYGEN_FOUND)
    message(STATUS "Doxygen not found — docs target skipped")
    return()
endif ()

# Optional Graphviz `dot` integration for dependency/call graphs.
find_program(DOT_EXECUTABLE dot)

option(LH_DOCS_GRAPHS "Generate documentation graphs (requires Graphviz)" ON)

if (LH_DOCS_GRAPHS)
    if (DOT_EXECUTABLE)
        set(DOXYGEN_HAVE_DOT YES)
        get_filename_component(DOXYGEN_DOT_PATH "${DOT_EXECUTABLE}" DIRECTORY)
        message(STATUS "Docs graphs: enabled")
    else ()
        set(DOXYGEN_HAVE_DOT NO)
        set(DOXYGEN_DOT_PATH "")
        message(WARNING "LH_DOCS_GRAPHS is ON but Graphviz not found — graphs disabled")
    endif ()
else ()
    set(DOXYGEN_HAVE_DOT NO)
    set(DOXYGEN_DOT_PATH "")
    message(STATUS "Docs graphs: disabled")
endif ()

set(PROJECT_VERSION "${LH_VERSION_MAJOR}.${LH_VERSION_MINOR}.${LH_VERSION_PATCH}")

# Generate Doxyfile from template with project-specific values.
configure_file(Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

# Main documentation target.
add_custom_target(lh_docs
        COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Generating documentation"
        VERBATIM
)

# README.md references images as docs/assets/...; HTML is written to
# ${CMAKE_SOURCE_DIR}/docs/html/, so the same relative path must exist under
# docs/html/ (i.e. docs/html/docs/assets/...). Doxygen does not mirror that tree
# for Markdown <img> tags, so copy the assets directory after generation.
if (EXISTS "${CMAKE_SOURCE_DIR}/docs/assets")
    add_custom_command(
            TARGET lh_docs
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/docs/html/docs"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/docs/assets"
            "${CMAKE_SOURCE_DIR}/docs/html/docs/assets"
            COMMENT "Copying docs/assets into HTML output (README images)"
            VERBATIM)
endif ()
