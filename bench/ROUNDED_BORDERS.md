# Native rounded borders

`lh_ui_canvas_stroke_box` is used by the entity renderer for a solid border with
uniform widths. It derives the inset contour from the same four corner radii as
the fill, then paints outer coverage minus inner coverage. The fill and shadow
remain the standard entity style operations; the application no longer constructs
an outline from two overlapping rounded fills.

Straight side bands are two bulk rectangle fills. Only corner bands use the
coverage-run path. This matters on a Cortex-M7: the initial row-only version made
many tiny canvas calls for the same two straight sides.

The common benchmark runner includes `BM_ui_canvas_stroke_box` for a 222x96 r10
outline, widths 1/3 and ARGB8888/RGB565. Run with:

```sh
lh_bench --benchmark_filter=BM_ui_canvas_stroke_box
```

Development observations on JLV135, Linux 4.5, ARM FDPIC GCC 13.4, `-O3`, 480x320
ARGB8888, continuous page swipes in the retained-widget demo:

| Version | Reported software output FPS |
| --- | --- |
| Initial native outline, row-only | 38.3 / 37.3 |
| Bulk straight bands | 40.1 / 40.1 |

These are five-second reports from two short device runs, not a precision claim
for every scene. The earlier hand-painted demo reached about 46 FPS and did less
widget processing. Migrating to native widgets is an architectural change, not a
claim that rendering got faster. Further profiling is needed.

The primitive is checked pixel by pixel against radius coverage for zero, thin
and consuming widths, independent corners (including a square corner), colour
alpha, rectangular clips, and fractional coordinates in an FPU build. A native
entity with gradient plus outline is also checked for clear outside corners,
visible border and an intact gradient interior. The complete integer UI suite
passes on Linux and Windows/Wine (658 tests).
