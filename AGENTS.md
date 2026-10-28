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

- Format with LH's own `.clang-format`; inspect the diff by hand.
  Formatting does not prove conformity with these rules.
- Preserve line endings (the tree is CRLF on Windows checkouts).
- Update `CHANGELOG.md` (`[Unreleased]`) for every public addition/removal.
- Do not commit unless the owner asks. Commits carry no AI attribution.
