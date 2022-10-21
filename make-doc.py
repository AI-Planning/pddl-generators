#! /usr/bin/env python3

"""Generate a Latex file containing all domain READMEs."""

from pathlib import Path
import sys


DIR = Path(__file__).resolve().parent

HEADER = """\
\\documentclass{article}

\\usepackage[margin=1in]{geometry}
\\usepackage[hidelinks]{hyperref}
\\usepackage[utf8]{inputenc}

\\title{PDDL Generators}

\\author{Jendrik Seipp, \\'Alvaro Torralba, J\\"org Hoffmann}
\\date{}
\\hypersetup{pdfinfo={
Title={PDDL Generators},
Author={Jendrik Seipp, Alvaro Torralba, Joerg Hoffmann}
}}
\\begin{document}
\\maketitle

A collection of PDDL generators, some of which have been used
to generate benchmarks for the International Planning Competition (IPC).

\\tableofcontents
"""

FOOTER = """\
\\end{document}
"""


def get_text(domain_dir: Path):
    files = set(x.name for x in domain_dir.iterdir())
    content = None
    for readme_name in ["README", "README.txt"]:
        if readme_name in files:
            with (domain_dir / readme_name).open() as f:
                content = f.read().strip()
            break
    if content is None:
        sys.exit(f"Error: {domain_dir.name} has no README.txt file.")
    return f"\\section{{{domain_dir.name.capitalize()}}}\n\n\\begin{{verbatim}}\n{content}\\end{{verbatim}}"


def main():
    parts = []
    for domain_dir in sorted(DIR.iterdir()):
        if domain_dir.is_dir() and not domain_dir.name.startswith("."):
            parts.append(get_text(domain_dir))
    print(HEADER + "\n\n".join(parts) + FOOTER)


if __name__ == "__main__":
    main()
