#!/usr/bin/env python
"""Audit the public headers for declarations that carry no Doxygen comment.

Doxygen's WARN_IF_UNDOCUMENTED has two blind spots, both silent:

  1. A header that is reached first as a nested #include is skipped when it
     later appears as a top-level input file. Its include guard is already
     defined, so Doxygen never looks inside it: no file page, no members and
     no warning. (include/lh/memory/typed/allocated.h pulls in bounds.h and
     typed.h that way.)
  2. A function that is declared in a header and defined in a .c file is
     attributed to the .c file, so a missing doc comment in the header is
     never reported against the header.

This script reads the headers themselves and reports every declaration that
is not preceded by a Doxygen comment block.
"""
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
INC = os.path.join(ROOT, "include")

DEFINE = re.compile(r"^#\s*define\s+([A-Za-z_]\w*)")
TYPEDEF = re.compile(r"^\}\s*\*?\s*([A-Za-z_]\w*)\s*;")
STRUCT_OPEN = re.compile(r"^(struct|union|enum)\s+([A-Za-z_]\w*)")
FUNC_PTR = re.compile(r"\(\s*\*")


def doc_block_ends(raw):
    """Mark each line that closes a /** ... */ block."""
    ends = [False] * len(raw)
    in_doc = False
    for i, line in enumerate(raw):
        s = line.strip()
        if not in_doc:
            if s.startswith("/**"):
                if "*/" in s:
                    ends[i] = True
                else:
                    in_doc = True
        elif "*/" in s:
            ends[i] = True
            in_doc = False
    return ends


def strip_comments(raw):
    """Blank out comment bodies so declarations can be spotted reliably."""
    out = []
    in_block = False
    for line in raw:
        s = line
        if in_block:
            j = s.find("*/")
            if j < 0:
                out.append("")
                continue
            s = " " * j + s[j + 2:]
            in_block = False
        while True:
            j = s.find("/*")
            k = s.find("//")
            if k >= 0 and (j < 0 or k < j):
                s = s[:k]
                break
            if j < 0:
                break
            e = s.find("*/", j + 2)
            if e < 0:
                s = s[:j]
                in_block = True
                break
            s = s[:j] + " " * (e - j) + s[e + 2:]
        out.append(s)
    return out


def macro_body_lines(raw):
    """Mark every line that belongs to the body of a multi-line macro.

    A macro body is code, not declarations: its statements must not be
    mistaken for prototypes.
    """
    inside = [False] * len(raw)
    continuing = False
    for i, line in enumerate(raw):
        if continuing:
            inside[i] = True
        continuing = line.rstrip().endswith("\\")
    return inside


def tagged_names(raw):
    """Names a doc block names explicitly with @typedef / @struct / @def ...

    A block may document several entities at once:

        /**
         * @struct lh_exception
         * @typedef lh_exception_t
         * @brief ...
         */
        struct lh_exception { ... };
        typedef struct lh_exception lh_exception_t;
    """
    names = set()
    tag = re.compile(r"@(typedef|struct|union|enum|def|var|fn)\s+([A-Za-z_]\w*)")
    i = 0
    n = len(raw)
    while i < n:
        if not raw[i].strip().startswith("/**"):
            i += 1
            continue
        block = []
        j = i
        while j < n:
            block.append(raw[j])
            if "*/" in raw[j]:
                break
            j += 1
        for m in tag.finditer("\n".join(block)):
            names.add(m.group(2))
        i = j + 1
    return names


def group_ranges(raw):
    """Index ranges covered by a `@name ... @{ ... @}` documentation group.

    Inside such a group the members are documented by the group, not one by
    one (Doxygen's DISTRIBUTE_GROUP_DOC shares the group's text).
    """
    ranges = []
    i = 0
    n = len(raw)
    open_at = None
    while i < n:
        s = raw[i].strip()
        if not s.startswith("/**"):
            i += 1
            continue
        # Collect the whole block.
        block = []
        j = i
        while j < n:
            block.append(raw[j])
            if "*/" in raw[j]:
                break
            j += 1
        text = "\n".join(block)
        if "@name" in text and open_at is None:
            open_at = j + 1
        elif "@}" in text and open_at is not None:
            ranges.append((open_at, j))
            open_at = None
        i = j + 1
    if open_at is not None:
        ranges.append((open_at, n - 1))
    return ranges


