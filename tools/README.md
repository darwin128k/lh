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

### Case Map

Generates wide-char case mapping tables (`lh_wstr_ptr_to_lower` / `lh_wstr_ptr_to_upper`) from UnicodeData.txt.

```sh
python3 gen_wchar_case_map.py
```
Writes to `src/lh/util/wstr/case/map.c`.

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