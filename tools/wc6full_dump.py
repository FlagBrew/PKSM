#!/usr/bin/env python3
"""Dump the fields PKSM reads from a Gen-6 wondercard (.wc6 / .wc6full).

PKSM shows the *embedded card*, not the in-game description text that sits in
the .wc6full header. This prints exactly what PKSM parses so you can compare it
against what the card is supposed to contain.

Usage:
    python3 wc6full_dump.py card.wc6full [path/to/PKSM/core/strings/eng]

The optional second argument resolves species/move IDs to names. Without it,
numeric IDs are printed (still enough to verify the bug).
"""
import sys, struct, os

LANGS = {0: "None", 1: "JPN", 2: "ENG", 3: "FRE", 4: "ITA", 5: "GER",
         6: "?6", 7: "SPA", 8: "KOR", 9: "CHS", 10: "CHT"}


def load_names(strings_dir, fname):
    """core/strings text files are 0-indexed by line (line 1 == ID 0)."""
    try:
        with open(os.path.join(strings_dir, fname), encoding="utf-8") as f:
            return [l.rstrip("\n") for l in f]
    except OSError:
        return None


def name(table, idx):
    if table and 0 <= idx < len(table):
        return f"{idx} ({table[idx]})"
    return str(idx)


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    raw = open(sys.argv[1], "rb").read()
    strings_dir = sys.argv[2] if len(sys.argv) > 2 else None
    moves = load_names(strings_dir, "moves.txt") if strings_dir else None
    species = load_names(strings_dir, "species.txt") if strings_dir else None

    # Detect format: a "full" gift has a 0x208-byte header before the card.
    if len(raw) >= 0x310:
        base, kind = 0x208, "full (.wc6full)"
    elif len(raw) >= 0x108:
        base, kind = 0x000, "raw (.wc6)"
    else:
        sys.exit(f"File too small ({len(raw)} bytes) to be a WC6.")
    c = raw[base:]

    u16 = lambda o: struct.unpack_from("<H", c, o)[0]
    u32 = lambda o: struct.unpack_from("<I", c, o)[0]

    def utf16(o, units):
        s = c[o:o + units * 2].decode("utf-16-le", "replace")
        return s.split("\x00", 1)[0]

    print(f"File          : {os.path.basename(sys.argv[1])} ({len(raw)} bytes)")
    print(f"Format        : {kind}, card base @ 0x{base:X}")
    print(f"Card ID       : {u16(0x00)}")
    print(f"Title  (0x02) : {utf16(0x02, 36)!r}")
    lang = c[0x85]
    print(f"Language(0x85): {lang} ({LANGS.get(lang, '?')})")
    print(f"Species(0x82) : {name(species, u16(0x82))}")
    print(f"Form   (0x84) : {c[0x84]}")
    print(f"Nickname(0x86): {utf16(0x86, 12)!r}")
    print(f"OT name(0xB6) : {utf16(0xB6, 12)!r}")
    print(f"Held item(0x78): {u16(0x78)}")
    for m in range(4):
        print(f"Move {m+1} (0x{0x7A + m*2:02X}): {name(moves, u16(0x7A + m * 2))}")
    for m in range(4):
        print(f"Relearn {m+1}(0x{0xD8 + m*2:02X}): {name(moves, u16(0xD8 + m * 2))}")


if __name__ == "__main__":
    main()