def audit(path):
    with open(path, "r", encoding="utf-8") as fh:
        raw = fh.read().split("\n")
    ends = doc_block_ends(raw)
    code = strip_comments(raw)
    groups = group_ranges(raw)
    macro_body = macro_body_lines(raw)
    tagged = tagged_names(raw)

    def documented_before(idx):
        k = idx - 1
        in_chain = False
        while k >= 0:
            if ends[k]:
                return True
            if code[k].strip() == "":
                k -= 1
                continue
            if macro_body[k]:
                # The body of the previous branch's macro.
                k -= 1
                continue
            s = code[k].strip()
            # A linkage wrapper may sit between the doc block and the
            # declaration it documents.
            if s.startswith("LH_COMPILER_EXTERN_C"):
                k -= 1
                continue
            # A preprocessor guard may sit between the doc block and the
            # definition it documents:
            #   /** @def X ... */  #ifndef X  #define X 1  #endif
            # and a conditional chain may carry one doc block for all of
            # its branches:
            #   /** @def X ... */  #if ... #define X 1 #elif ... #define X 2
            if s.startswith("#"):
                if re.match(r"#\s*(if|ifdef|ifndef|elif|else)\b", s):
                    in_chain = True
                    k -= 1
                    continue
                if re.match(r"#\s*define\b", s) and in_chain:
                    k -= 1
                    continue
                if k > 0 and ends[k - 1]:
                    return True
                return False
            return False
        return False

    def in_group(idx):
        for start, end in groups:
            if start <= idx <= end:
                return True
        return False

    def is_documented(idx, name=None):
        """Is the declaration at idx covered by a Doxygen comment?"""
        if in_group(idx):
            return True
        if name and name in tagged:
            return True
        return documented_before(doc_index(idx))

    def doc_index(idx):
        """The line the doc comment must precede.

        A prototype may put its return type, and an attribute macro, on
        lines of their own:

            /**
             * @brief ...
             */
            LH_ATTRIBUTE_FORCE_INLINE
            lh_usize_t
            lh_bit_scan_forward_u32(lh_u32_t x)

        The doc block sits above the attribute, not above the name.
        """
        k = idx - 1
        while k >= 0:
            if ends[k]:
                return k + 1
            if code[k].strip() == "":
                k -= 1
                continue
            prev = code[k].strip()
            # Linkage wrappers are not part of the declaration.
            if prev.startswith("LH_COMPILER_") or prev.startswith("extern"):
                return idx
            # A return type or an attribute line: keep walking up.
            if re.fullmatch(r"[A-Za-z_][A-Za-z0-9_ \t\*]*", prev) \
                    and not prev.startswith("#"):
                k -= 1
                continue
            return idx
        return idx

    missing = []
    depth = 0            # brace depth
    in_struct = False    # inside a struct/union/enum body
    in_macro = False     # inside a multi-line macro body
    i = 0
    n = len(code)
    while i < n:
        s = code[i].strip()

        if not s:
            i += 1
            continue

        # A macro body is code, not declarations.
        if macro_body[i]:
            i += 1
            continue

        # Track struct bodies so their members are not mistaken for
        # declarations.
        if in_struct:
            if s.startswith("}"):
                in_struct = False
                depth = max(0, depth - 1)
                # A trailing typedef: `} lh_foo_t;`
                m = TYPEDEF.match(s)
                if m and m.group(1).startswith("lh_") and not is_documented(i, m.group(1)):
                    missing.append((i + 1, m.group(1)))
            i += 1
            continue

        if s.startswith("struct ") or s.startswith("union ") or s.startswith("enum "):
            m = STRUCT_OPEN.match(s)
            # A forward declaration (`struct lh_x;`) has no body to document:
            # the definition carries the documentation.
            if m and not s.endswith(";") and not is_documented(i, m.group(2)):
                missing.append((i + 1, m.group(2)))
            if not s.endswith(";"):
                in_struct = True
                depth += 1
            i += 1
            continue

        # A typedef, including a function-pointer typedef. The name is the
        # one in parentheses, or the one after the struct/union/enum.
        if s.startswith("typedef "):
            m = re.search(r"\(\s*([A-Za-z_]\w*)\s*\)\s*\(", s)
            if not m:
                m = re.search(r"typedef\s+.*?\b([A-Za-z_]\w*)\s*;", s)
            if m and not is_documented(i, m.group(1)):
                missing.append((i + 1, m.group(1)))
            i += 1
            continue

        # A macro definition.
        m = DEFINE.match(s)
        if m:
            name = m.group(1)
            # A compile-time check, not an API: the project hides it from the
            # reference the same way.
            if name.startswith("lh_assert_static"):
                i += 1
                continue
            # The include guard is not an API: it is the `#define` that
            # follows the matching `#ifndef`.
            k = i - 1
            while k >= 0 and code[k].strip() == "":
                k -= 1
            guard = k >= 0 and re.match(r"^#\s*ifndef\s+" + re.escape(name) + r"\s*$",
                                        raw[k].strip())
            if not guard and not is_documented(i, name):
                missing.append((i + 1, name))
            i += 1
            continue

        # A function: a prototype (`;`) or an inline definition (`{`).
        if "(" in s and not s.startswith("#"):
            chunk = [s]
            j = i
            while j + 1 < n and ";" not in chunk[-1] and not chunk[-1].endswith("{") \
                    and (j - i) < 12:
                j += 1
                chunk.append(code[j].strip())
            text = " ".join(chunk)
            head = text.split("(")[0]
            is_def = text.rstrip().endswith("{")
            # An initializer is a variable, not a prototype. The `=` may
            # sit on the previous line:
            #   static const T NAME =
            #       factory(...);
            prev = ""
            k = i - 1
            while k >= 0 and code[k].strip() == "":
                k -= 1
            if k >= 0:
                prev = code[k].strip()
            if "=" in head or prev.endswith("="):
                is_def = False
                looks_like_func = False
            elif re.match(r"^(lh_assert_static|LH_COMPILER_EXTERN_C\w*)\s*\(", text):
                # A compile-time check or a linkage wrapper, not an API.
                looks_like_func = False
            else:
                looks_like_func = (
                    (";" in text or is_def)
                    and "(" in text
                    and "=" not in head
                    and not FUNC_PTR.search(text)
                    and not head.strip().endswith(",")
                    and not re.search(r"\b(if|for|while|switch|return|else)\s*$", head)
                )
            if looks_like_func:
                if not is_documented(i, None):
                    m2 = re.search(r"([A-Za-z_]\w*)\s*\(", text)
                    missing.append((i + 1, m2.group(1) if m2 else text[:60]))
                if is_def:
                    # Skip the body: it is code, not declarations.
                    level = 0
                    k = j
                    while k < n:
                        level += code[k].count("{") - code[k].count("}")
                        if level <= 0 and k > j:
                            break
                        k += 1
                    i = k + 1
                else:
                    i = j + 1
                continue

        i += 1
    return missing


def main():
    only = sys.argv[1] if len(sys.argv) > 1 else None
    rows = []
    total = 0
    for dirpath, _dirs, files in os.walk(INC):
        for name in sorted(files):
            if not name.endswith(".h"):
                continue
            path = os.path.join(dirpath, name)
            rel = os.path.relpath(path, ROOT).replace("\\", "/")
            if only and only not in rel:
                continue
            miss = audit(path)
            if miss:
                rows.append((rel, len(miss), miss))
                total += len(miss)
    rows.sort(key=lambda r: -r[1])
    for rel, count, miss in rows:
        names = ", ".join(m[1] for m in miss[:4])
        print(f"{count:4d}  {rel}\n        {names}" + (f" (+{count - 4})" if count > 4 else ""))
    print(f"\nheaders with undocumented declarations: {len(rows)}")
    print(f"undocumented declarations total: {total}")


if __name__ == "__main__":
    main()
