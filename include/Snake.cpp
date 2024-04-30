#include "Snake.hpp"
#include <iostream>

Snake::Snake(int length, int speed, char direction, int x, int y) {
    this->speed = speed;
    this->direction = direction;
    this->head = new SnakeNode;
    this->head->x = x;
    this->head->y = y;
    this->head->next = nullptr;
    this->length = length;
    this->currLength = 1;
}

SnakeNode* Snake::getHead() {
    return this->head;
}

int Snake::getCurrentLength() {
    return this->currLength;
}

void Snake::grow() {
    this->length++;
}

void Snake::turn(char direction) {
    this->direction = direction;
}

void Snake::move() {
    // create new head, then remove the last node unless the snake needs to grow
    // first, find the new x and y
    int x;
    int y;
    switch (this->direction) {
    case 'r': // right
        x = this->head->x + this->speed;
        y = this->head->y;
        break;
    case 'l': // left
        x = this->head->x - this->speed;
        y = this->head->y;
        break;
    case 'd': // down
        y = this->head->y + this->speed;
        x = this->head->x;
        break;
    case 'u': // up
        y = this->head->y - this->speed;
        x = this->head->x;
        break;
    }
    
    // create newNode and make it the head of the list
    SnakeNode* newHead = new SnakeNode;
    newHead->x = x;
    newHead->y = y;
    newHead->next = this->head;
    this->head = newHead;
    this->currLength++;

    // if the snake shouldn't grow -> remove the last node
    if (this->currLength > this->length) {
        SnakeNode* curr = this->head;
        for (int i = 0; i < this->length; i++) {
            curr = curr->next;
        }
        curr->next = nullptr;
        this->currLength--;
    }
}
