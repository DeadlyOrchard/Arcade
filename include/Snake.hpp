#pragma once

struct SnakeNode {
    int x;
    int y;
    SnakeNode* next;
};

class Snake {
private:
    SnakeNode* head;
    int length;
    int currLength;
    int speed;
    char direction;

public:
    Snake(int length, int speed, char direction, int x, int y);
    void grow(); 
    void turn(char direction);
    void move();
    SnakeNode* getHead();
    int getCurrentLength();
};