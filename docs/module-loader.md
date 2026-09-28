# Modules and loaders

How `lh_os_module_t` and `lh_os_loader_t` fit together, what happens when a
module is loaded, and — most important — in what order everything is torn
down.

Headers: `lh/os/module.h`, `lh/os/module/ops.h`, `lh/os/loader.h`.
Kernel layer underneath: `lh/os/system/shared.h`. Requires
`LH_LIBRARY_OPTION_OS`.

---

## 1. The model in one paragraph

The **module** is the main object. It has an image (a shared library, or the
program itself), its own methods (`start` / `stop`), its own private state
(`data`), and an **owner** — the loader that loaded it. Loading other modules
is a **privilege**: a module may be granted a **loader**, and then it owns
that loader and every child the loader loads. A module without a loader is a
leaf. The loader is only the mechanism (open, find the methods, start, keep,
unload in reverse); what a module *does* is decided by the module.

This is the same split as a kernel module: the module brings its own
`init` / `exit`, the loading facility only calls them at the right time.

---

## 2. The pieces

### Module — `lh_os_module_t`

| Field     | Meaning                                                                 |
|-----------|-------------------------------------------------------------------------|
| `path`    | Path of the image. Written only by `open` / `bind`.                      |
| `handle`  | OS handle of the image (`HMODULE` / `dlopen` handle), null when closed. |
| `owned`   | Whether `close` must release the handle (see §7).                        |
| `started` | Whether `start` succeeded and `stop` is still due.                       |
| `ops`     | The module's own methods (`lh_os_module_ops_t *`).                       |
| `data`    | The module's private state. The module owns it.                          |
| `owner`   | The loader holding this module. Null for a root.                         |
| `loader`  | The privilege to load children. Null for a leaf.                         |
| `node`    | The link among the owner's children (`lh_list_node_t`). Unlinked for a root. |

Access goes through getters and setters; nothing outside `module.c` touches
the fields. The parent module is **not stored** — it is derived:
`parent = owner ? owner->owner : null` (`lh_os_module_get_parent`).

### Methods — `lh_os_module_ops_t`

```c
typedef struct lh_os_module_ops {
    lh_bool_t (*start)(lh_os_module_t *self); /* false = refuse */
    void      (*stop)(lh_os_module_t *self);
} lh_os_module_ops_t;
```

A child image **exports one object** of this type under a name the loader
was given (the *entry* name, e.g. `"lh_module"`). An image without that
symbol is a plain library, not a module, and is rejected. The root module
(the program) gets its table from the program with `lh_os_module_set_ops`.
Either member may be null — that step is then a no-op.

### Loader — `lh_os_loader_t`

| Field     | Meaning                                                                    |
|-----------|----------------------------------------------------------------------------|
| `modules` | The children in load order: an intrusive list (`lh_list_t`, `lh/list.h`) linked through each child's `node`. |
| `owner`   | The module holding this privilege.                                         |
| `entry`   | The symbol name a child exports its `lh_os_module_ops_t` under (a copy).   |

The loader has **no callbacks of its own**. It does not know what a child
does; it only knows how to bring one up and down.

Every child is allocated separately and linked through its own node, so a
pointer returned by `lh_os_loader_load` or met while walking stays valid
until that child is unloaded — loading or unloading other children never
moves it. Children are walked in load order:

```c
for (lh_os_module_t *m = lh_os_loader_get_first(loader); m; m = lh_os_loader_get_next(loader, m))
```

A loader of your own can keep modules the same way: allocate them with
`lh_os_module_create`, link them through `lh_os_module_get_node` into your
own `lh_list_t`, get a module back from a link with
`lh_os_module_get_by_node`, and after unlinking release it with
`lh_os_module_destroy`.

### "Owner" means the same thing everywhere

```
          owner                       owner
 module ─────────► loader    loader ─────────► module
 "the loader that holds me"  "the module this privilege belongs to"
```

---

## 3. A tree

```
root  (the program, owner = null)
 └─ loader  (entry "lh_module", owner = root)
     ├─ a        (owner = root's loader, leaf)
     └─ holder   (owner = root's loader)
         └─ loader  (entry "lh_module", owner = holder)
             └─ b   (owner = holder's loader, leaf)
```

`get_parent(b) == holder`, `get_parent(holder) == root`,
`get_parent(root) == null`.

---

## 4. Two layers, two state machines

A module has an **image** layer and a **lifecycle** layer. They are separate
on purpose, and they are always torn down in one order: lifecycle first,
image second. The methods live *inside* the image, so they must never run
after it is unmapped.

```
 image:      empty ──open/bind/bind_executable──► loaded ──close──► empty
 lifecycle:  stopped ──start (true)──► started ──stop──► stopped
                     └─start (false)──► stopped   (stop is NOT called)
```

