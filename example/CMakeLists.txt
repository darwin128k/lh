# Small programs showing lh at work. The window example left with the drawing
# engine; add the next one here when there is an API to show.

add_executable(lh_example_window_stats window_stats.c)
target_link_libraries(lh_example_window_stats PRIVATE lh)

# window_stats.c renders its own status line via Win32 GDI directly; it does not
# need any extra UI source on top of the core library, so the target stays the
# same size whatever LH_LIBRARY_OPTION_GL is set to.

# The gx examples share one screen of controls (gx_controls.c) and show it two
# ways: in a browser (gx_web.c, the web host) and in a desktop window
# (gx_window.c, the system's window and surface). Both need gx (the GL option)
# and the OS; the window needs the window system as well.
if (LH_LIBRARY_OPTION_GL AND LH_LIBRARY_OPTION_OS)
    add_executable(lh_example_gx_web gx_web.c gx_controls.c)
    target_link_libraries(lh_example_gx_web PRIVATE lh)
    if (LH_LIBRARY_OPTION_OS_WINDOW)
        add_executable(lh_example_gx_window gx_window.c gx_controls.c)
        target_link_libraries(lh_example_gx_window PRIVATE lh)
    endif ()
endif ()
