#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <flecs.h>
#include <iostream>
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

    Snake snake = Snake(&ecs, WIDTH, HEIGHT, NODE_SIZE, BASE_PATH);

    // reset snake to default vals
    flecs::system snakeRestart = ecs.system()
        .iter([snake](flecs::iter &it) {
            snake.restart();
        });
    snakeRestart.disable();

    // save game and ask to play again
    flecs::system snakeEnd = ecs.system()
        .iter([snake](flecs::iter &it) {
            snake.end();
        });
    snakeEnd.disable();

    // snake movement, growing, and dying logic
    flecs::system snakeUpdate = ecs.system()
        .kind(flecs::OnUpdate)
        .iter([snake](flecs::iter &it) {
            snake.update();
        });
    
    // draw every text component that's enabled
    flecs::system drawText = ecs.system<Text>()
        .each([ren, BASE_PATH](const Text &t) {
            char* fontPath = (BASE_PATH + "PressStart.ttf").data();
            // get font
            TTF_Font* font = TTF_OpenFont(fontPath, t.fontSize);
            // create graphic
            SDL_Color offWhite = {155, 155, 155};
            SDL_Surface *surf = TTF_RenderText_Solid(font, t.text.data(), offWhite);
            SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
            SDL_Rect rect {t.pos.x, t.pos.y, 0, 0};
            TTF_SizeText(font, t.text.data(), &rect.w, &rect.h);
            rect.x -= rect.w / 2;
            rect.y -= rect.h / 2;
            SDL_RenderCopy(ren, tex, NULL, &rect);
            // free memory
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
            TTF_CloseFont(font);
        });

    // draw the snake
    flecs::system renDraw = ecs.system()
        .kind(flecs::PreStore)
        .iter([ren, snake, drawText] (flecs::iter &it) {
            SDL_SetRenderDrawColor(ren, 33, 33, 33, 255); // background color
            SDL_RenderClear(ren);
            std::vector<SDL_Vertex> obj = snake.getRenderData();
            SDL_RenderGeometry(ren, nullptr, obj.data(), obj.size(), nullptr, 0);
            drawText.run();
        });
    
    // present the renderer
    flecs::system renPresent = ecs.system<const Position>()
        .kind(flecs::OnStore)
        .iter([ren] (flecs::iter &it) {
            SDL_RenderPresent(ren);
        });
    
    SDL_Event e;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                Velocity v;
                switch(e.key.keysym.sym) {
                // quit
                case SDLK_ESCAPE:
                    running = false;
                    break;
                // restart
                case SDLK_SPACE:
                    if (!snakeUpdate.enabled()) {
                        snakeRestart.run();
                        snakeUpdate.enable();
                    }
                    break;
                default:
                    snake.exec(e.key.keysym.sym);
                }
                break;
            }
        }
        ecs.progress();
        if (!snake.isAlive()) {
            if (snakeUpdate.enabled()) {
                snakeUpdate.disable();
                snakeEnd.run();
            }
        }
    }
    return 0;
}