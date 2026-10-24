#!/usr/bin/env python3
"""Carve the XNU kernel (com.apple.kernel fileset entry) exec segments from a
full top-level kernelcache macho, plus find a panic-string VMA inside it.

The fileset entry's segment fileoffs are ABSOLUTE (into the container file),
so we read from the container directly. Saves: kernel_text_<tag>.bin,
kernel_text_<tag>.json {segs: [{name, vmaddr, size, fileoff}]}.

Usage: carve_kernel_text.py <container.macho> <tag> [find_string]
"""
import json
import struct
import sys

def lc_iter(macho, base=0):
    """Yield (cmd, cmdsize, abs_cmd_off) for macho header at abs offset base."""
    ncmds, sizeofcmds = struct.unpack_from("<II", macho, base + 16)
    off = base + 32
    for _ in range(ncmds):
        cmd, cmdsize = struct.unpack_from("<II", macho, off)
        yield cmd, cmdsize, off
        off += cmdsize
        if off > base + 32 + sizeofcmds:
            break

def main():
    path = sys.argv[1]
    tag = sys.argv[2]
    find_s = sys.argv[3].encode() if len(sys.argv) > 3 else None
    data = open(path, "rb").read()
    out = {"segs": []}
    texts = []
    koff = None
    for cmd, cmdsize, off in lc_iter(data):
        if cmd == 0x80000035:  # LC_FILESET_ENTRY
            vmaddr, fileoff = struct.unpack_from("<QQ", data, off + 8)
            stroff = struct.unpack_from("<I", data, off + 24)[0]
            end = data.index(b"\x00", off + stroff)
            nm = data[off + stroff:end].decode()
            if nm == "com.apple.kernel":
                koff = fileoff
                kbase = vmaddr
                print(f"kernel entry: fileoff {fileoff:#x} vmaddr {vmaddr:#x}")
                break
    if koff is None:
        print("com.apple.kernel entry NOT FOUND")
        return 1
    # walk the kernel entry's load commands
    for cmd, cmdsize, off in lc_iter(data, koff):
        if cmd == 0x19:  # LC_SEGMENT_64
            segname = data[off+8:off+24].rstrip(b"\x00").decode()
            vmaddr, vmsize, fileoff, filesize = struct.unpack_from("<QQQQ", data, off+24)
            out["segs"].append({"name": segname, "vmaddr": vmaddr,
                                "vmsize": vmsize, "fileoff": fileoff,
                                "filesize": filesize})
            print(f"  {segname:14s} vmaddr {vmaddr:#018x} vmsize {vmsize:#x} "
                  f"fileoff {fileoff:#x} filesz {filesize:#x}")
            if segname in ("__TEXT_EXEC", "__TEXT"):
                # extract the segment bytes (fileoff absolute into container)
                end = min(fileoff + filesize, len(data))
                seg = data[fileoff:end]
                fn = f"kernel_{segname.strip('_').lower()}_{tag}.bin"
                open(fn, "wb").write(seg)
                print(f"    -> wrote {fn} ({len(seg):#x} bytes)")
                texts.append((segname, vmaddr, fn))
                if find_s:
                    idx = seg.find(find_s)
                    if idx >= 0:
                        print(f"    *** STRING {find_s!r} @ VMA {vmaddr + idx:#x}")
                        out["string_vma"] = vmaddr + idx
    if find_s and "string_vma" not in out:
        # also search every segment regardless of extraction
        for s in out["segs"]:
            seg = data[s["fileoff"]:min(s["fileoff"] + s["filesize"], len(data))]
            idx = seg.find(find_s)
            if idx >= 0:
                print(f"    *** STRING {find_s!r} in {s['name']} @ VMA {s['vmaddr'] + idx:#x}")
                out.setdefault("string_vma", s["vmaddr"] + idx)
                break
    open(f"kernel_text_{tag}.json", "w").write(json.dumps(out, indent=1))
    return 0

if __name__ == "__main__":
    sys.exit(main())
