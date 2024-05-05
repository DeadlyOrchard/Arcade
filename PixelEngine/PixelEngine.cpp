#include "PixelEngine.hpp"
#include "PixGame.hpp"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <iostream>

Engine::Engine(int width, int height, int delay) {
    _running = true;
    _width = width;
    _height = height;
    _delay = delay;
}

void Engine::run() {
    _win = NULL;
    _ren = NULL;
    _win = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, SDL_WINDOW_BORDERLESS);
    if (_win == NULL) {
        printf("Window could not be created! %s\n", SDL_GetError());
    }
    _ren = SDL_CreateRenderer(_win, -1, NULL);
    if (_ren == NULL) {
        printf("Renderer could not be created! %s\n", SDL_GetError());
    }

    Game g = Game(8);

    while (_running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                _running = false;
                break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    _running = false;
                    break;
                }
            }
        }
        SDL_SetRenderDrawColor(_ren, 0, 0, 0, 255);
        SDL_RenderClear(_ren);

        int pixSize = g.getPixelSize();
        int gameLen = g.getLen();
        for (int i = 0; i < gameLen; i++) {
            GameObject obj = g.process();
            _drawObject(obj, pixSize);
        }
        g.reset();
        g.update();
        SDL_RenderPresent(_ren);
        SDL_Delay(_delay);
    }
}

void Engine::_drawObject(GameObject obj, int pixSize) {
    SDL_Rect pixRect = SDL_Rect{0, 0, pixSize, pixSize};
    Pixel origin = obj.getOrigin();
    int objLen = obj.getLen();
    for (int j = 0; j < objLen; j++) {
        ColorManager cm = obj.process();
        Color c = cm.getColor();
        SDL_SetRenderDrawColor(_ren, c.r, c.g, c.b, c.a);
        int cmLen = cm.getLen();
        for (int k = 0; k < cmLen; k++) {
            Pixel p = cm.process();
            pixRect.x = origin.x + p.x * pixSize;
            pixRect.y = origin.y + p.y * pixSize;
            SDL_RenderFillRect(_ren, &pixRect);
        }
        cm.reset();
    }
    obj.reset();
}

int main(int argc, char* argv[]) {
    Engine eng = Engine(800, 600, 100);
    eng.run();
    return 0;
}