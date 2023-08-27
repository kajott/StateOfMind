#!/usr/bin/env python3
import sys
import os

OUTDIR = "mind_dat.unpacked"

if __name__ == "__main__":
    if not os.path.isdir(OUTDIR):
        os.makedirs(OUTDIR)

    with open("mind.dat", "rb") as f:
        while True:
            try:
                filename = f.readline().strip()
            except EOFError:
                break
            if not filename:
                break
            print(filename, end=' ')
            filename = filename.decode('ascii')
            size = int(f.readline().strip())
            print(f"({size} bytes)")
            with open(os.path.join(OUTDIR, filename), "wb") as d:
                d.write(f.read(size))
