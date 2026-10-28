# LH — concept and rules

Read this before changing anything in LH. These rules are not suggestions:
they are the owner's standing requirements, repeated in every session.
When a rule and existing code disagree, the code is the debt — follow the rule
and note the debt in the parent project's `docs/cleanup.md`.

## What LH is

LH is a box of bricks. Each brick is a small, public, separately testable
piece: a type, a function, a macro kernel. Bricks do not know about each
other's purpose; a mechanism (an allocator, a parser, a socket loop) is the
*composition* of bricks, never a monolith that re-implements them.

More public API is better than less: many small bricks, including dangerous
ones (`*_unsafe`, unchecked), are fine — a caller who misuses them owns the
consequence. Hidden implementation is what we avoid.

## 0. LH uses its own bricks — first rule

> The owner: "Why did we create all these wrappers at all, if nobody uses
> them — not even LH itself?"

A brick that exists and is not used where it applies is a defect of the code
that bypasses it, not an optional nicety. LH itself is the first user of
every brick.

- Before writing any line, search LH for the brick that already does it:
  named characters (`lh/char/map.h`), character classes (`lh/char/*`),
  set membership (`lh_str_ptr_contains_char`), sub-views
  (`lh_str_view_from_offset`), trimming, number formatting and parsing
  (`lh/str/format/*`, `lh/str/parse/*`), digit conversion
  (`lh_char_to_digit` / `lh_char_from_digit`), min / max / abs (`lh/math`),
  overflow (`lh_interval_*_is_*_overflow`), memory kernels, casts.
- A hand-written loop, `switch`, range test or literal that duplicates a brick
  is replaced by the brick — in the file being edited and, per section 8,
  everywhere else it occurs.
- If the brick does not fit, fix or extend the brick. Never write a local
  copy "because it was quicker".
- A new brick is used by LH right away: introduce it together with the code
  that switches to it.

## 0.1. From smaller to larger; no extra includes

- Bricks are layered **from the smaller to the larger**. The smallest brick
  includes nothing of LH; the next one includes only it; a third joins the
  two; a fourth builds on the third. A header never includes anything above
  its own layer, and nothing includes back down into a cycle.
- **No extra includes.** A file includes an entity once: when a header it
  already includes brings an entity in, the file does not include that
  entity's header again. Add an include only for what nothing included so
  far provides.
- Exception: a source file keeps its own header first (`x/y.c` includes
  `<lh/x/y.h>`), and an include inside `#if` never counts as provided — it may
  be off in another build configuration.
- Run `python tools/check-includes.py` (from the Authum root) after every
  change; it must report 0. `--fix` removes the redundant lines one at a time.
  Build every backend after a fix: Windows here, Linux through WSL
  (`build-lh-concurrency`), since a POSIX source is not compiled on Windows.

## 1. One file — one entity

- A header declares **one** thing: one struct, one typedef, one enum, or the
  API of one entity. Never bundle several typedefs, structs and enums into
  one file.
- Follow the existing split: `<name>.h` (API), `<name>/fields.h` (struct
  members via the fields macro), `<name>/fn.h` + `<name>/cb.h` (callback
  function type and pointer), `<name>/initializer.h`, `<name>/kind.h` (enum).
  See `lh/memory/allocator/` and `lh/array/`.
- Each piece stands on its own; together they form the mechanism.
- Exception: a **type family** is one entity and stays in one file — the base
  C types (`numeric/types.h`), the fixed widths `u8`…`u64` / `s8`…`s64`
  (`numeric/fixed/types.h`), the same struct per width (`numeric/interval/`,
  `char/interval/`), `char.h`, `memory/word.h`. Do not split them per width.

## 2. Decompose and delegate

- A heavy function is a sign of hidden bricks. Split it; each part becomes
  a public, documented, tested function.
- A composite only delegates. Reference example — `calloc`:
  allocate (`lh_memory_allocator_try_alloc`) + wipe (`lh_memory_std_set`),
  overflow by the existing kernel (`lh_interval_closed_is_mul_overflow`).
  It contains no copy of either implementation.
