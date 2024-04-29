#ifndef SNAKE_HPP
#define SNAKE_HPP

struct SnakeNode {
    int x;
    int y;
    SnakeNode* next;
};

class Snake {
private:
    int length;
    int speed;
    SnakeNode head;
    bool isGrowing = false;

public:
    Snake(int length, int speed, int x, int y);    // creates a snake
    int grow(); 
    void move(char direction);
    int getX();
    int getY();
};

#endif