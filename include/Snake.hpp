#ifndef SNAKE_HPP
#define SNAKE_HPP

struct SnakeCell {
    int x;
    int y;
    SnakeCell* nextCell;
};

class Snake {
private:
    int length;
    int speed;
    SnakeCell head;
    bool isGrowing = false;

public:
    Snake(int length, int speed, int x, int y);    // creates a snake
    int grow(); 
    void move(char direction);
    int getX();
    int getY();
};

#endif