- No `static` functions. `static` is only for file-local data.

## 3. No parallel implementations, no detours

- Before writing anything, search LH for the kernel that already does it.
  Extend an existing abstraction before introducing a new one.
- If an existing brick is wrong or awkward, **fix that brick**. Never work
  around it with a new wrapper, a second variant or a local re-implementation.
- No wrappers that only rename an existing kernel (removed example:
  `lh/size/ops.h`, which only wrapped the `lh_interval_*_overflow` macros).
- Characters: name them with `lh/char/map.h` (`lh_char_map_ht`, `_space`,
  `_colon`, `_del`, ...), never bare `'	'` / `0x20` literals in code; test a
  byte against a set with `lh_str_ptr_contains_char` (or the
  `lh_str_ptr_fill_char_set` bitmap), with the set as an array of map names.
  Literals stay in tests, where they are the expected data. A token-pasting
  macro (`lh_wstr_ptr_cat`) takes its `_va` form so a map name expands first.
- Memory: always LH kernels (`lh_memory_std_copy/move/set/compare`), never
  libc `mem*` / `str*`. The same applies to OS and FS: portable code uses
  LH kernels; native APIs and native types live only in the OS backend.

## 4. Fallible and checked forms

- An operation that can fail exists as a pair built on the same bricks:
  - `try_<op>` reports failure as its result (`lh_null` / `lh_bool_false`)
    and leaves the decision to the caller;
  - `<op>` is the `try_` call followed by `lh_runtime_check_*`.
- The checked form never duplicates the `try_` logic; it only checks.
  Example: `lh_memory_allocator_alloc` / `_try_alloc`, `_realloc` /
  `_try_realloc` (dispatching to `_try_resize`, `_try_move`, `_try_dealloc`).

## 5. Names say what comes back

- Boolean results: `is_`, `has_`, `can_`, `needs_` — `lh_memory_allocator_realloc_needs_alloc`.
- Value reads: `get_` (or a noun when the meaning is unmistakable); writes: `set_`.
- Actions are verbs: `alloc`, `copy`, `resize`, `move`.
- Fallible: `try_`; unchecked/raw: `_unsafe`.
- A reader must know from the name alone whether a function returns
  something and what.

## 6. Tests per brick

- Every brick has its own GoogleTest in `test/src/lh/` (common runner, pinned
  submodule). A composite is tested through its bricks plus one test of the
  composition. Death tests (`LH_EXPECT_CHECK_DEATH`) for every checked form.
- No standalone runners, manual `CHECK` macros or parallel frameworks.

## 7. Documentation: briefs, not one-liners

- Every header starts with `@file` + `@brief` and a paragraph: what the entity
  is, who owns what, lifecycle, how it composes with neighbours.
- Every public function: what it does, every return value, failure results,
  ownership of arguments and results, preconditions.
  "Write only the allocation callback." is not documentation.
- LH is public open source; write for a person who opens the file for the
  first time.

## 8. Formatting and changes

- Code must read easily. Top-level blocks are separated by one blank line:
  the file comment, the include guard, the `#include` block, every type,
  every declaration and every definition, every test. A struct keeps its own
  `typedef` / `@copybrief` line attached. Never write a file as one dense block.
- When something is pointed out in one file, check the whole tree for the
  same problem; never fix only the file that was named.
- Run `python tools/check-layout.py` (from the Authum root) after any change;
  `--fix` inserts the missing blank lines. It must report 0 places.
- Format with LH's own `.clang-format` (`SeparateDefinitionBlocks: Always`);
  inspect the diff by hand. Formatting does not prove conformity with these rules.
- Preserve line endings (the tree is CRLF on Windows checkouts).
- Update `CHANGELOG.md` (`[Unreleased]`) for every public addition/removal.
- Do not commit unless the owner asks. Commits carry no AI attribution.
