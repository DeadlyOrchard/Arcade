#include "Snake.hpp"
#include <iostream>

Snake::Snake(int length, int speed, int x, int y) {
    this->length = length;
    this->speed = speed;
    head.x = x;
    head.y = y;
    head.next = nullptr;
}

void Snake::move(char direction) {
    // create new head, then remove the last node unless the snake isGrowing
    SnakeNode newNode;
    switch (direction) {
        case 'r': // right
        newNode.x = this->head.x + this->speed;
        newNode.y = this->head.y;
        break;
        case 'l': // left
        newNode.x = this->head.x - this->speed;
        newNode.y = this->head.y;
        break;
        case 'u': // up
        newNode.y = this->head.y + this->speed;
        newNode.x = this->head.x;
        break;
        case 'd': // down
        newNode.y = this->head.y - this->speed;
        newNode.x = this->head.x;
        break;
    }
    newNode.next = &this->head;
    this->head = newNode;
    this->length++;
    if (!this->isGrowing) {
        SnakeNode curr = this->head;
        for (int i = 0; i < this->length - 1; i++) {
            curr = *curr.next;
        }
        curr.next = nullptr;
        this->length--;
    } 
}

int Snake::getX() {
    return this->head.x;
}

int Snake::getY() {
    return this->head.y;
}