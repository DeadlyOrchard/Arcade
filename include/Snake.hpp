#pragma once

struct SnakeNode {
    int x;
    int y;
    SnakeNode* next;
};

class Snake {
private:
    bool isAlive;
    SnakeNode* head;
    int length;
    int currLength;
    int speed;
    char direction;
    int bounds[4];

public:
    Snake(int length, int speed, char direction, int x, int y, int minX, int minY, int maxX, int maxY);
    void grow(); 
    void turn(char direction);
    void update();
    SnakeNode* getHead();
    int getCurrentLength();
    bool getStatus();          // returns snake->isDead
};