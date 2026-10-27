# Software 3D renderer

`lh/ui/renderer3d.h` is a portable, allocation-free triangle rasterizer. Enable
`LH_LIBRARY_OPTION_UI=ON` and `LH_LIBRARY_OPTION_MATH_FPU=ON`. It draws into a
caller-owned ARGB8888 `lh_ui_pixmap_t`, with a separate caller-owned float depth
buffer containing at least width × height elements. No OS, window, graphics
driver, map format or game SDK is required by this renderer.

## Usage

1. Build an orthonormal camera with `lh_ui_renderer3d_camera_init`. Supply world
   position, forward direction, up direction, vertical field of view in radians,
   near distance and far distance. The world has no imposed up axis.
2. Call `lh_ui_renderer3d_begin` to validate and clear the target/depth buffers.
3. Submit world-space triangles with `lh_ui_renderer3d_triangle`, or editor/entity
   markers with `lh_ui_renderer3d_box`.
4. Present the pixmap using the existing LH surface/display facilities.

## Limits

The renderer clips against all six camera frustum planes and uses perspective
depth interpolation. Triangles are double-sided. Flat ARGB colors are currently
opaque replacements; texture sampling, lightmaps, alpha blending, animated meshes,
materials and GPU acceleration are not implemented. Invalid cameras and undersized
depth buffers are rejected; non-finite triangles are ignored. Target dimensions
are bounded to 8192 per axis. Caller buffers must remain valid for the frame.

## Relation to the GPU sink

This is an additional CPU backend, not a replacement for `lh_ui_viewport3d`'s
existing GPU sink interface. It works with the same general idea of a 3D pane,
but its buffer ownership is explicit and it does not create native GPU contexts.

## Scope

Game-specific adapters load geometry outside LH and submit generic primitives.
For example, a BSP reader belongs to its application/library, not this renderer.
Navigation, agents and network protocols also remain outside LH.

## Tests

The `renderer3d.*` GoogleTest cases belong to `lh_test_ui` when UI and MATH_FPU
are enabled. They cover depth ordering, behind-camera rejection, near/side
clipping, buffer bounds and camera validation.