| Call             | Layer     | Does                                                        |
|------------------|-----------|-------------------------------------------------------------|
| `init`           | —         | Empty module. No OS call.                                   |
| `open(path)`     | image     | Load a library from a path. Owns the handle.                |
| `bind(addr)`     | image     | Take the already-loaded image containing `addr`.            |
| `bind_executable`      | image     | Take the program itself.                                    |
| `start`          | lifecycle | Run `ops->start`. Second call is a no-op.                   |
| `stop`           | lifecycle | Unload children, then run `ops->stop`. Safe if not started. |
| `close`          | both      | `stop`, then release the image, then forget `ops`.          |
| `deinit`         | both      | `close`, then free the stored path.                         |
| `create`         | —         | A new `init`'ed module on the heap (runtime allocator).     |
| `destroy`        | both      | `deinit`, then free a module from `create`.                  |

`close` forgets `ops` because the table normally lives in the image that was
just released; keeping the pointer would leave it dangling.

---

## 5. Loading

### 5.1 The root

The program is the root. It binds itself, gives itself its methods, starts,
and takes the loader privilege:

```c
lh_os_module_t root;

lh_os_module_init(&root);
lh_os_module_bind_executable(&root);
lh_os_module_set_ops(&root, &root_ops);      /* optional */
lh_os_module_set_data(&root, &app_state);    /* optional */
lh_os_module_start(&root);

lh_os_loader_t *loader = lh_os_module_grant_loader(&root, "lh_module");
for (each path the program decided to load)      /* its policy, see §5.3 */
    lh_os_loader_load(loader, &path);
```

`grant_loader` allocates the loader and stores it in the module. A module can
be granted **one** loader; a second call fails.

### 5.2 One child — `lh_os_loader_load(loader, path)`

```
 1. allocate a lh_os_module_t, init it          ── fail: out of memory, return null
 2. child->owner = loader                       (so start can already see its parent)
 3. open(path)                                  ── fail: OS error, go to 7
 4. ops = get_sym(child, loader->entry)         ── missing: not a module, go to 7
 5. set_ops(child, ops)
 6. start(child)                                ── refused: go to 7
    ├─ success: append child to loader->modules, return child
 7. deinit(child) and free it, return null      (nothing is kept)
```

Notes:

- In step 6 the child may itself call `grant_loader` and load its own
  children (that is how `holder` above gets `b`). The whole subtree is loaded
  *inside* the parent's `start`.
- If `start` refuses, `stop` is **not** called. Whatever `start` acquired
  before refusing, it releases itself — except a loader it was granted: that
  one is revoked automatically (its children unloaded, the loader freed).
- A failed child leaves no trace in `loader->modules`.

### 5.3 Which paths — the caller's policy

The loader takes **one path at a time**. Where the paths come from — a list
in a config file, a directory scan, a fixed set compiled in — is not the
loader's business: every program does it differently, and lh does not guess.

lh gives the pieces to build such a policy portably:

| Need                                   | Use                                              |
|----------------------------------------|--------------------------------------------------|
| Where the program itself lives          | `lh_os_module_get_path_as_const(&root)`          |
| The directory of a file                 | `lh_fs_path_parent`                              |
| Build a path                            | `lh_fs_path_join`                                |
| List a directory                        | `lh_os_fs_dir_t` (`open` / `read` / `close`)      |
| The shared-library suffix of this OS    | `lh_os_system_shared_get_ext` (`.dll` / `.so`)    |
| "The directory does not exist"          | `lh_os_system_error_code_is_not_found` / `_is_not_dir` on the native error after a failed open |
| Skip a module's own file                | `lh_fs_path_equals`                              |

---

## 6. Unloading

Unloading is driven from the top: `lh_os_module_deinit(&root)` (or `close`, or
`stop`) tears down the whole tree beneath that module.

### 6.1 One module — `lh_os_module_stop(m)`

```
 1. if m holds a loader:
      lh_os_loader_deinit(m->loader)       (unload all children — §6.2)
      free the loader, m->loader = null
 2. if m is started:
      ops->stop(m)
      m->started = false
```

**Children go before their parent**: a child may still be using something
its parent provides, so the parent's `stop` runs only after every child is
gone.

### 6.2 A loader — `lh_os_loader_unload(loader)`

```
 while there are children:
     pop the LAST one off the list
     lh_os_module_deinit(child)   → close → stop → (its own children first) → release image
     free it
```

Children are unloaded in **reverse load order**: the last loaded goes first.
A later module may depend on an earlier one, never the other way round.

One child can also go on its own, in O(1), leaving its siblings in place:
`lh_os_loader_unload_child(loader, child)` unlinks it and does the same
`deinit` + free. Whether the others can live without it is the caller's call.

### 6.3 One module, full — `lh_os_module_close(m)`

```
 1. stop(m)                         (§6.1: children, then ops->stop)
 2. release the image if owned      (FreeLibrary / dlclose)
 3. handle = null, ops = null
```

