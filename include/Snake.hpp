#pragma once
#include <flecs.h>
#include <time.h>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

struct Position {
    int x, y;
};

struct Velocity {
    int x, y;
};

struct Body {
    std::vector<Position> list;
    bool alive;
};

struct GameData {
    std::string gameName;
    time_t start, end;
    int score;
};

struct Text {
    std::string text;
    int fontSize;
    Position pos;
};

class Snake {
private:
    flecs::entity _head;
    flecs::entity _body;
    flecs::entity _food;
    flecs::entity _gameOver;
    flecs::entity _data;
    int _width, _height, _wCenter, _hCenter, _nodeSize;
    char *_fontPath;
public:
    Snake(flecs::world*, int, int, int, std::string);
    void update() const; // move snake forward in time
    void restart() const; // reset snake to default settings
    void end() const; // show gameover screen
    std::vector<SDL_Vertex> getRenderData() const; // get necessary data for rendering
    bool isAlive() const;
    void exec(int) const; // execute an input
};