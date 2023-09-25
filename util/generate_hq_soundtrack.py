#!/usr/bin/env python3
"""
Produce a high-quality (44 kHz stereo instead of 32 kHz mono) soundtrack
for the demo "State of Mind" by Bomb! by combining the in-demo low-quality
version with a better quality version of the (commercial) source material
downloaded from YouTube.

Requires FFmpeg and a recent version of yt-dlp to be installed and available
in the PATH.
"""
import subprocess
import argparse
import array
import sys
import os
import io

OUTPUT_FILE = "mind_hq.mp3"
DATA_FILE = "mind.dat"
DEMO_MUSIC_FILE = "mind.mp3"
ORIG_MUSIC_FILE = "mind_hq.dat"
ORIG_URL = "https://www.youtube.com/watch?v=5mNVnr5BKsA"
RATE = 48000
ORIG_RATE = 47899
ORIG_START =  5164469 * 2
ORIG_END   = 14863061 * 2
DEMO_START =  9702738 * 2
LD_FADE_IN =       10 + 1
FADE_IN    = 1 << LD_FADE_IN


def trace_cmd(*cmd):
    print("+", ' '.join(cmd))
    return list(cmd)


def try_run(*cmd):
    try:
        subprocess.run(trace_cmd(*cmd), stdout=subprocess.DEVNULL, check=True)
        return True
    except (EnvironmentError, subprocess.CalledProcessError) as e:
        print(e)
        return False


def decode_to_pcm(infile, rate):
    proc = subprocess.Popen(trace_cmd("ffmpeg", "-loglevel", "error", "-i", infile, "-ar", str(rate), "-ac", "2", "-f", "s16le", "-"), stdout=subprocess.PIPE)
    pcm = array.array('h')
    try:
        pcm.fromfile(proc.stdout, 99999999)
    except EOFError:
        pass  # will always hit, as we're reading the entire file
    proc.stdout.close()
    proc.wait()
    return pcm


def encode_to_mp3(data, outfile, rate):
    proc = subprocess.Popen(trace_cmd("ffmpeg", "-y", "-loglevel", "error", "-f", "s16le", "-ar", str(rate), "-ac", "2", "-i", "-", "-c:a", "libmp3lame", "-b:a", "192k", outfile), stdin=subprocess.PIPE)
    data.tofile(proc.stdin)
    proc.stdin.close()
    proc.wait()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("-f", "--force", action='store_true',
                        help="force re-downloading and re-processing")
    parser.add_argument("-k", "--keep", action='store_true',
                        help=f"keep intermediate files ({DEMO_MUSIC_FILE} and {ORIG_MUSIC_FILE})")
    args = parser.parse_args()

    os.chdir(os.path.join(os.path.dirname(sys.argv[0]), ".."))

    if os.path.isfile(OUTPUT_FILE) and not(args.force):
        print(f"{OUTPUT_FILE} already exists, nothing to do.")
        sys.exit(0)

    print("Checking prerequisites ...")
    ok = True
    if not(os.path.isfile(ORIG_MUSIC_FILE)) or args.force:
        if not try_run("yt-dlp", "--version"): ok = False
    if ok and not(try_run("ffmpeg", "-version")): ok = False
    if not ok:
        print("ERROR: yt-dlp and FFmpeg must be installed in available in the PATH.")
        sys.exit(1)

    if not(os.path.isfile(DEMO_MUSIC_FILE)) or args.force:
        print("Extracting demo soundtrack ...")
        if not os.path.isfile(DATA_FILE):
            print(f"ERROR: {DATA_FILE} not found.")
            sys.exit(1)
        with open(DATA_FILE, "rb") as data:
            while True:
                filename = data.readline().strip().decode('ascii')
                size = int(data.readline().strip())
                if filename == DEMO_MUSIC_FILE:
                    with open(DEMO_MUSIC_FILE, "wb") as f:
                        f.write(data.read(size))
                    break
                else:
                    data.seek(size, 1)

    print("Decoding demo soundtrack ...")
    demo = decode_to_pcm(DEMO_MUSIC_FILE, RATE)

    if not(os.path.isfile(ORIG_MUSIC_FILE)) or args.force:
        print("Downloading original soundtrack ...")
        subprocess.run(trace_cmd("yt-dlp", "-f", "bestaudio", "-o", ORIG_MUSIC_FILE, ORIG_URL), check=True)

    print("Decoding original soundtrack ...")
    orig = decode_to_pcm(ORIG_MUSIC_FILE, ORIG_RATE)

    print("Processing audio ...")
    final = array.array('h', (((i * x) >> LD_FADE_IN) for i, x in enumerate(orig[ORIG_START:ORIG_START+FADE_IN]))) \
          + orig[ORIG_START+FADE_IN:ORIG_END] + demo[DEMO_START:]

    print("Encoding to MP3 ...")
    encode_to_mp3(final, OUTPUT_FILE, RATE)

    if not args.keep:
        print("Removing temporary files ...")
        for f in (DEMO_MUSIC_FILE, ORIG_MUSIC_FILE):
            try:
                os.unlink(f)
            except EnvironmentError:
                pass

    print("Done.")
