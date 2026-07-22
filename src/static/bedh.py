#!/usr/bin/env python3

# Copyright © 2026 Simon Danielsson
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files, to deal in the Software
# without restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies of the
# Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

"""
Requirements: Python 3.10+

bedh.py

A small utility that converts an UTF-8 input-file into a valid
C header (.h) file containing the input-file's contents as an unsigned
char array and a corresponding length variable.

```
Usage:
$ ./bedh.py input.txt
```

Author: Simon Danielsson

https://github.com/simon-danielsson/bedh.py
https://www.simondanielsson.se/

"""

import sys
from pathlib import Path

ROW_WIDTH = 11

def read_file(path: Path) -> bytes:
    return path.read_bytes()

def bytes_to_hex(data: bytes):
    for b in data:
        yield f"0x{b:02x}"

def format_filename_to_varname(filename: str, len: bool) -> str:
    output = []

    for c in filename:
        if c.isalnum():
            output.append(c)
        elif c in " -.":
            output.append("_")

    name = "".join(output).strip("_")

    if name and name[0].isdigit():
        name = "_" + name

    if len:
        name += "_len"

    return name

def process_hex(src_path: Path, dest_path: Path):
    content = read_file(src_path)

    h: list[str] = []
    for num in bytes_to_hex(content):
        h.append(num)

    varname_array = format_filename_to_varname(src_path.name, False)
    varname_len = format_filename_to_varname(src_path.name, True)

    with open(dest_path, "w") as file:
        file.write("unsigned char ")
        file.write(f"{varname_array}")
        file.write("[] = {\n")

        for i, hexval in enumerate(h, 1):
            file.write(f"{hexval}, ")
            if i % ROW_WIDTH == 0:
                file.write("\n")

        file.write("\n};\n")
        file.write(f"unsigned int {varname_len} = {len(h)};")

def print_usage() -> None:
    print("Usage:")
    print("$ ./bedh.py input.txt")
    sys.exit(0)

def process_args() -> Path:
    if len(sys.argv) <= 1:
        print("Error: no arguments were provided")
        print_usage()
    for a in sys.argv[1:]:
        match a:
            case "-h" | "--help":
                print_usage()
            case _:
                src_path = Path(a).resolve()
                if src_path.exists():
                    return src_path
                else:
                    print("Error: provided path does not exist")
                    print_usage()
    print_usage()
    sys.exit(0)

def derive_dest_path(p: Path) -> Path:
    dst_name = p.name.split(".")[:1]
    dst_name.append(".h")
    dst_path = p.with_name("".join(dst_name))
    return dst_path

def main():
    src_path = process_args()
    dest_path = derive_dest_path(src_path)

    process_hex(src_path, dest_path)
    print("(bedh.py) generated:")
    print(f"{dest_path}")

if __name__ == "__main__":
    main()