### 6.4 The whole sequence for the tree in §3

Loaded in the order `a`, `holder` (which loads `b` inside its `start`):

```
start:a;  start:holder;  start:b;
```

`lh_os_module_deinit(&root)`:

```
root.stop
 └─ root.loader deinit, last child first:
     holder.deinit → holder.close → holder.stop
      └─ holder.loader deinit:
          b.deinit → b.close → b.stop → ops->stop(b)   ─► "stop:b"
                              → release b's image
      └─ ops->stop(holder)                               ─► "stop:holder"
      → release holder's image
     a.deinit → a.close → a.stop → ops->stop(a)          ─► "stop:a"
      → release a's image
 └─ ops->stop(root)
 → root's handle dropped (not released on Windows, see §7)
```

Result: `stop:b; stop:holder; stop:a;` then the root.

### 6.5 Guarantees

- `stop` runs exactly once for every module whose `start` succeeded, and
  never for one whose `start` refused.
- A module's `stop` runs after all its children are unloaded and before its
  own image is released.
- Siblings are unloaded in reverse load order.
- No module's code runs after its image is released (`close` stops first and
  forgets `ops`).
- `stop` and `close` are safe to call again on an already stopped or closed
  module. `deinit` is called once, like any other `deinit`.

---

## 7. Handle ownership (`owned`)

| How the image was taken | Windows                       | POSIX                        |
|-------------------------|-------------------------------|------------------------------|
| `open(path)`            | owned (`FreeLibrary`)          | owned (`dlclose`)             |
| `bind(addr)`            | not owned (no refcount taken) | owned (`dlopen RTLD_NOLOAD`) |
| `bind_executable`             | not owned                     | owned (`dlopen(NULL)`)        |

`close` releases the handle only when it is owned; otherwise it simply
forgets it.

---

## 8. Errors

Two last-error slots, never mixed (see `lh/os/system/error/code.h`):

- `lh_os_last_error` — our own checks: empty path, already open, out of
  memory, loader already granted, not loaded.
- `lh_os_system_last_error` — the native call failed (`GetLastError` /
  `errno`), e.g. the library did not open or the entry symbol is missing.

Native codes stay native. To ask what a native code *means* the same way on
every OS, use `lh/os/system/error/kind.h`.

### Text is UTF-8

Every path lh takes and every path, name or message it hands back is UTF-8,
on every OS. On Windows the backend talks to the kernel only through the
`...W` functions and converts UTF-8 ⇄ UTF-16 at that boundary; the `...A`
functions are never used, because they read the ANSI code page (cp1251,
cp1252, …) rather than UTF-8. So `Папка-日本/модуль.dll` loads the same on a
Russian, an English or a Japanese Windows, and `lh_os_module_get_path_as_const`
returns it byte for byte. With `LH_LIBRARY_OPTION_OS_WERROR` on, native error
messages are UTF-16 instead (that option's purpose).

The kernel's own text has a type, `lh_os_str_t` (`lh/os/str.h`): UTF-16
(`lh_wstr_t`) on Windows, UTF-8 (`lh_str_t`) on POSIX. The one place text
crosses between the two is `lh_os_system_str_from_utf8` /
`lh_os_system_str_to_utf8` (`lh/os/system/str.h`) — a conversion on Windows,
a copy on POSIX.

---

## 9. Writing a module

```c
#include <lh/os/loader.h>
#include <lh/os/module.h>

static lh_bool_t start(lh_os_module_t *self)
{
    lh_os_module_t *parent = lh_os_module_get_parent(self);   /* already set */

    lh_os_module_set_data(self, my_state_new(parent));

    /* Optional: this module loads its own children. Which ones is its
       own policy (§5.3); the loader only takes paths. */
    lh_os_loader_t *loader = lh_os_module_grant_loader(self, "lh_module");
    if (loader) {
        for (each path this module decided to load)
            lh_os_loader_load(loader, &path);
    }
    return lh_bool_true;                 /* false = refuse to be loaded */
}

static void stop(lh_os_module_t *self)
{
    /* Children are already gone here. */
    my_state_free(lh_os_module_get_data(self));
}

EXPORT const lh_os_module_ops_t lh_module = {start, stop};
```

Rules of thumb:

- Do not free `self` or close its image from `start` / `stop` — the owner
  does that.
- Do not keep pointers to children past their unload.
- If `start` refuses, release what it acquired (a granted loader is revoked
  for you).

---

## 10. Caveat: one allocator for the whole tree

If every module links its own copy of lh statically, memory can be allocated
by one image and freed by another — for example, a child's `grant_loader`
allocates the loader, and the parent's code frees it during unload. This is
correct only while all images share one allocator (one C runtime: the same
UCRT/MSVCRT on Windows, glibc on Linux). A module built against a different
C runtime would break it.
