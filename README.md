# nm_objdump — ELF inspection utilities

A small collection of compact C utilities for inspecting ELF binaries, focused
on symbol listing and hex-dumping (a lightweight reimplementation and helpers
for `nm` / `objdump` functionality). This repository contains two primary
tools:

- `hnm` — an `nm`-style utility that prints symbols from 32-bit and 64-bit
	ELF files with proper endianness handling.
- `hobjdump` — a tiny wrapper that invokes the system `objdump` to display
	section and hex-dump information (mimics `objdump -s -f`).

These tools were implemented from the ground up for learning and auditing
ELF internals, and include helper routines for byte-swapping, hex-dumping,
and portable ELF parsing.

**Highlights**

- Supports both ELF32 and ELF64 formats.
- Handles big-endian and little-endian binaries automatically.
- Prints symbols in a readable layout similar to GNU `nm`.
- Includes `objdump`-style hex-dump formatting for sections.

**Build**

The project uses a simple Makefile located in the `nm_objdump` directory.
To build both utilities, run:

```bash
cd nm_objdump
make
```

This produces two binaries in the same directory: `hnm` and `hobjdump`.

**Usage**

- List symbols (like `nm`):

```bash
./hnm <path-to-elf> [more-files...]
```

Example:

```bash
./hnm /bin/ls
```

- Hex-dump and section info (wrapper for `objdump`):

```bash
./hobjdump <path-to-file>
```

`hobjdump` simply calls the system `/usr/bin/objdump -s -f <file>` and prints
the result. Use it when you want an easy, local call to `objdump`.

**Repository layout**

- `nm_objdump/` — main implementation directory and Makefile
	- `hnm.c`, `hnm.h` — `hnm` entry and ELF handler
	- `dnm_32.c`, `dnm_64.c` — dispatch/management for 32/64-bit symbol lists
	- `a32_*.c`, `a64_*.c` — helpers that determine symbol types/printing
	- `hobjdump.c` — small wrapper that invokes `/usr/bin/objdump`
	- `objdump_*.c`, `objdump_utils.c` — helpers for hex-dump formatting
	- `header.h`, `hobjdump.h`, `objdump.h` — shared headers
	- `Makefile` — build script

**Files of interest**

- `hnm` (source: `hnm.c`, `processor.c`) — core ELF parsing and symbol printing.
- `objdump_utils.c` — `od_hexdump()` prints data in `objdump -s` style.

**Notes & contributions**

This code is suitable for study, experimentation, and small projects where a
compact ELF inspection toolset is useful. If you plan to extend or reuse the
code, consider:

- adding command-line options (filtering, demangling, sorting),
- adding tests against a set of known ELF binaries, and
- adding documentation for edge cases (stripped binaries, corrupted headers).

If you'd like, I can:

- add CLI flags to `hnm` (filter by type/section),
- add unit tests and CI, or
- create a small example showing output on a few system binaries.

