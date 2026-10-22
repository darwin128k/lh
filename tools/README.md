# Tools

Scripts for code generation and analysis. These tools are used during development to generate source code from external data (Unicode tables) and analyze header dependencies.

> **Attention:** Requires Python 3.6 or higher.

## Include Graph

Generates Graphviz visualization of header dependencies.

```sh
python3 gen_include_graph.py output
```
Generates `output.dot` and `output.svg`.

```sh
python3 gen_include_graph.py graph.svg
```
Generates only `graph.svg`.

```sh
python3 gen_include_graph.py graph.dot
```
Generates only `graph.dot`.

```sh
python3 gen_include_graph.py graph -f dot
```
Generates only `graph.dot`.

```sh
python3 gen_include_graph.py graph -f svg
```
Generates only `graph.svg`.

```sh
python3 gen_include_graph.py graph -f dot svg
```
Generates both `graph.dot` and `graph.svg`.

## Unicode Code Generation

Scripts that generate Unicode character tables from external data.

### Case Map (wide)

Generates wide-char case mapping tables (`lh_wstr_ptr_to_lower` / `lh_wstr_ptr_to_upper`) from UnicodeData.txt.

```sh
python3 gen_wchar_case_map.py
```
Writes to `src/lh/util/wstr/case/map.c`.

### Case Map (narrow)

Generates narrow `lh_char_t` case mapping tables (`lh_str_ptr_to_lower` / `lh_str_ptr_to_upper`)
from the same UnicodeData.txt, restricted to code points where both the source and mapped
scalar fit in a single byte (0..255) — a handful of Latin-1 mappings whose target escapes that
range (e.g. MICRO SIGN, LATIN SMALL LETTER Y WITH DIAERESIS) are dropped, since they can't be
represented. Emits both implementations selected by `LH_LIBRARY_OPTION_STR_CASE_MAP_USE_TABLE`
(see that option in `cmake/library_options.cmake`): a dense 256-entry lookup table, and the
sorted sparse pair table with binary search.

```sh
python3 gen_str_case_map.py
```
Writes to `src/lh/util/str/case/map.c`.

### Case Fold

Generates case folding table (`lh_wchar_fold_case`) from CaseFolding.txt.

```sh
python3 gen_wstr_case_fold.py
```
Writes to `src/lh/util/wchar/case/fold.c`.

### Data

Unicode data files in `data/unicode/`:
- `CaseFolding-16.0.0.txt`
- `UnicodeData-16.0.0.txt`

Download from [unicode.org](https://www.unicode.org/Public/UCD/latest/) (UCD Database). Refresh when bumping UCD version.

## Number Conversion Tables

Computes the two tables `lh/real` converts numbers with, in exact Python integers
(nothing to download), and writes their initializers into the `.c` files:

- `m_real_power5` — 128-bit powers of five for Eisel–Lemire (`lh_str_ptr_parse_real`),
  the same table as fast_float's;
- `m_real_cached` — cached powers of ten for Grisu2 (`lh_str_ptr_format_real`),
  the same table as RapidJSON's.

```sh
python3 gen_real_tables.py
```
Writes to `src/lh/real/power5.c` and `src/lh/real/cached.c`.
