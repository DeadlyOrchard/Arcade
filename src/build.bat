SET in_main=%cd%\src\main.cpp
SET in_arcade=%cd%\include\Arcade.cpp
SET in_snake=%cd%\include\Snake.cpp
SET in_flecs=%cd%\include\flecs.c

SET out_main=%cd%\bin\main.o
SET out_arcade=%cd%\bin\Arcade.o
SET out_snake=%cd%\bin\Snake.o
SET out_flecs=%cd%\bin\flecs.o
SET out_app=%cd%\bin\application

SET custom_include=-I%cd%\include

:: CHANGE THESE LINES
SET SDL2=C:\DevDependencies\SDL2-2.30.2-x86_64-w64-mingw32
SET SDL2_ttf=C:\DevDependencies\SDL2_ttf-2.22.0-x86_64-w64-mingw32

SET SDL2_include=-I%SDL2%\include

SET SDL2_lib=-L%SDL2%\lib
SET SDL2_ttf_lib=-L%SDL2_ttf%\lib

gcc -c %in_main% -o %out_main% %custom_include% %SDL2_include%
gcc -c %in_arcade% -o %out_arcade% %custom_include% %SDL2_include%
gcc -c %in_snake% -o %out_snake% %custom_include% %SDL2_include%
gcc -c %in_flecs% -o %out_flecs%
g++ %out_main% %out_arcade% %out_snake% %out_flecs% -o %out_app% %SDL2_lib% %SDL2_ttf_lib% -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lWs2_32