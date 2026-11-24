# Small programs showing lh at work. The window example left with the drawing
# engine; add the next one here when there is an API to show.

# window_stats.c renders its own status line via Win32 GDI directly; it does not
# need any extra UI source on top of the core library, so the target stays the
# same size whatever LH_LIBRARY_OPTION_GL is set to. It opens a window, so it
# needs the OS and its window system.
if (LH_LIBRARY_OPTION_OS AND LH_LIBRARY_OPTION_OS_WINDOW)
    add_executable(lh_example_window_stats window_stats.c)
    target_link_libraries(lh_example_window_stats PRIVATE lh)
endif ()
