#include "Snake.hpp"
#include <stdio.h>
#include <SDL2/SDL.h>

class Game {
private:
    bool running;
    int width;
    int height;
    int delay;
    SDL_Renderer* ren;
    SDL_Window* win;

public:
    Game(int width, int height, int delay);
    void run();
};