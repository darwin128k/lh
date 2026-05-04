#!/usr/bin/env python3
import os
import re
import argparse
import subprocess

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(SCRIPT_DIR)
INCLUDE_DIR = os.path.join(PROJECT_ROOT, "include")

def get_all_headers():
    """Find all .h files in include/lh/"""
    headers = []
    for root, dirs, files in os.walk(INCLUDE_DIR):
        for f in files:
            if f.endswith('.h'):
                path = os.path.join(root, f)
                rel = os.path.relpath(path, INCLUDE_DIR)
                headers.append(rel)
    return headers

def get_includes(filepath):
    """Extract #include statements from a file."""
    includes = []
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                match = re.match(r'#include\s+[<"](.+?)[>"]', line.strip())
                if match:
                    inc = match.group(1)
                    if inc.startswith('lh/'):
                        includes.append(inc)
    except:
        pass
    return includes

def build_full_graph():
    """Build graph for all headers."""
    headers = get_all_headers()
    header_set = set(headers)

    edges = []
    for h in headers:
        full_path = os.path.join(INCLUDE_DIR, h)
        incs = get_includes(full_path)
        for inc in incs:
            if inc in header_set:
                edges.append((h, inc))

    return edges, header_set

def generate_dot(edges, nodes, output_file):
    """Generate graphviz dot file."""
    dirs = {}
    for n in nodes:
        d = n.split('/')[0] if '/' in n else ''
        if d not in dirs:
            dirs[d] = []
        dirs[d].append(n)

    with open(output_file, 'w') as f:
        f.write("digraph includes {\n")
        f.write("    rankdir=TB;\n")
        f.write("    nodesep=0.6;\n")
        f.write("    ranksep=1.0;\n")
        f.write("    compound=true;\n")
        f.write("    outputorder=edgesfirst;\n")
        f.write("    newrank=true;\n")
        f.write("    node [shape=box, fontname=\"monospace\", fontsize=10, fontweight=bold, style=filled, fillcolor=white];\n")
        f.write("    edge [fontsize=2, color=\"#1976d2\", splines=ortho];\n")
        f.write("    graph [autosize=true];\n")

        for d, files in sorted(dirs.items()):
            if d:
                f.write(f'    subgraph cluster_{d} {{\n')
                f.write(f'        label="{d}";\n')
                f.write(f'        color="#1976d2"; penwidth=2;\n')
                for n in sorted(files):
                    short = '/'.join(n.split('/')[1:]) if '/' in n else n
                    short = short.replace('.h', '')
                    f.write(f'        "{n}" [label="{short}", color="#1976d2", penwidth=1];\n')
                f.write('    }\n')

        for src, dst in edges:
            f.write(f'    "{src}" -> "{dst}";\n')

        f.write("}\n")

def main():
    parser = argparse.ArgumentParser(description="Generate include graph for lh library.")
    parser.add_argument(
        "output",
        nargs="?",
        default=None,
        help="Output file path (extension determines format: .dot or .svg)"
    )
    parser.add_argument(
        "-f", "--format",
        nargs="*",
        default=[],
        choices=["dot", "svg"],
        help="Output formats: dot, svg, or both (default: svg)"
    )
    args = parser.parse_args()

    edges, nodes = build_full_graph()
    print(f"Total headers: {len(nodes)}")
    print(f"Total edges: {len(edges)}")

    explicit_formats = set(args.format) if args.format else set()

    if args.output:
        output = args.output
        base, ext = os.path.splitext(output)
        if ext in [".dot", ".svg"] and not explicit_formats:
            output_formats = {ext[1:]}
        else:
            output_formats = explicit_formats if explicit_formats else {"svg"}
        base = output.rsplit(".", 1)[0]
    else:
        base = "/tmp/full_includes"
        output_formats = explicit_formats if explicit_formats else {"svg"}

    keep_dot = "dot" in explicit_formats or ("dot" in output_formats and not explicit_formats)

    dot_path = base + ".dot"
    generate_dot(edges, nodes, dot_path)
    print(f"Generated {dot_path}")

    if "svg" in output_formats:
        svg_path = base + ".svg"
        try:
            subprocess.run(["dot", "-Tsvg", dot_path, "-o", svg_path],
                          check=True, capture_output=True)
            print(f"Generated {svg_path}")
        except FileNotFoundError:
            print("Warning: dot (graphviz) not found, skipping SVG generation")
        except subprocess.CalledProcessError as e:
            print(f"Warning: dot failed to generate SVG: {e.stderr.decode()}")

    if not keep_dot and os.path.exists(dot_path):
        os.remove(dot_path)
        print(f"Removed {dot_path}")

    final_ext = "svg" if "svg" in output_formats else "dot"
    print(f"Done: {base}.{final_ext}")

if __name__ == "__main__":
    main()