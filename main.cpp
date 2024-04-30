#include <stdio.h>
#include <SDL2/SDL.h>
#include <iostream>
#include "Snake.hpp"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }
    printf("SDL successfully initialized\n");

    // game constants
    const int WIDTH {800};
    const int HEIGHT {400};
    const int DELAY {20};
    enum BG_C { r = 33, g = 33, b = 33, a = 255 };
    SDL_Window *win = NULL;
    SDL_Renderer *ren = NULL;
    win = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS);
    ren = SDL_CreateRenderer(win, -1, NULL);
    bool quit = false;
    Snake snake = Snake(1, 1, 10, 'r', 0, 0);
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                case SDLK_RIGHT:
                    snake.turn('r');
                    break;
                case SDLK_LEFT:
                    snake.turn('l');
                    break;
                case SDLK_UP:
                    snake.turn('u');
                    break;
                case SDLK_DOWN:
                    snake.turn('d');
                    break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
                break;
            }
        }

        SDL_SetRenderDrawColor(ren, BG_C::r, BG_C::g, BG_C::b, BG_C::a);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawLine(ren, 0, 0, 10, 10);
        SDL_RenderPresent(ren);
        SDL_Delay(DELAY);
        snake.move();
    }
    SDL_Quit();
    return 0;
}
