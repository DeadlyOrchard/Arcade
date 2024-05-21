#include <stdio.h>
#include <SDL2/SDL.h>
#include <flecs.h>
#include <iostream>
#include "Arcade.hpp"
#include "Snake.hpp"

// rendering constants
const int WIDTH = 800;
const int HEIGHT = 600;
const int WIN_FLAGS = SDL_WINDOW_BORDERLESS;
const int NODE_SIZE = 20;

int main(int argc, char *argv[]) {
    // constants which are not known until main is called
    const std::string BASE_PATH = SDL_GetBasePath();
    flecs::world ecs(argc, argv);
    ecs.set_target_fps(12);

    // window, renderer, events, and text lib init
    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, WIN_FLAGS, &win, &ren);
    TTF_Init();
    if (win == nullptr) {
        std::cout << "Could not create window" << std::endl;
        return -1;
    }
    if (ren == nullptr) {
        std::cout << "Could not create renderer" << std::endl;
        return -1;
    }

    SnakeGame game = SnakeGame(ren, &ecs, WIDTH, HEIGHT, NODE_SIZE, BASE_PATH);
    ecs.system()
        .kind(flecs::OnUpdate)
        .iter([game](flecs::iter &it) {
            game.update();
        });
    
    SDL_Event e;
    bool running = true;
    while (game.getStatus()) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                game.exec(e.key.keysym.sym);
                break;
            }
        }
        ecs.progress();
    }
    return 0;
}