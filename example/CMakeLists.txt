# Small programs showing lh at work. The window example left with the drawing
# engine; add the next one here when there is an API to show.

add_executable(lh_example_window_stats window_stats.c)
target_link_libraries(lh_example_window_stats PRIVATE lh)

# window_stats.c renders its own status line via Win32 GDI directly; it does not
# need any extra UI source on top of the core library, so the target stays the
# same size whatever LH_LIBRARY_OPTION_UI is set to.
