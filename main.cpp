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
    Snake snake = Snake(1, 1, 0, 0);
    std::cout << "X: " << snake.getX();
    std::cout << ", Y: " << snake.getY();
    char directions[4] = {'r', 'l', 'u', 'd'};
    std::cout << std::endl;
    for (char direction : directions) {
        snake.move(direction);
        std::cout << "Move: " << direction << '\n';
        std::cout << "X: " << snake.getX();
        std::cout << ", Y: " << snake.getY() << '\n';
    }
    SDL_Quit();
    return 0;
}