#!/usr/bin/env python3
"""Parse a Luma3DS exception dump and reconstruct a backtrace for PKSM.

Usage:
    tools/luma-crash-bt.py <crash_dump_XXXXXXXX.dmp> [PKSM.elf]

    The ELF defaults to 3ds/out/PKSM.elf relative to the repo root (the
    script's parent directory). The ELF MUST be from the same build that
    produced the crash, or every address will symbolize to garbage.

Where dumps come from:
    With Luma3DS's "Save exception dumps" enabled, a crash writes
    sdmc:/luma/dumps/arm11/crash_dump_XXXXXXXX.dmp. Copy it off the SD card.

How to read the output:
    - "pc" is where the fault happened; "far" is the address whose access
      faulted (data aborts only).
    - "lr" is only a real return address if the crash happened early in a
      function. Optimized ARM code reuses lr as a scratch register, so a
      garbage-looking lr (e.g. below 0x100000, outside .text) usually means
      lr holds a data value — often related to far. Don't trust it blindly.
    - The stack scan lists every word on the dumped stack that lands in the
      ELF's .text range. It's a heuristic backtrace: stale frames and spilled
      function pointers show up too, but the crashing call chain is almost
      always readable top-to-bottom (innermost first). Entries whose symbol
      offset is small (+0x0, +0x4) are likely function POINTERS, not return
      addresses.
    - Heap corruption pattern seen before: pc inside _free_r/_malloc_r with
      far pointing at a nonsense address = some earlier code corrupted the
      heap (double free, use-after-free, racing a destructor). The stack scan
      shows who called free, not who corrupted it.

Requires arm-none-eabi-nm / arm-none-eabi-addr2line (devkitARM), looked up in
PATH then /opt/devkitpro/devkitARM/bin.
"""

import bisect
import os
import shutil
import struct
import subprocess
import sys

EXCEPTION_TYPES = ["FIQ", "undefined instruction", "prefetch abort", "data abort"]
REG_NAMES = [
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11",
    "r12", "sp", "lr", "pc", "cpsr",
    # ARM11 only, present when registerDumpSize allows:
    "dfsr", "ifsr", "far", "fpexc", "fpinst", "fpinst2",
]


def find_tool(name):
    path = shutil.which(name) or f"/opt/devkitpro/devkitARM/bin/{name}"
    if not os.path.exists(path):
        sys.exit(f"error: {name} not found in PATH or /opt/devkitpro/devkitARM/bin")
    return path


def text_range(elf):
    readelf = find_tool("arm-none-eabi-readelf")
    out = subprocess.run([readelf, "-SW", elf], capture_output=True, text=True, check=True).stdout
    for line in out.splitlines():
        parts = line.replace("]", "] ").split()
        if ".text" in parts:
            i = parts.index(".text")
            addr, _off, size = (int(parts[i + 2], 16), parts[i + 3], int(parts[i + 4], 16))
            return addr, addr + size
    sys.exit("error: no .text section in ELF (wrong file?)")


def load_symbols(elf, lo, hi):
    nm = find_tool("arm-none-eabi-nm")
    out = subprocess.run([nm, "-n", elf], capture_output=True, text=True, check=True).stdout
    syms = []
    for line in out.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in "tTwW" and lo <= int(parts[0], 16) < hi:
            syms.append((int(parts[0], 16), parts[2]))
    if not syms:
        sys.exit("error: no code symbols in ELF (wrong file?)")
    syms.sort()
    return syms


def sym_for(syms, addr):
    i = bisect.bisect_right(syms, (addr, "\U0010ffff")) - 1
    if i < 0:
        return None
    return f"{syms[i][1]}+{addr - syms[i][0]:#x}"


def addr2line(elf, addrs):
    if not addrs:
        return {}
    tool = find_tool("arm-none-eabi-addr2line")
    out = subprocess.run(
        [tool, "-fCe", elf] + [f"{a:#x}" for a in addrs],
        capture_output=True, text=True, check=True).stdout.splitlines()
    result = {}
    for i, addr in enumerate(addrs):
        func, loc = out[2 * i], out[2 * i + 1]
        if len(func) > 100:  # demangled lambda wrappers can be screen-fillers
            func = func[:100] + "…"
        result[addr] = f"{func}  [{loc}]" if "??" not in loc else func
    return result


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    dump_path = sys.argv[1]
    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    elf = sys.argv[2] if len(sys.argv) > 2 else os.path.join(repo_root, "3ds/out/PKSM.elf")
    if not os.path.exists(elf):
        sys.exit(f"error: ELF not found: {elf} (build first, or pass it explicitly)")

    with open(dump_path, "rb") as dump:
        d = dump.read()
    if struct.unpack_from("<2I", d) != (0xDEADC0DE, 0xDEADCAFE):
        sys.exit("error: not a Luma3DS exception dump (bad magic)")
    (ver_min, ver_maj, processor, core, exc_type, _total,
     reg_size, code_size, stack_size, extra_size) = struct.unpack_from("<4H6I", d, 8)

    print(f"Luma3DS exception dump v{ver_maj}.{ver_min}")
    print(f"processor: ARM{processor} core {core}")
    print(f"exception: {EXCEPTION_TYPES[exc_type] if exc_type < 4 else exc_type}")

    regs = struct.unpack_from(f"<{reg_size // 4}I", d, 0x28)
    reg = dict(zip(REG_NAMES, regs))
    stack_off = 0x28 + reg_size + code_size
    extra = d[stack_off + stack_size: stack_off + stack_size + extra_size]
    if processor == 11 and extra_size >= 16:
        name = extra[:8].rstrip(b"\0").decode(errors="replace")
        tid = struct.unpack_from("<Q", extra, 8)[0]
        print(f"process:   {name} ({tid:#018x})")

    text_lo, text_hi = text_range(elf)
    syms = load_symbols(elf, text_lo, text_hi)

    print("\nregisters:")
    for n, v in zip(REG_NAMES, regs):
        s = sym_for(syms, v) if text_lo <= v < text_hi and n not in ("cpsr", "dfsr", "ifsr", "fpexc") else ""
        print(f"  {n:<7} {v:#010x}  {s}")
    if "lr" in reg and not (text_lo <= reg["lr"] < text_hi):
        print("  note: lr is outside .text -> scratch value, not a return address")

    # pc / lr with source lines
    key_addrs = [reg[r] for r in ("pc", "lr") if r in reg and text_lo <= reg[r] < text_hi]
    lines = addr2line(elf, key_addrs)
    for a in key_addrs:
        print(f"\n{a:#x}: {lines[a]}")

    # stack scan
    stack = d[stack_off: stack_off + stack_size]
    sp = reg.get("sp", 0)
    hits = []
    for i in range(0, len(stack) - 3, 4):
        v = struct.unpack_from("<I", stack, i)[0]
        if text_lo <= v < text_hi:
            hits.append((sp + i, v))
    print(f"\nstack scan ({len(hits)} code addresses on the dumped stack, innermost first):")
    locs = addr2line(elf, [v for _, v in hits])
    for addr, v in hits:
        print(f"  [sp+{addr - sp:#06x}] {v:#010x}  {sym_for(syms, v)}  {locs[v]}")


if __name__ == "__main__":
    main()
