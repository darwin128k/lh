# Small programs showing lh at work; each is also the first real caller of the
# API it uses. Built with -DLH_BUILD_EXAMPLES=ON.

if (LH_LIBRARY_OPTION_OS_WINDOW)
    add_executable(lh_example_entity_window src/entity_window.c)
    target_link_libraries(lh_example_entity_window PRIVATE ${PROJECT_NAME})
endif ()
