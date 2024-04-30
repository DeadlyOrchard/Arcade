#pragma once
#include <stdio.h>
#include <SDL2/SDL.h>

struct SnakeNode {
    int x;
    int y;
    SnakeNode* next;
};

class Snake {
private:
    SnakeNode head;
    int length;
    int speed;
    int nodeSize;
    char direction;
    bool isGrowing = false;

public:
    Snake(int length, int speed, int nodeSize, char direction, int x, int y);
    void grow(); 
    void turn(char direction);
    void move();
    void draw(SDL_Renderer* r);
};