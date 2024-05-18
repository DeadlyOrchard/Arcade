# General
- Goal: an arcade cabinet program
- In the works:
  - Snake Game
- Tools
  - GNU compilers GCC and G++
  - [ SDL2 Library ](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.2)
    - [SDL2_ttf Library ](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.22.0)
  - [ Flecs Framework ](https://github.com/SanderMertens/flecs)

## Build/Compile Instructions
- As of 5/18/2024,
  - The flecs framework is already in the include directory, but the SDL2 and SDL2_ttf library must be installed.
    - In building this project, I'm using: 
      - `SDL2-devel-2.30.2-win32-x86.zip` from the SDL2 release linked above. 
      - `SDL2_ttf-devel-2.22.0-win32-x64.zip` from the SDL2_ttf release linked above.
  - On Windows, 
    - After cloning the repo, the file `.\Arcade\src\build.bat` will do most of the heavy lifting. There are two changes that need to be done.
      - In `.\Arcade\src\build.bat`,
        - Assign SDL2 (line 15) with the absolute path to SDL2 on your system
        - Assign SDL2_ttf (line 16) with the absolute path to SDL2_ttf on your system
    - Then, navigate to `.\Arcade` in the terminal and run `.\src\build.bat`.
    - After supplying the `.\Arcade\bin` with the necessary `.dll` files,
      - `SDL2.dll`
      - `SDL2_ttf.dll`
    - `.\Arcade\bin\application.exe` should be ready!
