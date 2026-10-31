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

More public API is better than less: many small bricks, including the
unchecked fast forms (no suffix) and the validating ones (`_v`), are fine — a
caller who misuses them owns the consequence. Hidden implementation is what we
avoid.

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
- **A `for` loop counts with an assignment in its third clause**:
  `for (i = 0U; lh_math_lt(i, n); i = lh_math_add_one(i))`. The bare form
  `lh_math_add_one(i)` — the value discarded — is dropped by GCC 16 (MSYS2
  MinGW): the loop then runs forever or stops on another condition, with no
  warning. The LH sources always assign; keep it that way.
- If the brick does not fit, fix or extend the brick. Never write a local
  copy "because it was quicker".
- A new brick is used by LH right away: introduce it together with the code
  that switches to it.
- **Everything goes down to its root.** When a function grows its own loop
  because the root brick "is too slow" or "does not handle this case" (for
  example `lh_memory_std_*` on small copies), the question is not to that
  function but to the root: why does it not cope? Fix the root — a small-size
  path in the kernel, a missing parameter — and delete the local copy.
  Measure before and after (`bench/`), and write the reason and the numbers
  into the root's documentation and `CHANGELOG.md`. An unexplained local
  re-implementation is not allowed; nobody can later tell why it exists.

## 0.1.1. What a kernel is; the OS is the whole, not the base

- A **kernel** is a set of small universal implementations that everything
  else uses; the more of them, the better. `lh_memory_std_*` is the main one.
- The **OS is the most general whole**, composed of parts: the filesystem,
  the network, threads, windows, input. A part does not depend on "the OS";
  the OS is assembled from the parts. So the filesystem is not built on the
  OS — the OS includes the filesystem. (Today the portable filesystem sits
  under `lh/os/fs/`; moving the parts out from under `lh/os/` is tracked in
  `docs/cleanup.md`.)

## 0.2. The reference layering: native backend below, LH above

The OS layer is the model for every subsystem that touches the platform:

- `src/lh/os/system/<backend>/` (`win`, `posix`, ...) holds **only** the
  platform's own calls and types, as they are — no LH portable wrappers, no
  LH policy. It may use LH types and the small bricks it needs (bit ops),
  nothing above them.
- The portable LH API (`lh/os/*`, `lh/fs/*`, ...) is built **on top** of that
  backend and never calls the platform directly.
- A new platform feature first goes into each backend in the platform's own
  terms, then gets one portable wrapper above them.

## 0.0. No raw fields — everywhere, LH and Authum

A struct's fields are touched **only** by that struct's own level-1 getters
and setters. Every other function — in its own `.c` file too, in every other
file of LH and of Authum — reads and writes through them. No exceptions.

- Level 1: `get_<field>` / `set_<field>` read or write one field and call
  nothing. They are the only code that names a field.
- Level 2 uses only level 1: a computed value is composed, never stored or
  re-derived by hand — `get_end` is `get_begin + get_size`; `set` is
  `set_begin` + `set_size`.
- Level 3 and up use the levels below: predicates, compare, find, copy, the
  `_v` (validated) forms. A function never skips a level to reach a field.
- Headers and `.c` files are ordered by level: level 1 first, then 2, ...
  `python tools/check-levels.py <file.c> <prefix>` prints the levels (and any
  cycle), `--order` checks the file; `python tools/order-levels.py <file.c>
  <file.h> <prefix>` sorts both, keeping the order inside a level. Done for
  `lh_memory_view_*`, `lh_memory_bounds_*`, `lh_memory_typed_*`.
- The fast forms have no suffix and no checks; `_v` adds validation where it
  is really needed — not a `_v` for every function, and no forms for private
  contracts (`_nonempty`, `_by_size` setters). The developer owns the
  consequences of a fast form; its documentation says what it does not check.

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

- `extern "C"` (`LH_COMPILER_EXTERN_C_BEGIN` / `_END`) wraps only what has
  linkage: function declarations and function-type typedefs (a C++ function
  type carries its language linkage). Structs, enums and plain typedefs have
  no linkage and are declared **before** the block. `extern` objects have
  linkage too and stay inside. Check with `python tools/check-extern-c.py`
  (0 required; `--fix` rebuilds the blocks, keeping declaration order).

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
- A brick may be a thin layer that **calls** another brick, under any name that
  says what it answers (`lh_memory_is_equal` calls `lh_memory_std_compare`).
  What is forbidden is **copying an implementation**: an `is_equal` that walks
  the bytes itself instead of calling `compare` duplicates the kernel. Also
  forbidden: a second brick with the same contract and a new name (removed
  example: `lh/size/ops.h`, whose `lh_size_add` was
  `lh_interval_closed_is_add_overflow` again).
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
- Fallible: `try_`; validating: `_v`; unchecked/raw: no suffix — that is the
  fast form, and it does not check.
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
