#pragma once
#include <flecs.h>
#include <vector>
#include <SDL2/SDL.h>
#include <iostream>
#include "Arcade.hpp"

class Snake : public Scene {
private:
    flecs::entity _head, _body, _food, _score;
    int _nodeSize;
    RenderData _nodeLocal;
public:
    Snake(SDL_Renderer*, flecs::world*, int, int, int, std::string);
    void reset() const;
    void update() const;
    void renderUpdate() const;
    void exec(int) const; // execute an input
    bool isGameOver() const; // returns true if snake dead, false otherwise
};

class SnakeGame : Game {
public:
    SnakeGame(SDL_Renderer*, flecs::world*, int, int, int, std::string);
    void exec(int) const;
    void update() const;
};