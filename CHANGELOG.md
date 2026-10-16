# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- AES-NI for `lh_crypto_rijndael_t` (`lh/crypto/rijndael.h`): 16-byte (AES) blocks encrypt and decrypt with `AESENC` / `AESDEC` when the CPU has AES-NI, in every mode and for 16/24/32-byte keys; 24/32-byte Rijndael blocks and CPUs without AES-NI keep the table path, same bytes. `lh_crypto_rijndael_init` also prepares the `AESDEC` round keys (`AESIMC`) once, so decrypt runs as fast as encrypt; the session gains `aesni_dec_key` (240 bytes) when `LH_LIBRARY_OPTION_SIMD_HAVE_AESNI` is on, so library and caller must share one `lh/config.h`. About 32x faster encrypt and 70x faster decrypt per block on an i7-3770K (AES-128: 765 → 24 ns, 1661 → 24 ns)
- `lh_cpu_simd_has_aesni` (`lh/cpu/simd.h`) and `LH_LIBRARY_OPTION_SIMD_HAVE_AESNI` (`lh/config.h`, compile-only probe in `cmake/check_simd.cmake`): runtime and toolchain checks for AES-NI
- FIPS-197 known answers for AES-128/192/256 (encrypt and decrypt) and SP 800-38A first blocks for CBC and CFB in `test/src/lh/crypto/rijndael.cpp`
- Third-party notices in `LICENSE.md` with license texts in `LICENSES/` (`Apache-2.0.txt` for the Roboto font compiled into `lh/ui/font/roboto.c`, `Unicode-3.0.txt` for the Unicode Character Database 16.0.0 under `tools/data/unicode/`); `requirements.txt` pins Pillow for the `scripts/` generators
- CI: Windows legacy jobs build and test through `scripts/build-windows-xp.bat` (x86) and `build-windows-vista.bat` (x64) at the XP / Vista API level on a modern MSVC (compile and test only, not XP / Vista-deployable binaries)
- `lh_test_ui` checks `lh/ui.h` from C++ and as C11 (`test/src/lh/ui/umbrella.cpp`, `umbrella.c`)
- `lh_ui_layout_run` and its bricks (`lh/ui/layout.h`): `lh_ui_layout_run_t` / `lh_ui_layout_cross_t`, `lh_ui_layout_run_init`, `lh_ui_layout_run_measure_child`, `lh_ui_layout_run_measure`, `lh_ui_layout_justify_lead`, `lh_ui_layout_run_justify`, `lh_ui_layout_run_length`, `lh_ui_layout_run_place_child`, `lh_ui_layout_run_place`, `lh_ui_layout_cross_fit`, `lh_ui_layout_cross_at`, `lh_ui_layout_child_line`: one line of the flow measured, justified and placed a child at a time
- `lh_ui_canvas_box_frame_t`, `lh_ui_canvas_box_frame_init`, `lh_ui_canvas_box_corners_init`, `lh_ui_canvas_box_row_is_upper`, `lh_ui_canvas_box_zone`, `lh_ui_canvas_box_row_reach`, `lh_ui_canvas_box_rows` (`lh/ui/canvas/box.h`): the shown edges of a box and its rows, corners and bands
- `lh_ui_blur_pass_t`, `lh_ui_blur_pass_init`, `lh_ui_blur_column`, `lh_ui_blur_window_span`, `lh_ui_blur_window_take_samples`, `lh_ui_blur_window_drop_samples`, `lh_ui_blur_window_average_color`, `lh_ui_blur_sample_color` (`lh/ui/blur.h`): the column pass one column at a time
- `lh_ui_canvas_sw_shadow_pass_t`, `lh_ui_canvas_sw_shadow_pass_init`, `lh_ui_canvas_sw_shadow_row`, `lh_ui_canvas_sw_shadow_span` (`lh/ui/canvas/sw.h`): the software box shadow a row at a time
- `lh_math_quat_axis_x` / `_y` / `_z` and `lh_math_quat_axis_cyclic` (`lh/math/quat.h`): where a unit quaternion takes each axis; `lh_math_mat4_set_column_vec3` (`lh/math/mat4.h`)
- `lh_ui_gradient_sampler_lerp_t` with `lh_ui_gradient_sampler_lerp_init` / `_next` / `_color`, `lh_ui_gradient_sampler_stop_frac`, `lh_ui_gradient_sampler_segment_at`, `lh_ui_gradient_sampler_is_flat`, `lh_ui_gradient_sampler_flat_run`, `lh_ui_gradient_sampler_linear_run`, `lh_ui_gradient_sampler_radial_row` (`lh/ui/gradient/sampler.h`)
- `lh_math_scalar_slack`, `lh_math_scalar_centre` (`lh/math/scalar/centre.h`): non-negative leftover room and half-up centre offset `(room - own + 1) / 2` on discrete scalars
- `lh_math_div_round` (`lh/math/ops.h`): half-up integer division `(numer + denom / 2) / denom` for non-negative values
- `lh_math_size_min`, `lh_math_size_max`, `lh_math_size_min_side` (`lh/math/size.h`) and `lh_math_fsize_*` counterparts: component-wise min/max and the shorter side
- `lh_ui_scalar_slack`, `lh_ui_scalar_centre` (`lh/ui/scalar.h`): UI leftovers and half-up centring through `lh_ui_scalar_trunc`, same pixels with FPU on or off
- `lh_math_slice_t` (`lh/math/slice.h`) and `lh_math_view_t` (`lh/math/view.h`): read-only closed slice and strided view of `lh_float_t` values, built on `lh_memory_view_slice_fields` and `lh_interval_closed_*`
- `LH_FLOAT_T_MIN`, `LH_FLOAT_T_MAX` and `LH_FLOAT_T_EPS` (`lh/numeric/float.h`): `FLT_MIN` (smallest positive normalized value), `FLT_MAX` and `FLT_EPSILON` limits for `lh_float_t`
- `lh_memory_view_return_of_slice`, `lh_memory_view_return_empty`, `lh_memory_view_return_init_v`, `lh_memory_view_return_init_by_size` (`lh/memory/view.h`): half-open view constructors and the closed-to-half-open conversion
- `lh_memory_view_slice_return_empty`, `lh_memory_view_slice_return_init_v`, `lh_memory_view_slice_return_init_by_size`, `lh_memory_view_slice_find_offset_from_ptr` (`lh/memory/view/slice.h`): previously file-local bricks, now public
- `lh_memory_view_slice_is_ropen_empty`, `lh_memory_view_slice_is_unclosable_ropen`, `lh_memory_view_slice_from_view`, `lh_memory_view_slice_contains_ropen`, `lh_memory_view_slice_overlaps_ropen`, `lh_memory_view_slice_find_ropen`, `lh_memory_view_slice_rfind_ropen`, `lh_memory_view_slice_compare_ropen`, `lh_memory_view_slice_rcompare_ropen`, `lh_memory_view_slice_take_first_ropen`, `lh_memory_view_slice_take_last_ropen`, `lh_memory_view_slice_from_offset_ropen`: half-open cases a closed slice cannot store (`[x, x)` and an empty or null argument range)
- `lh_memory_view_slice_get_ropen_last`, `lh_memory_view_slice_get_ropen_past`, `lh_memory_view_slice_get_ropen_end`, `lh_memory_view_slice_get_ropen_size`, `lh_memory_view_slice_needs_ropen`, `lh_memory_view_slice_return_of_ropen_v`, `lh_memory_view_slice_ropen_contains_range`, `lh_memory_view_slice_ropen_overlaps_range`, `lh_memory_view_slice_assert_ropen_not_empty`, `lh_memory_view_slice_get_ropen_view_size`, `lh_memory_view_slice_assert_sub_range`, `lh_memory_view_slice_get_ropen_ptr`, `lh_memory_view_slice_return_ropen_sub`, `lh_memory_view_return_init_of_slice`: short bricks the half-open view path is built from
- `lh_math_vec4_madd` (`lh/math/vec4.h`) and `lh_math_vec3_madd` (`lh/math/vec3.h`): multiply-accumulate `a + b * s` into `out`; `out` may be the storage `a` or `b` was copied from

