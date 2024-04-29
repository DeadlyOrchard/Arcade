# Repository Purpose
I'll be writing little arcade games using the gcc compiler and SDL2 libraries. The games will be created individually and then built into one executable.

## Build/Compile Instructions
- As of 4/28/2024,
- `cmd /c chcp 65001>nul && {compiler_dir} -fdiagnostics-color=always -g {working_dir}\main.cpp -I {SDL2_mingw32_dir}\include -L {SDL2_mingw32_dir}\lib -Wall -lmingw32 -lSDL2main -lSDL2 -o {out}\main.exe`
- Where
  - `{compiler_dir}` is the location of the c++ compiler,
  - `{working_dir}` is the location of the overall application folder,
  - `{SDL_mingw32_dir}` is the location of the SDL2 library built for mingw32,
    - After downloading [ SDL2-devel-2.30.2-mingw.zip ](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.2),
    - Use the location of the subfolder, "x86_64-w64-mingw32" for this path.
  - `{out}` is the location of the folder where the executable will be built.
