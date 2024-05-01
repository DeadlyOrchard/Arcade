#pragma once

struct SnakeNode {
    int x;
    int y;
    SnakeNode* next;
};

class Snake {
private:
    SnakeNode* head;
    SnakeNode* food;
    bool isAlive;
    int length;
    int currLength;
    int nodeSize;
    char direction;
    int bounds[4];
    SnakeNode* makeFood();

public:
    Snake(int length, int nodeSize, char direction, int x, int y, int minX, int minY, int maxX, int maxY);
    void grow(); 
    void turn(char direction);
    void update();
    int getCurrentLength();
    bool getStatus();          // returns snake->isDead
    SnakeNode* getHead();
    SnakeNode* getFood();
};