### Changed

- Test / bench dependencies updated to the newest releases that still build as C++14: GoogleTest v1.13.0 → v1.16.0; Google Benchmark moved from the `darwin128k/benchmark` fork (v1.7.1 + a `<cstdint>` fix) back to upstream v1.8.5, which no longer needs the fix
- CI pinned: `actions/checkout` v4 → v7.0.1 and `ilammy/msvc-dev-cmd` v1.13.0 by commit SHA; runners `ubuntu-24.04` and `windows-2025-vs2026` instead of `-latest` (the images `-latest` points to today)
- `lh/ui.h` is now a complete umbrella: it also includes `lh/ui/canvas/box.h`, `canvas/web.h`, `display.h`, `display/fields.h`, `gradient/sampler.h`, `gradient/sampler/fields.h`, `gradient/sine.h`, `present.h`, `present/sink.h`, `present/sink/fields.h`, `viewport3d.h`, `web/buf.h`, `web/html.h`, `web/html/attr.h`, `web/js.h`, `web/js/x.h`, `web/page.h`, `web/ws.h`, `web/x.h`, plus `font/roboto.h` when `LH_LIBRARY_OPTION_UI_FONT_ROBOTO` is on and `web/host.h` when `LH_LIBRARY_OPTION_OS` is on
- `lh_byte_ptr` / `lh_byte_cptr` moved to `lh/util/byte/ptr.h`; `lh/byte/ptr.h` stays as a compatibility header that only includes it, and the new `lh/util/byte.h` pulls in the pointer and byte-pointer helpers in one include
- `lh_ptr_of` / `lh_cptr_of` live in `lh/util/ptr.h`; `lh/byte/ptr.h` includes `lh/util/ptr.h` one-way (`lh/util/ptr.h` no longer includes `lh/byte/ptr.h`), breaking the include cycle; files using `lh_byte_ptr` / `lh_byte_cptr` include `lh/byte/ptr.h` directly
- `lh_bit_*` operator macros (`lh_bit_mask`, `_and`, `_disjoint`, `_or`, `_set`, `_xor`, `_not`, `_shl`, `_shr`) moved from `lh/bit.h` to their own `lh/bit/ops.h`; `lh/bit.h` is now a pure umbrella over `lh/bit/bswap.h`, `endian.h`, `ops.h`, `packed.h`, `rotate.h` and `scan.h`. `lh/fs/attr.h`, `lh/fs/perm.h`, `lh/logger/level/flags.h`, `lh/memory/view/slice/flags.h` and `lh/util/type.h` include `lh/bit/ops.h` instead of the umbrella
- Operator macros (arithmetic, comparison / selection, bitwise, overflow checks, `lh_math_div_round`) moved from `lh/math.h` to their own `lh/math/ops.h`; `lh/math.h` is now a pure umbrella and also pulls in `lh/math/floor.h` and `lh/math/rescale.h`. Headers under `lh/math/` include `lh/math/ops.h` instead of the umbrella, which removes the include cycle; `lh/math/div_round.h` is gone (`lh_math_div_round` lives in `lh/math/ops.h`)
- One formula, one place: removed `lh_math_div_round_u32` (it only forwarded to `lh_math_div_round`; pass `lh_u32_t` operands for unsigned semantics); `lh_math_rescale_u32` and `lh_ui_color_blend_channel` now use `lh_math_div_round` instead of open-coding the half-add; new generic `lh_math_slack` / `lh_math_centre_half` (`lh/math/scalar/centre.h`) back both `lh_math_scalar_slack` / `_centre` and `lh_ui_scalar_slack` / `_centre`; the layout cross slot uses `lh_ui_scalar_slack`; `lh_math_size_*` and `lh_math_fsize_*` min / max / min_side share `lh_math_size_pairwise` / `lh_math_size_min_side_of` (`lh/math/size/ops.h`). Results are bit-identical with FPU on and off

