#include "Snake.hpp"

Snake::Snake(int length, int speed, int x, int y) {
    this->length = length;
    this->speed = speed;
    head.x = x;
    head.y = y;
    head.nextCell = nullptr;
}

void Snake::move(char direction) {
    SnakeCell curr = this->head;
    while (head.nextCell != nullptr) {
        curr = *head.nextCell;
    }
    SnakeCell next;
    switch (direction) {
        case 'r': // right
        next.x = curr.x + this->speed;
        break;
        case 'l': // left
        next.x = curr.x - this->speed;
        break;
        case 'u': // up
        next.y = curr.y + this->speed;
        break;
        case 'd': // down
        next.y = curr.y - this->speed;
        break;
    }
    curr.nextCell = &next;
    if (!this->isGrowing) {
        this->head = *this->head.nextCell;
    }
}

int Snake::getX() {
    return this->head.x;
}

int Snake::getY() {
    return this->head.y;
}