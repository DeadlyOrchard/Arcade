#include "Game.hpp"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <iostream>


Game::Game(int width, int height, int delay) {
    this->running = true;
    this->width = width;
    this->height = height;
    this->delay = delay;
}

void Game::run() {
    this->win = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, NULL);
    this->ren = SDL_CreateRenderer(this->win, -1, NULL);
    const int snakeSize = 20;
    const int snakeLength = 8;
    Snake snake = Snake(snakeLength, snakeSize, 'r', 400, 300, 0, 0, this->width - snakeSize, this->height - snakeSize);
    while (this->running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                this->running = false;
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
                    this->running = false;
                    break;
                }
                break;
            }
        }
        SDL_SetRenderDrawColor(this->ren, 33, 33, 33, 255);
        SDL_RenderClear(this->ren);
        SDL_SetRenderDrawColor(this->ren, 255, 255, 255, 255);
        SDL_Rect rect {0, 0, snakeSize, snakeSize};
        int len = snake.getCurrentLength();
        SnakeNode* curr = snake.getHead();
        for (int i = 0; i < len; i++) {
            rect.x = curr->x;
            rect.y = curr->y;
            SDL_RenderDrawRect(this->ren, &rect);
            if (curr->next != nullptr) {
                curr = curr->next;
            }
        }
        SDL_RenderPresent(this->ren);
        if (snake.getStatus()) {
            snake.update();
        }
        SDL_Delay(this->delay);
    }
}

int main(int argc, char* argv[]) {
    Game game = Game(800, 600, 100);
    game.run();
    return 0;
}