- Heavy functions split into the bricks above, same pixels and results with `LH_LIBRARY_OPTION_MATH_FPU` on and off: the layout pass (`layout_run`, 32 locals → `lh_ui_layout_run`, 2), `lh_ui_canvas_fill_box` (21 → 7), `lh_ui_blur_columns` (23 → 2), `lh_ui_canvas_sw_shadow` (19 → 3), `lh_math_mat4_from_quat` (13 → 1), the gradient sampler's table fill (21 → 8) and row / column (13 → 2, 8 → 3, one shared linear walk)
- Layout / text centring and end-align leftover use `lh_ui_scalar_centre` / `lh_ui_scalar_slack` (`lh/ui/layout`, `lh/ui/text/align`, `lh/ui/text/line`, justify-center lead); blur / shadow / radius coverage use `lh_math_div_round`; gradient length and byte clamp use `lh_math_abs` / `lh_math_clamp`; radius max and insets shrink use `lh_ui_size_min_side` / `lh_ui_scalar_slack`
- `lh_ui_scalar_trunc` (`lh/ui/scalar.h`): on the int path (`LH_LIBRARY_OPTION_MATH_FPU` off) it is now a force-inline copy in the header, so call sites pay no function call in Debug or without LTO; same signature, null check and address-takeable; the float path stays an exported function. Covered by new tests in `test/src/lh/ui/scalar.cpp`
- Pure forward wrappers no longer assert `out` themselves when the function they forward it to already does (40 functions in `lh/math`, `lh/memory`, `lh/str`, `lh/wstr`, `lh/os`, `lh/runtime`, `lh/werror`, `lh/timestamp`, `lh/ui`); a null `out` is still caught, by the callee
- `lh_math_mat4_mul_vec4`, `lh_math_mat4_mul`, `lh_math_quat_rotate` and `lh_math_quat_nlerp` use fewer temporaries (10 → 1, 5 → 1, 7 → 2, 4 → 2), built from `lh_math_vec4_madd` / `lh_math_vec3_madd`; `lh_math_mat4_mul` stays safe when `out` is where `a` or `b` came from
- `lh_memory_view_slice_*_ropen` and `lh_memory_view_slice_from_view` are built from short public bricks. Signatures and results stay the same, including empty `[x, x)` and the order of range checks
- `lh_memory_view_*` is a thin wrapper over `lh_memory_view_slice_*`. Signatures and results stay the same; a non-empty view `[begin, end)` is the closed slice `[begin, end - 1]`, and a valid empty view `[x, x)` is handled before that conversion. `lh_memory_view_is_aligned` still checks the exclusive end. `lh_memory_view_set_by_size` stores `begin + size`, not the closed `begin + size - 1`. `lh_memory_view_between` can return `[p, p)` when that empty sub-range is contained. Find, rfind, compare and rcompare of an empty or null needle stay on the half-open raw path

