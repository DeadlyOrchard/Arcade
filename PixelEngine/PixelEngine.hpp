#include "PixGame.hpp"
#include <stdio.h>
#include <SDL2/SDL.h>

class Engine {
private:
    bool _running;
    int _width, _height, _delay;
    SDL_Renderer* _ren;
    SDL_Window* _win;
    void _drawObject(GameObject, int);
public:
    Engine(int, int, int);
    void run();
};

