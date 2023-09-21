# Bomb! - State Of Mind (SDL port)

This repository contains a source port of the classic (1998) demo
[State Of Mind](https://www.pouet.net/prod.php?which=26)
by the French demogroup [Bomb!](https://demozoo.org/groups/127/).
It is based on the source code that has been released in 2003, and is targeted
to run on modern desktop systems.

## Features

- builds with reasonably modern compilers
- runs on reasonably modern operating systems
- native 64-bit binary, no emulation
- uses SDL 2.0 and OpenGL for display and sound
- "antialiased blocky" upscaling from 320x200 to desktop resolution
  on OpenGL 3.3 and up
- tested on
  - Windows 10 (x64_64)
  - Linux (Ubuntu 22.04, x64_64)
  - Raspberry Pi 4 (64-bit Raspberry Pi OS)

## Bugs

- some graphical glitches in the "liquid tunnel" scene near the end
- textures pop in a bit late in the canyon scene

## TODOs

- fix the bugs
- create a high-quality (44.1 or 48 kHz stereo instead of 32 kHz mono) soundtrack
  by combining the demo's music file with the
  [source](https://soundcloud.com/senserband/states-of-mind)
  [material](https://www.youtube.com/watch?v=5mNVnr5BKsA)
- improve lyrics timing


## FAQs

### Why?

In essence, I just wanted to watch the demo without resorting to DOSBox.
Since nobody else did a modern port, I had to do it myself.

### How?

The demo has originally been written for 1998-era Pentium 1 systems running DOS,
and as was common at the time (and also pretty much required for performance
reasons), large parts of it are implemented in x86 assembly code.
This would have made it non-trivial to port it to modern 32-bit x86 systems,
very hard to get it going on x64_64, and would have required a full rewrite
for anything else (e.g. Arm).

Fortunately, the demo's original programmer was (and maybe still is) a devoted
Unix user; in fact, the demo ran on Linux from day one, albeit using ancient
APIs like SVGALib, DGA and OSS, which are of no use today.
He also ported the demo to various Unix workstations, using plain C
implementations for everything. While I'm not aware that the Sun Solaris (SPARC)
or SGI IRIX (MIPS) versions have ever been released in binary form,
their source code is included in the 2003 source code release.

That plain C code is what this port is based on. My contributions were to:
- create a CMake-based build system instead of the old Makefile contraptions
- make the code believe it's running on a generic Unix machine
- make the code believe that said Unix machines are little-endian
  (as all relevant CPU architectures today are)
- implement a display output driver based on SDL and OpenGL
- implement a sound output driver based on SDL
- fix the (surprisingly few!) portability and 64-bit compatibility bugs
- work around a bunch of weird bugs

### You're displaying it in the wrong aspect ratio! It's supposed to be 4:3!

I'm not, and it's not. The demo has been designed with square pixels in mind,
as is evident e.g. in the Pepsi logo background of the metaballs scene.
The original DOS version also sets a 320x240 resolution with letterboxing.

### The sync between the soundtrack's lyrics and the words on screen seems off?

It may seem like that, but after reviewing the original demo multiple times in
various emulators and watching
[YouTube captures](https://www.youtube.com/watch?v=BGip9eq1enU)
from actual hardware, I came to the conclusion that the timing was never
as tight as I remembered it being. There are some words that are perfectly
spot-on, but others are off by more than a beat.

It also doesn't help that the demo never made any attempts to synchronize
audio and video: The video timer is started at the beginning, and immediately
after that, audio playback starts, and nobody ever looks at its progress after
that.


## Build Instructions

CMake and a decent compiler (GCC 11 and 12 as well as MSVC 19 have been tested) is required. When not compiling for Windows with MSVC, the SDL2 development
libraries also need to be installed. On Debian, Ubuntu and derivatives,
all prerequisited can be installed with

    sudo apt install build-essential cmake libsdl2-dev

Building is then done with

    cmake -S. -Dbuild -DCMAKE_BUILD_TYPE=Release
    cmake --build build

The binary will be put into the project's base directory, not the usual
CMake build subdirectory.

The required data file is downloaded automatically during the build process,
as is the SDL2 SDK on Windows+MSVC.