- `lh/float/*` (`lh_float_sqrt`, `lh_float_sin_cos`, `lh_float_floor_to_int`, `lh_float_ceil_to_int`) became public functions in `.c` with headers in `include/lh/float/`
- `lh/float` moved to `lh/math/float`; `lh_float_sqrt`, `lh_float_sin_cos`, `lh_float_floor_to_int`, `lh_float_ceil_to_int` renamed to `lh_math_float_sqrt`, `lh_math_float_sin_cos`, `lh_math_float_floor_to_int`, `lh_math_float_ceil_to_int`
- `lh_math_vec2/3/4_dot`, `_length_sq` and `_length` compute through `lh_math_view_*` over the vector components (same results, bit for bit)
- RGB565 and ARGB8888 alpha blending without SSE (ARM) works on words: two channels per multiply in `0x00FF00FF` lanes, an exact divide-free `/255`, `SMLAD` on cores with the DSP extension, and RGB565 pixels are processed in pairs (two 16-bit accesses, which the compiler may merge); results unchanged bit for bit

### Fixed

- Roboto copyright year in `lh/ui/font/roboto.h`, `src/lh/ui/font/roboto.c` and the `scripts/README.md` font example: 2015, matching the font file (was 2011)
- Building with `LH_LIBRARY_OPTION_MATH_FPU=ON` failed: `lh_ui_scalar` (a literal-only macro that pastes `e0f`) was given variables and calls in `lh/ui` and its tests; those sites now use the new `lh_ui_scalar_of` (`lh/ui/scalar.h`), a cast for any arithmetic expression on both paths. `lh_ui_view_t::whole_count` and its loop index are `lh_u32_t` instead of `lh_ui_scalar_t` (a float cannot index an array)
- `lh_ui_point_to_point` / `lh_ui_point_from_point` and `lh_ui_rect_to_rect` / `lh_ui_rect_from_rect` take an out-parameter on the `MATH_FPU=OFF` path too, matching the float path
- `test/src/lh/os/serial.cpp` called `lh_os_serial_get_stream` with its old by-value signature
- With `LH_LIBRARY_OPTION_MATH_FPU=ON`, alignment and layout landed on half pixels where the int path truncates (centring in `lh_ui_text_align_get_origin` and `lh_ui_text_line_get_shift`, `justify` center / space-*, fill shares by weight and the cross room shared between wrapped lines in `lh/ui/layout`). Those divisions now go through the new `lh_ui_scalar_trunc` (`lh/ui/scalar.h`), which drops the fraction toward zero on the float path and copies on the int path, so both paths place the same pixels

## [0.3.0] - 2026-09-14

### Added

