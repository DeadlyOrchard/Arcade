# Repository Purpose
- I'll be writing little arcade games using the gcc compiler and SDL2 libraries. The games will be created individually and then built into one executable.
- One major goal for this project is to make all of it from scratch save for the graphics library, SDL2.
  - That means writing my own memory management for basically the entire project so keep that in mind if you're thinking of installing. 
- The first game to be added to the arcade is Snake

## Build/Compile Instructions
- As of 5/02/2024,
- `${compiler_loc} -g ${workspaceFolder}\main.cpp -I ${SDL2_mingw32_dir}\include -L ${SDL2_mingw32_dir}\lib -lmingw32 -lSDL2main -lSDL2 ${workspaceFolder}\include\*.cpp -o ${out}\main.exe`
- Where
  - `{compiler_loc}` is the location of the c++ compiler,
    - Note: best to use g++ compiler to avoid linker errors.
  - `{workspaceFolder}` is the location of the overall application folder,
  - `{SDL_mingw32_dir}` is the location of the SDL2 library built for mingw32,
    - After downloading [ SDL2-devel-2.30.2-mingw.zip ](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.2),
    - Use the location of the subfolder, "x86_64-w64-mingw32" for this path.
  - `{out}` is the location of the folder where the executable will be built.
