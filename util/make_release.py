#!/usr/bin/env python3
import subprocess
import zipfile
import sys
import os

ARCHIVE = "mind_v13.zip"
EXE = "mind.exe" if (sys.platform == "win32") else "mind"
MANDATORY_FILES = ["mind.dat", EXE]
if sys.platform == "win32": MANDATORY_FILES += ["SDL2.dll"]
OPTIONAL_FILES = ["mind_hq.mp3", "file_id.diz"]

def run(*cmd, may_fail=False):
    print(os.getenv("PS4", "+ ") + ' '.join(cmd))
    try:
        subprocess.run(cmd, check=True)
    except Exception as e:
        print("FAILED:", e, file=sys.stderr)
        if not may_fail: sys.exit(1)
        return False
    return True

def rm_f(filename):
    print(os.getenv("PS4", "+ ") + "rm -f", filename)
    try:
        os.unlink(filename)
    except EnvironmentError:
        pass

if __name__ == "__main__":
    os.chdir(os.path.join(os.path.dirname(sys.argv[0]), ".."))

    if not os.path.exists(EXE):
        run("cmake", "-S.", "-Bbuild", "-DCMAKE_BUILD_TYPE=Release")
        run("cmake", "--build", "build", "--config", "Release")

    run("python3", "util/generate_hq_soundtrack.py", may_fail=True)

    rm_f(ARCHIVE)
    with zipfile.ZipFile(ARCHIVE, "w", compresslevel=9, allowZip64=False) as z:
        for f in MANDATORY_FILES + OPTIONAL_FILES:
            if (f in MANDATORY_FILES) and not(os.path.isfile(f)):
                print(f"ERROR: mandatory file '{f}' is missing")
            print(f, "->", ARCHIVE)
            z.write(f)