- Read-only closed-byte-slice type `lh_memory_view_slice_t` (`lh/memory/view/slice.h`) with full API: unpack, flags, direction, size, containment, offset distance, overlap, alignment checks, indexed byte access, and slicing
- Supporting headers for view slice: `lh/memory/view/slice/direction.h`, `flags.h`, `fields.h`, `initializer.h`
- `lh/attribute/static.h` and `lh/compiler/attribute/static.h` — static attribute helpers
- `lh/str/view/initializer.h` — brace-enclosed initializer macros for `lh_str_view_t`
- String view search API: `lh_str_view_find_char`, `lh_str_view_find`, `lh_str_view_rfind` (with optional case-ignore flag)
- String view character-set search: `lh_str_view_find_of`, `lh_str_view_rfind_of`, `lh_str_view_find_not_of`, `lh_str_view_rfind_not_of`
- String view comparison API: `lh_str_view_compare`, `lh_str_view_rcompare`, `lh_str_view_equals` (with optional case-ignore flag)
- `lh_str_view_find_null_terminator`, `lh_str_view_is_null_terminated` — null-terminator detection
- `lh_str_view_clear`, `lh_str_view_swap` — mutating helpers for string views
- Raw string character-set search: `lh_str_ptr_find_of_chars`, `lh_str_ptr_rfind_of_chars`, `lh_str_ptr_find_not_of_chars`, `lh_str_ptr_rfind_not_of_chars`
- Member-offset utility macro (`lh_offset_of`) for compile-time layout checks
- Runtime check option (`LH_LIBRARY_OPTION_RUNTIME_CHECK_REF`) to enable null-pointer checks in `lh_runtime_check_ref`
- Bounds convenience/fallback APIs: `lh_memory_bounds_init_by_empty`, `lh_memory_bounds_make_by_empty`, `lh_memory_bounds_make_or_empty`, and `lh_memory_bounds_slice_or_empty`
- Expanded test coverage for bounds validated and fallback flows (`*_v`, `*_or_empty`, slice geometry and death cases)
- Subsystem detection macros for Windows-specific visibility attributes (`lh/subsys.h`)
- Macro utilities (`lh/util/arg.h`): `lh_arg_concat` for token concatenation, `lh_arg_get_count` for variadic argument counting (1-10)
- Runtime assertion macros (`lh/assert.h`): `lh_assert_runtime`, `lh_assert_runtime_if`, `lh_assert_runtime_ifn`, `lh_assert_runtime_ref` — conditional throw with three variants (code only, message only, code + message)
- Raise dispatch (`lh/runtime/raise.h`): `lh_runtime_raise` macro with auto-detecting argument pattern via `_Generic`
- Size-aware string comparison (`lh/str/view.h`, `lh/util/wstr/ptr.h`): `lh_str_ptr_compare_size`, `lh_wstr_ptr_compare_size` for length-bounded comparison
- Wide string search functions: `lh_wstr_ptr_find_char`, `lh_wstr_ptr_find`, `lh_wstr_ptr_rfind`, `lh_wstr_ptr_compare`, `lh_wstr_ptr_equals`, with case-fold support
- Doxygen documentation for internal macro implementations (raise.h, assert.h, arg.h, fields.h)
- New growable typed array `lh_vector_t` (`lh/vector.h`): `lh_vector_init`/`deinit`, `push_back`/`push_back_of`, `insert`/`insert_of`, `erase`, `pop_back`, `reserve`, `clear`, a capacity growth policy (`lh_vector_get_grown_capacity`), and accessors (`get_size`, `get_capacity`, `get_data`, `get_ptr`, `get_begin`/`get_end`, `get_type_size`, `get_typed`/`get_typed_as_const`, `is_empty`, `is_valid_index`)
- `lh_str_t` (`lh/str.h`), an owning growable NUL-terminated string built on `lh_vector_t`: `lh_str_init`, `lh_str_deinit`, `lh_str_clear`, `lh_str_push_back`, `lh_str_append`, `lh_str_as_view`, `lh_str_get_data`/`get_size`, `lh_str_is_empty`
- `lh_memory_typed_t` (`lh/memory/typed.h`) and `lh_memory_typed_allocated_t` (`lh/memory/typed/allocated.h`): a byte range paired with an element type size, layered under `lh_memory_bounds_t`/`lh_vector_t` — indexed get/set, value-copy and value-swap helpers, retype, assign/swap, offset-from-index and index-from-offset lookups, and range-bounds retrieval
- Level-filtered logger (`lh/logger.h`): `lh_logger_init`/`init_of`, `lh_logger_pack`/`unpack`/`set`/`deinit`, per-level callback slots (`lh_logger_get_emit_cb`/`get_context`/`get_flags`), `lh_logger_log`/`lh_logger_log_v`, and level shortcut macros `lh_logger_debug`/`info`/`notice`/`warning`/`error`/`critical`/`alert`/`emergency`
- IO abstraction (`lh/io/`): callback-based `lh_io_reader_t`/`lh_io_writer_t` and a unified `lh_io_stream_t` combining both
- `lh_net_ip4_t` IPv4 address type (`lh/net/ip.h`): make/set/assign/parse/format/pack/unpack/equals, octet access, `is_loopback`/`is_private`
- `lh_net_port_t` parsing (`lh/net/port.h`)
- Generic socket address type `lh_net_socket_addr_t` (`lh/net/socket/addr.h`) with an IPv4 backing (`lh/net/socket/addr/ip4.h`)
- `lh_os_net_socket_t` (`lh/os/net/socket.h`): a blocking TCP/UDP client socket — init/open/connect/send/recv/close, plus handle/reader/writer/stream accessors
- Integer formatting: `lh_str_ptr_format_uint`, `lh_str_ptr_format_sint` (`lh/str/format/uint.h`, `sint.h`)
- Hex formatting/parsing: `lh_str_ptr_format_hex`, `lh_str_ptr_parse_hex` (`lh/str/format/hex.h`, `lh/str/parse/hex.h`)
- Byte-buffer formatting: `lh_str_ptr_format_bytes_hex`, `lh_str_ptr_format_bytes_hex_dump` (`lh/str/format/bytes.h`)
- Text formatting: `lh_str_ptr_format_text`/`_v` (`lh/str/format/text.h`)
- Header-only, force-inline decimal parsing: `lh_str_ptr_parse_uint` (`lh/str/parse/uint.h`)
- String splitting (`lh/str/split/next.h`): `lh_str_ptr_split_next`, `lh_str_ptr_find_of_char`
- Byte swap (`lh/util/bit/bswap.h`): `lh_bit_bswap_u16`/`u32`/`u64`
- Endian pack/unpack (`lh/util/bit/endian.h`): `lh_bit_pack_be16`/`le16`/`be32`/`le32`/`be64`/`le64` and matching `unpack_*`
- Bit rotate (`lh/util/bit/rotate.h`): `lh_bit_rotate_left`/`right_u32`/`u64`
- Bit scan (`lh/util/bit/scan.h`), extracted as a shared, header-only force-inline primitive: `lh_bit_scan_forward`/`reverse_u8`/`u16`/`u32`/`u64`
- Hex-digit helpers (`lh/char/xdigit.h`): `lh_char_is_xdigit`, `lh_char_to_xdigit`, `lh_char_from_xdigit`, `lh_char_xdigit_accumulate`, `lh_char_ord`/`ord_to`
- Runtime SIMD feature detection (`lh/cpu/simd.h`): `lh_cpu_simd_has_sse2`/`has_ssse3`/`has_avx2`
- CPU vendor detection (`lh/cpu/vendor.h`): `lh_cpu_vendor_is_intel`/`is_amd` (standalone utility, not currently wired into `memory/std.c`'s dispatch)
- `lh/compiler/cpu.h`: `lh_compiler_cpu_has_feature`, a portable wrapper around `__builtin_cpu_supports`
- `lh/compiler/arch.h` / `lh/compiler/arch/family.h`: `LH_COMPILER_ARCH_8/16/32/64` width constants and `LH_COMPILER_ARCH_FAMILY_IS_X86`/`_ARM`
- SSE2/SSSE3/AVX2 tiered, runtime-dispatched SIMD kernels for `lh_memory_std_copy`, `copy_rev`, `rcopy`, `set`, `compare`, `rcompare` (`lh/memory/std.h`)
- A size-gated REP MOVSB path for `lh_memory_std_copy` (MSVC `__movsb` intrinsic / GCC-Clang inline `rep movsb`)
- SSE2 non-temporal streaming stores for large copies, and prefetching ahead of the copy cursor, both above tunable size thresholds
- CMake-tunable SIMD thresholds (`LH_LIBRARY_OPTION_MEMORY_STD_SIMD_MIN_THRESHOLD`, `_SET_THRESHOLD`, `_DIRECT_DISPATCH_THRESHOLD`, `_STREAM_THRESHOLD`, `LH_LIBRARY_OPTION_MEMORY_STD_GCC_REP_MOVSB_THRESHOLD`, `_PREFETCH_TRIGGER`, `_PREFETCH_DISTANCE`, `LH_LIBRARY_OPTION_ALGORITHM_COMPARE_BLOCK`), plus a compile-time SIMD probe (`cmake/check_simd.cmake`) recording `LH_LIBRARY_OPTION_SIMD_HAVE_{SSE2,SSSE3,AVX2}`
- Compiler-portable force-inline attribute macro (`lh/attribute/force_inline.h`, `LH_ATTRIBUTE_FORCE_INLINE`)
- `lh_foreach` (`lh/foreach.h`): a generic "iterate n elements, bind each to a name" `for`-loop header macro
- Rijndael/AES block cipher (`lh/crypto/rijndael.h`, `lh/crypto/rijndael/mode.h`)
- SHA hashing (`lh/crypto/sha.h`, `lh/crypto/sha/kind.h`)
- Const accessors for exception/error/origin retrieval: `lh_exception_get_error_as_const`, `lh_exception_get_origin_as_const` (and catch-stack equivalents)
- Google Benchmark-based microbenchmark harness under `bench/` (opt-in via `LH_BUILD_BENCH`), covering `memory/std`, bounds, typed, view, vector, allocator, net, and str/wstr pointer operations

### Changed

- `lh_str_ptr_size` renamed to `lh_str_ptr_get_size` for naming consistency with the `get_` prefix convention
- `lh_memory_view` slice methods now delegate geometry to `lh_memory_view_slice`
- Slice flags and direction types (`lh_memory_view_slice_flags_t`, `lh_memory_view_slice_direction_t`) unified and shared between bounds/slice and view/slice layers
- Rename `is_back` parameter to `from_back` for consistency across memory and string APIs
- Rename memory accessors with 'value' suffix: `get_front`/`get_back` → `get_front_value`/`get_back_value` in view and str/view modules
- Enhanced death.h documentation clarifying the role of `LH_LIBRARY_OPTION_RUNTIME_CHECK_REF` in enabling null-pointer death tests
- Runtime assertions (`lh_assert_runtime_*`) replaced direct `lh_runtime_throw` calls in internal functions for consistency
- Collapsed duplicate `*_multiple_of`/`is_multiple_of` and `*_aligned_is_begin_aligned`/`is_begin_aligned` alias pairs into single `is_multiple_of` / `is_begin_aligned` names across `lh_memory_bounds_t`, `lh_memory_bounds_slice_t`, `lh_memory_view_t`, and `lh_memory_view_slice_t`, for a consistent `is_` prefix on boolean predicates
- Memory bounds/view/typed/vector model went through a full internal rewrite that stores size instead of end pointers, with the element count cached on `lh_memory_typed_t`/`lh_vector_t` instead of re-derived from the byte range on every call; `lh_memory_bounds_allocated_t` is now a thin typedef marker over `lh_memory_bounds_t` rather than a distinct layout
- Broad internal renames for naming-convention consistency across the CPU/SIMD/bit-scan/compiler layers (e.g. `lh_cpu_has_sse2`/`avx2` → `lh_cpu_simd_has_sse2`/`avx2`; `compiler/cpu/supports.h` flattened into `lh/compiler/cpu.h`), and consistent use of `lh_cast_static`/`lh_uchar_t` in place of raw casts/`unsigned char` in the CPU-detection and socket code
- Unicode case-map generator now derives `src/lh/util/str/case/map.c` directly from `UnicodeData.txt` instead of hand-transcribing it, and shares its Unicode-data parsing between the narrow and wide-char generators
- `lh_str_ptr_to_lower`/`_to_upper` sped up via an optional dense 256-entry table (`LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE`, ~10.7x when on); `lh_wstr_ptr_to_lower`/`_to_upper` sped up via an optional two-level BMP block table (`LH_LIBRARY_OPTION_WSTR_CASE_MAP_USE_TABLE`, ~12.8x when on)
- `lh_net_ip4_format`/`lh_net_socket_addr_format` and `lh_vector_get_ptr` optimized to cut redundant work on the hot path
- README updated to document the SIMD/CPU/bit-scan work and the new IO/networking features

### Fixed

- `lh_memory_std_copy` under MSVC was 6-15x slower than GCC due to byte-at-a-time codegen
- `lh_net_ip4_socket_addr_format` (like `lh_net_ip4_format`) was going through the printf engine instead of formatting directly
- Wide string literal macro fixed to avoid an MSVC C5104 warning; character case-mapping initialization order fixed
- `__builtin_cpu_supports` boolean-normalization bug fixed while adding SSSE3 detection (a non-zero-but-not-1 return value was being treated as false)

### Removed

- `lh/memory/bounds/state.h` — state flags consolidated into the view/slice layer
- Standalone `lh/memory/bounds/slice/direction.h` and `lh/memory/bounds/slice/flags.h` — replaced by shared `lh/memory/view/slice/direction.h` and `flags.h`

## [0.2.0] - 2026-04-20

### Added

- GitHub Actions CI workflow for Linux (GCC/Clang) and Windows (MSVC) builds with tests.
- CTest registration for the `lh_test` executable (`enable_testing()` + `add_test(...)`).
- GoogleTest coverage for `lh/memory/std` byte helpers (`copy`, `copy_rev`, `rcopy`,
  `move`, `set`, `compare`, `rcompare`) under mirrored test path `test/src/lh/memory/std.cpp`.
- Portable wide character type (`lh_wchar_t`) and related headers; `lh/uchar.h` for Unicode
  scalar-width aliases.
- Non-owning memory **ranges**, **views**, and **string views** with iteration, slicing,
  find/compare/move/set helpers; aligned pointer grid macros (`lh_ptr_align_up_grid`, …).
- Interval arithmetic macros, pair-field terminology for bounds, and **binary search** helpers
  (`lh_interval_ropen_binary_search`, …) used by generated Unicode tables.
- `lh_wchar_case_pair_t` for code-point mapping pairs; **case folding** for a single wide
  character (`lh_wchar_fold_case`, table in `src/lh/util/wchar/case/fold.c`) and **simple
  case mapping** over wide buffers (`lh_wstr_ptr_to_lower`, `lh_wstr_ptr_to_upper`, table in
  `src/lh/util/wstr/case/map.c`). Regenerate from UCD via `tools/gen_wstr_case_fold.py` and
  `tools/gen_wchar_case_map.py` (defaults write those paths).
- Raw C string helpers: substring, equality, starts/ends with, fill with a repeating pattern.
- Wide-string helpers: content checks, equality, prefix/suffix, fill with wildcard, optional
  case fold for comparisons where applicable.

### Changed

- Moved `ptr.h`, `type.h`, `bit.h`, and `math.h` to `lh/util/` with include guards
  `LH_UTIL_PTR_H`, `LH_UTIL_TYPE_H`, `LH_UTIL_BIT_H`, and `LH_UTIL_MATH_H`.
  Replace `#include <lh/ptr.h>` (and the other three) with `#include <lh/util/…>`.
- Documentation terminology aligned for pointer aliases (`lh_ptr` and `lh_void_ptr`).
- CI now runs tests through `ctest` instead of direct test binary execution.
- Improved English comments in root/test/docs CMake files for clearer build configuration docs.
- Added `LH_ENABLE_RELEASE_MAX_OPT` (default `ON`) to optionally enable aggressive
  `Release`-only optimization flags and best-effort IPO/LTO for the `lh` target.
- IPO/LTO unsupported toolchains now produce a concise status line instead of verbose
  probe output during configure.
- Wide-character **case fold** and **simple case map** sources reorganized under
  `src/lh/util/wchar/` and `src/lh/util/wstr/`; string comparison helpers refactored for
  clearer pointer use; `lh_memory_view` APIs use `lh_ptr` instead of `lh_void` where
  appropriate for readability and consistency.

### Fixed

- `lh_memory_view_init_by_size` now avoids non-portable `void*` arithmetic (MSVC-compatible).
- Runtime terminate path is hardened with a guaranteed non-null default/fallback callback.
- GoogleTest CRT configuration on MSVC now matches project runtime to prevent `/MT` vs `/MD` link errors.

## [0.1.0] - 2026-04-05

### Added

- Portable fixed-width integer types (`lh_uint_t`, `lh_sint_t`, `lh_ushort_t`, `lh_sllong_t`, …)
- Character types with explicit signedness (`lh_uchar_t`, `lh_schar_t`)
- Boolean type (`lh_bool_t`)
- Void aliases (`lh_void`, `lh_void_ptr`)
- Compiler detection: type (Clang / GCC / MSVC), version, target OS, C++ mode
- `extern "C"` guards via `LH_COMPILER(EXTERN_C_BEGIN)` / `LH_COMPILER(EXTERN_C_END)`
- Constructor / destructor and other attribute helpers (`LH_ATTRIBUTE`, …)
- Type-safe cast macros: `lh_cast_static`, `lh_cast_reinterpret`
- Pointer helpers: `lh_ptr_of`, `lh_cptr_of`, and related macros
- String pointer aliases: `lh_str_ptr`, `lh_str_cptr` (`lh/str/ptr.h`)
- Compound-literal initializer macros (`lh/initializer.h`: `lh_initializer`, `lh_initializer_of_type`, …)
- Structured errors: `lh_error_t` with `lh_error_code_t` and `lh_error_desc_t` (`lh/error/desc.h`, alias of `lh_str_cptr` today)
- Exception value type `lh_exception_t`, debug `lh_exception_origin_t`, initializers under `lh/exception/`
- Runtime exception path: catch stack, throw / rethrow macros (`lh/runtime/…`)
- Non-owning memory views/ranges and numeric interval helpers
- `lh_version_t` and `lh_get_version()` (`lh/lh.h`)
- `LH_LIBRARY_OPTION_*` build-time options in generated `lh/config.h` (thread-local, terminate handler, catch-stack depth, …)
- `LH_OPTION_ON` / `LH_OPTION_OFF` symbolic constants for feature toggles
- CMake build with `LH_BUILD_SHARED`, `LH_BUILD_DOCS`, `LH_BUILD_TESTS`, optional Doxygen + Graphviz (`LH_DOCS_GRAPHS`)
- Tracked `include/lh/config.h` for manual builds; CMake emits a generated `lh/config.h` on the build include path so it overrides by order
- Doxygen documentation; GoogleTest-based test target

### Changed

- String-related public headers live under `lh/str/` (replacing the former `lh/string/` path)
- Generic initializer macros moved from `lh/initializer/initializer.h` to `lh/initializer.h`
- Error APIs use `lh_error_desc_t` for description pointers instead of hard-coded `const char *`

### Removed

- `LH_COMPILE_OPTION_*` layer (`compile_option.h`, related CMake modules): optimization, debug symbols, `NDEBUG`, and LTO are expected from the toolchain / CMake flags, not the public lh API

